/*
 * NFC Attendance Box — Arduino UNO R4
 * - MFRC522 (SPI) on SS=10, RST=9
 * - microSD (SPI) on CS=4
 * - RTC (Uno R4) for exact time stamps
 * CSV inputs (on SD):
 *   students.csv : UID_HEX;NOM_PRENOM;CODE_GROUPE
 *   slots.csv    : YYYY-MM-DD;HH:MM;HH:MM;CODE_PROF;CODE_GROUPE;COURS;SALLE
 * Output per session (auto): ATTEND_YYYYMMDD_HHMM_HHMM_PROF.csv
 *   timestamp;uid;name;group;status;course;room
 */

#include <SPI.h>
#include <SD.h>
#include <MFRC522.h>
#include <RTC.h>   // Arduino UNO R4 RTC

// ----------------- Pins -----------------
#define PIN_SD_CS   4
#define PIN_RFID_SS 10
#define PIN_RFID_RST 9
#define PIN_LED_OK   6
#define PIN_LED_ERR  5
#define PIN_BUZZER   3

// ----------------- Config -----------------
const int CODE_PROF_BOITIER = 24718540;     // ← code prof attitré au boîtier
const unsigned long DEBOUNCE_MS = 1500;     // anti rebadge immédiat (ms)
const uint8_t MAX_SESSIONS = 32;            // sessions du jour (après filtrage)
const uint8_t MAX_GROUPS_PER_SESSION = 8;   // groupes par créneau
const uint16_t MAX_SEEN_PER_SESSION = 800;  // présents max par créneau (sécurité)

// ----------------- Globals -----------------
MFRC522 mfrc522(PIN_RFID_SS, PIN_RFID_RST);
RTC rtc;

// Structure créneau lu depuis slots.csv
struct Slot {
  String date;    // "YYYY-MM-DD"
  int startMin;   // minutes depuis 00:00
  int endMin;     // minutes depuis 00:00
  int codeProf;
  int codeGroupe;
  String course;
  String room;
};

// État consolidé d’un créneau (union de groupes)
struct SessionState {
  String key;                // date + start + end + prof
  String date;
  int startMin;
  int endMin;
  int codeProf;
  String course;
  String room;
  int groups[MAX_GROUPS_PER_SESSION];
  uint8_t groupCount = 0;

  String seenUIDs[MAX_SEEN_PER_SESSION];
  uint16_t seenCount = 0;
  bool finalized = false;
};

SessionState sessions[MAX_SESSIONS];
uint8_t sessionCount = 0;

// Rebadge immédiat (anti bounce carte)
String lastUID = "";
unsigned long lastScanMs = 0;

// ----------------- Utils -----------------
int hhmmToMin(const String &hhmm) {
  int h = hhmm.substring(0,2).toInt();
  int m = hhmm.substring(3,5).toInt();
  return h*60 + m;
}
String minToHHMM(int total) {
  char buf[6];
  int h = (total/60)%24;
  int m = total%60;
  snprintf(buf, sizeof(buf), "%02d:%02d", h, m);
  return String(buf);
}
String twoDigits(int v) { char b[3]; snprintf(b,3,"%02d",v); return String(b); }

// Crée une clé unique pour un créneau (prof+date+heures)
String makeSessionKey(const String& date, int startMin, int endMin, int prof) {
  return date + "_" + minToHHMM(startMin) + "_" + minToHHMM(endMin) + "_" + String(prof);
}

// Retourne YYYY-MM-DD et HH:MM depuis la RTC
void nowStrings(String &date, String &hhmm) {
  RTCTime t;
  rtc.getTime(t);
  date = String(t.getYear()) + "-" + twoDigits((int)t.getMonth()) + "-" + twoDigits(t.getDayOfMonth());
  hhmm = twoDigits(t.getHour()) + ":" + twoDigits(t.getMinutes());
}
int nowMinutes() {
  RTCTime t; rtc.getTime(t);
  return ((int)t.getHour())*60 + (int)t.getMinutes();
}

// Initialise la RTC à l’heure de compilation au boot
void rtcInitFromCompile() {
  rtc.begin();
  // Parse __DATE__ "Mmm dd yyyy" and __TIME__ "hh:mm:ss"
  const char* months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char mmm[4]; int dd, yyyy, hh, mm, ss;
  sscanf(__DATE__, "%3s %d %d", mmm, &dd, &yyyy);
  sscanf(__TIME__, "%d:%d:%d", &hh, &mm, &ss);
  int monthNum = (strstr(months, mmm) - months) / 3 + 1;

  RTCTime compiled((uint8_t)dd, (Month)monthNum, (uint16_t)yyyy, (uint8_t)hh, (uint8_t)mm, (uint8_t)ss);
  rtc.setTime(compiled);
}

// LED/Buzzer helpers
void signalOK()   { digitalWrite(PIN_LED_OK, HIGH);  tone(PIN_BUZZER, 1200, 80); delay(120); digitalWrite(PIN_LED_OK, LOW); }
void signalERR()  { digitalWrite(PIN_LED_ERR, HIGH); tone(PIN_BUZZER, 300, 200); delay(200); digitalWrite(PIN_LED_ERR, LOW); }

// ----------------- CSV parsing -----------------
bool splitCSVLine(const String& line, char sep, String out[], int expected) {
  int idx = 0, start = 0;
  for (int i=0; i<(int)line.length() && idx<expected; ++i) {
    if (line[i] == '\r' || line[i] == '\n') break;
    if (line[i] == sep) {
      out[idx++] = line.substring(start, i);
      start = i+1;
    }
  }
  if (idx < expected-1) {
    // last field
    out[idx++] = line.substring(start);
  }
  return idx == expected;
}

// Ajoute (ou fusionne) une ligne de slots.csv dans sessions[]
void addOrMergeSlot(const Slot& s) {
  if (s.codeProf != CODE_PROF_BOITIER) return; // on ne charge que les créneaux du prof du boîtier

  String key = makeSessionKey(s.date, s.startMin, s.endMin, s.codeProf);

  // existe déjà ?
  for (uint8_t i=0;i<sessionCount;i++) {
    if (sessions[i].key == key) {
      // fusionner groupe
      if (sessions[i].groupCount < MAX_GROUPS_PER_SESSION) {
        sessions[i].groups[sessions[i].groupCount++] = s.codeGroupe;
      }
      // course/room (on garde le premier non vide)
      if (sessions[i].course.length()==0) sessions[i].course = s.course;
      if (sessions[i].room.length()==0)   sessions[i].room   = s.room;
      return;
    }
  }
  // nouvelle session
  if (sessionCount >= MAX_SESSIONS) return;
  SessionState &ss = sessions[sessionCount++];
  ss.key      = key;
  ss.date     = s.date;
  ss.startMin = s.startMin;
  ss.endMin   = s.endMin;
  ss.codeProf = s.codeProf;
  ss.course   = s.course;
  ss.room     = s.room;
  ss.groups[0]= s.codeGroupe;
  ss.groupCount = 1;
}

// Charge slots.csv (uniquement les dates du jour pour économiser RAM)
void loadSlotsToday() {
  String today, t; nowStrings(today, t);

  File f = SD.open("/slots.csv", FILE_READ);
  if (!f) { Serial.println(F("ERR: slots.csv missing")); return; }

  String line;
  while (f.available()) {
    line = f.readStringUntil('\n');
    if (line.length() < 8) continue;
    String cols[7];
    if (!splitCSVLine(line, ';', cols, 7)) continue;
    // date;start;end;codeProf;codeGroupe;course;room
    if (cols[0] != today) continue;

    Slot s;
    s.date = cols[0];
    s.startMin = hhmmToMin(cols[1]);
    s.endMin   = hhmmToMin(cols[2]);
    s.codeProf = cols[3].toInt();
    s.codeGroupe = cols[4].toInt();
    s.course = cols[5];
    s.room   = cols[6];
    addOrMergeSlot(s);
  }
  f.close();
  Serial.print(F("Loaded sessions: ")); Serial.println(sessionCount);
}

// Lookup UID → (name, group). Renvoie true si trouvé
bool lookupStudentByUID(const String& uid, String &name, int &groupCode) {
  File f = SD.open("/students.csv", FILE_READ);
  if (!f) { Serial.println(F("ERR: students.csv missing")); return false; }
  String line;
  while (f.available()) {
    line = f.readStringUntil('\n');
    if (line.length() < 6) continue;
    String cols[3];
    if (!splitCSVLine(line, ';', cols, 3)) continue;
    if (cols[0].equalsIgnoreCase(uid)) {
      name = cols[1];
      groupCode = cols[2].toInt();
      f.close();
      return true;
    }
  }
  f.close();
  return false;
}

// Est-ce que groupCode ∈ groupes de la session ?
bool groupInSession(const SessionState &ss, int groupCode) {
  for (uint8_t i=0;i<ss.groupCount;i++) if (ss.groups[i]==groupCode) return true;
  return false;
}

// Trouve session(s) actives à l’instant
int findActiveSessions(uint8_t outIdx[], int maxOut) {
  String today, hhmm; nowStrings(today, hhmm);
  int nowM = nowMinutes();
  int n=0;
  for (uint8_t i=0;i<sessionCount;i++) {
    if (sessions[i].finalized) continue;
    if (sessions[i].date == today && nowM >= sessions[i].startMin && nowM <= sessions[i].endMin) {
      if (n<maxOut) outIdx[n++] = i;
    }
  }
  return n;
}

// Empêche double badge : si déjà vu dans ce créneau
bool alreadySeen(SessionState &ss, const String& uid) {
  for (uint16_t i=0;i<ss.seenCount;i++) if (ss.seenUIDs[i]==uid) return true;
  return false;
}
void markSeen(SessionState &ss, const String& uid) {
  if (ss.seenCount < MAX_SEEN_PER_SESSION) ss.seenUIDs[ss.seenCount++] = uid;
}

// Nom du fichier CSV de présence du créneau
String attendanceFilename(const SessionState &ss) {
  return String("/ATTEND_") + ss.date + "_" + minToHHMM(ss.startMin) + "_" + minToHHMM(ss.endMin) + "_" + ss.codeProf + ".csv";
}

// Append ligne CSV
void appendAttendance(const SessionState &ss, const String& timestamp, const String& uid, const String& name, int groupCode, const String& status) {
  String fn = attendanceFilename(ss);
  File f = SD.open(fn.c_str(), FILE_WRITE);
  if (f) {
    String line = timestamp + ";" + uid + ";" + name + ";" + String(groupCode) + ";" + status + ";" + ss.course + ";" + ss.room + "\n";
    f.print(line);
    f.close();
  } else {
    Serial.println(F("ERR: cannot open attendance file"));
  }
}

// Finalise un créneau en ajoutant les ABSENTS (par lecture de students.csv)
void finalizeSession(SessionState &ss) {
  if (ss.finalized) return;
  ss.finalized = true;

  // Construire un set des UIDs vus (naïf, mais suffisant)
  // On va scanner students.csv, et pour toute ligne dont le group ∈ session.groups, si UID non dans seen → ABSENT
  File f = SD.open("/students.csv", FILE_READ);
  if (!f) { Serial.println(F("ERR: students.csv missing")); return; }

  String tsNow;
  {
    String d, h; nowStrings(d,h); tsNow = d + " " + h + ":00";
  }

  String line;
  while (f.available()) {
    line = f.readStringUntil('\n');
    if (line.length() < 6) continue;
    String cols[3];
    if (!splitCSVLine(line, ';', cols, 3)) continue;
    String uid = cols[0];
    String name = cols[1];
    int g = cols[2].toInt();
    if (!groupInSession(ss, g)) continue;

    bool seen = false;
    for (uint16_t i=0;i<ss.seenCount;i++) { if (ss.seenUIDs[i]==uid) { seen=true; break; } }
    if (!seen) {
      appendAttendance(ss, tsNow, uid, name, g, "ABSENT");
    }
  }
  f.close();
  Serial.print(F("Finalized session -> ")); Serial.println(attendanceFilename(ss));
}

// ----------------- Setup & Loop -----------------
void setup() {
  pinMode(PIN_LED_OK, OUTPUT);
  pinMode(PIN_LED_ERR, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_LED_OK, LOW);
  digitalWrite(PIN_LED_ERR, LOW);

  Serial.begin(115200);
  delay(300);

  rtcInitFromCompile();

  SPI.begin();

  // SD
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println(F("ERROR: SD init failed"));
    signalERR();
    while(true){ delay(1000); }
  }

  // RFID
  mfrc522.PCD_Init();
  Serial.println(F("NFC Attendance ready."));
  loadSlotsToday(); // charge les créneaux du jour (pour le prof du boîtier)

  // Si tu veux forcer la RTC via série: envoyer T=YYYY-MM-DD HH:MM:SS
  Serial.println(F("Set RTC via serial: T=2025-09-19 08:00:00 (optional)"));
}

void handleSerialTimeSet() {
  if (!Serial.available()) return;
  String s = Serial.readStringUntil('\n');
  s.trim();
  if (!s.startsWith("T=")) return;
  // T=YYYY-MM-DD HH:MM:SS
  String t = s.substring(2);
  int y = t.substring(0,4).toInt();
  int mo= t.substring(5,7).toInt();
  int d = t.substring(8,10).toInt();
  int h = t.substring(11,13).toInt();
  int mi= t.substring(14,16).toInt();
  int se= t.substring(17,19).toInt();
  RTCTime tt((uint8_t)d, (Month)mo, (uint16_t)y, (uint8_t)h, (uint8_t)mi, (uint8_t)se);
  rtc.setTime(tt);
  Serial.println(F("RTC updated."));
}

void loop() {
  handleSerialTimeSet();

  // Finalisation auto des sessions quand l’heure est passée
  int nowM = nowMinutes();
  for (uint8_t i=0;i<sessionCount;i++) {
    if (!sessions[i].finalized && nowM > sessions[i].endMin) {
      finalizeSession(sessions[i]);
    }
  }

  // Lecture carte ?
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(10);
    return;
  }

  // UID → hex string uppercase
  String uid = "";
  for (byte i=0; i<mfrc522.uid.size; i++) {
    byte b = mfrc522.uid.uidByte[i];
    char buf[3]; snprintf(buf,3,"%02X", b);
    uid += buf;
  }

  // anti rebadge immédiat (si même UID dans la seconde)
  unsigned long nowMs = millis();
  if (uid == lastUID && (nowMs - lastScanMs) < DEBOUNCE_MS) {
    mfrc522.PICC_HaltA();
    return;
  }
  lastUID = uid; lastScanMs = nowMs;

  // Session(s) actives ?
  uint8_t idxs[8];
  int nActives = findActiveSessions(idxs, 8);
  if (nActives <= 0) {
    Serial.println(F("No active session now for this prof."));
    signalERR();
    mfrc522.PICC_HaltA();
    return;
  }

  // UID -> (name, group)
  String studentName; int studentGroup;
  if (!lookupStudentByUID(uid, studentName, studentGroup)) {
    Serial.println(F("Unknown UID"));
    signalERR();
    mfrc522.PICC_HaltA();
    return;
  }

  // Cherche une session active dont les groupes contiennent celui de l’élève
  SessionState *match = nullptr;
  for (int k=0;k<nActives;k++) {
    SessionState &ss = sessions[idxs[k]];
    if (groupInSession(ss, studentGroup)) { match = &ss; break; }
  }
  if (match == nullptr) {
    Serial.println(F("Student not in any active group."));
    signalERR();
    mfrc522.PICC_HaltA();
    return;
  }

  // Double badge ?
  if (alreadySeen(*match, uid)) {
    Serial.println(F("Already checked-in for this session."));
    // Retour visuel but pas d’erreur sonore
    digitalWrite(PIN_LED_OK, HIGH); delay(80); digitalWrite(PIN_LED_OK, LOW);
    mfrc522.PICC_HaltA();
    return;
  }

  // Enregistre présence
  String date, hhmm; nowStrings(date, hhmm);
  String ts = date + " " + hhmm + ":00";
  markSeen(*match, uid);
  appendAttendance(*match, ts, uid, studentName, studentGroup, "PRESENT");
  Serial.print(F("OK -> ")); Serial.print(studentName); Serial.print(F(" @ ")); Serial.println(ts);
  signalOK();

  mfrc522.PICC_HaltA();
}