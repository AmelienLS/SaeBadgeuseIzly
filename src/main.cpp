#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>

// === Configuration WiFi ===
const char WIFI_SSID[] = "VOTRE_SSID";           // À modifier
const char WIFI_PASSWORD[] = "VOTRE_MOT_DE_PASSE"; // À modifier

// === Configuration serveur ===
const char SERVER_HOST[] = "votre-serveur.com";   // À modifier
const int SERVER_PORT = 80;
const char SERVER_PATH[] = "/export_etudiants.php"; // Chemin vers le script PHP

// === Pin mapping ===
constexpr uint8_t SD_CS_PIN = 10;
constexpr uint8_t SD_PWR_PIN = 8;
constexpr uint8_t RFID_SS_PIN = 7;
constexpr uint8_t RFID_RST_PIN = 9;

// === Configuration ===
constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t READ_COOLDOWN_MS = 1500;
constexpr char LOG_FILE_NAME[] = "/badges.csv";
constexpr char DB_FILE_NAME[] = "/etudiants.csv";
constexpr uint32_t DB_UPDATE_INTERVAL_MS = 3600000; // 1 heure

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
WiFiClient wifiClient;
HttpClient httpClient = HttpClient(wifiClient, SERVER_HOST, SERVER_PORT);

bool sdReady = false;
bool wifiConnected = false;
uint32_t lastDbUpdate = 0;

struct LastBadge
{
    byte uid[10] = {0};
    byte byteCount = 0;
    uint32_t timestamp = 0;
} lastBadge;

struct Student
{
    String uid;
    String nom;
    String prenom;
    bool found;
};

// ================================
// FONCTIONS WIFI
// ================================

bool connectWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        return true;
    }

    Serial.print(F("[WiFi] Connexion à "));
    Serial.print(WIFI_SSID);
    Serial.print(F("..."));

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20)
    {
        delay(500);
        Serial.print(F("."));
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println(F(" Connecté!"));
        Serial.print(F("[WiFi] Adresse IP: "));
        Serial.println(WiFi.localIP());
        wifiConnected = true;
        return true;
    }
    else
    {
        Serial.println(F(" Échec"));
        wifiConnected = false;
        return false;
    }
}

bool downloadDatabase()
{
    if (!connectWiFi())
    {
        Serial.println(F("[WiFi] Impossible de se connecter au WiFi"));
        return false;
    }

    if (!sdReady)
    {
        Serial.println(F("[DB] Carte SD non prête"));
        return false;
    }

    Serial.print(F("[DB] Téléchargement de la base depuis "));
    Serial.println(SERVER_HOST);

    // Supprimer l'ancienne base si elle existe
    if (SD.exists(DB_FILE_NAME))
    {
        SD.remove(DB_FILE_NAME);
    }

    // Effectuer la requête HTTP
    httpClient.get(SERVER_PATH);

    int statusCode = httpClient.responseStatusCode();
    Serial.print(F("[DB] Code HTTP: "));
    Serial.println(statusCode);

    if (statusCode != 200)
    {
        httpClient.stop();
        Serial.println(F("[DB] Erreur lors du téléchargement"));
        return false;
    }

    // Ouvrir le fichier pour écriture
    File dbFile = SD.open(DB_FILE_NAME, FILE_WRITE);
    if (!dbFile)
    {
        httpClient.stop();
        Serial.println(F("[DB] Impossible de créer le fichier"));
        return false;
    }

    // Lire et écrire les données par blocs
    int bytesWritten = 0;
    while (httpClient.available())
    {
        char c = httpClient.read();
        dbFile.write(c);
        bytesWritten++;
    }

    dbFile.close();
    httpClient.stop();

    Serial.print(F("[DB] Base téléchargée: "));
    Serial.print(bytesWritten);
    Serial.println(F(" octets"));

    lastDbUpdate = millis();
    return true;
}

// ================================
// FONCTIONS BASE DE DONNÉES
// ================================

Student findStudentByUid(const String &uid)
{
    Student student;
    student.uid = uid;
    student.found = false;

    if (!SD.exists(DB_FILE_NAME))
    {
        Serial.println(F("[DB] Base de données non trouvée"));
        return student;
    }

    File dbFile = SD.open(DB_FILE_NAME, FILE_READ);
    if (!dbFile)
    {
        Serial.println(F("[DB] Impossible d'ouvrir la base"));
        return student;
    }

    // Lire ligne par ligne
    String line = "";
    bool isHeader = true;

    while (dbFile.available())
    {
        char c = dbFile.read();

        // Ignorer les \r, traiter seulement \n comme fin de ligne
        if (c == '\r')
        {
            continue;
        }

        if (c == '\n')
        {
            if (line.length() > 0)
            {
                // Ignorer l'en-tête
                if (isHeader)
                {
                    isHeader = false;
                    line = "";
                    continue;
                }

                // Parser la ligne CSV: uid_rfid,nom,prenom,email,codeGroupe
                int firstComma = line.indexOf(',');
                if (firstComma > 0)
                {
                    String fileUid = line.substring(0, firstComma);
                    fileUid.trim();

                    if (fileUid.equalsIgnoreCase(uid))
                    {
                        // Trouvé ! Extraire nom et prénom
                        int secondComma = line.indexOf(',', firstComma + 1);
                        int thirdComma = line.indexOf(',', secondComma + 1);

                        if (secondComma > 0 && thirdComma > 0)
                        {
                            student.nom = line.substring(firstComma + 1, secondComma);
                            student.prenom = line.substring(secondComma + 1, thirdComma);
                            student.nom.trim();
                            student.prenom.trim();
                            student.found = true;
                        }

                        dbFile.close();
                        return student;
                    }
                }

                line = "";
            }
        }
        else
        {
            line += c;
        }
    }

    // Traiter la dernière ligne si le fichier ne se termine pas par \n
    if (line.length() > 0 && !isHeader)
    {
        int firstComma = line.indexOf(',');
        if (firstComma > 0)
        {
            String fileUid = line.substring(0, firstComma);
            fileUid.trim();

            if (fileUid.equalsIgnoreCase(uid))
            {
                int secondComma = line.indexOf(',', firstComma + 1);
                int thirdComma = line.indexOf(',', secondComma + 1);

                if (secondComma > 0 && thirdComma > 0)
                {
                    student.nom = line.substring(firstComma + 1, secondComma);
                    student.prenom = line.substring(secondComma + 1, thirdComma);
                    student.nom.trim();
                    student.prenom.trim();
                    student.found = true;
                }
            }
        }
    }

    dbFile.close();
    return student;
}

// ================================
// FONCTIONS SD
// ================================

void ensureSdHeader()
{
    if (!SD.exists(LOG_FILE_NAME))
    {
        File file = SD.open(LOG_FILE_NAME, FILE_WRITE);
        if (file)
        {
            file.println(F("timestamp_ms,uid_hex,nom,prenom"));
            file.close();
        }
    }
}

bool logBadgeToSd(const Student &student, uint32_t timestamp)
{
    File file = SD.open(LOG_FILE_NAME, FILE_WRITE);
    if (!file)
    {
        Serial.println(F("[SD] Impossible d'ouvrir le fichier de log"));
        return false;
    }

    file.print(timestamp);
    file.print(',');
    file.print(student.uid);
    file.print(',');
    file.print(student.nom);
    file.print(',');
    file.println(student.prenom);
    file.close();
    return true;
}

void tryMountSd()
{
    if (sdReady)
    {
        return;
    }

    sdReady = SD.begin(SD_CS_PIN);
    if (sdReady)
    {
        ensureSdHeader();
        Serial.println(F("[SD] Carte détectée"));
    }
}

// ================================
// FONCTIONS RFID
// ================================

String formatUid(const MFRC522::Uid &uid)
{
    String formatted;
    for (byte i = 0; i < uid.size; ++i)
    {
        if (uid.uidByte[i] < 0x10)
        {
            formatted += '0';
        }
        formatted += String(uid.uidByte[i], HEX);
    }
    formatted.toUpperCase();
    return formatted;
}

bool isSameAsLast(const MFRC522::Uid &uid)
{
    if (uid.size != lastBadge.byteCount)
    {
        return false;
    }

    for (byte i = 0; i < uid.size; ++i)
    {
        if (uid.uidByte[i] != lastBadge.uid[i])
        {
            return false;
        }
    }

    return (millis() - lastBadge.timestamp) < READ_COOLDOWN_MS;
}

void cacheLastBadge(const MFRC522::Uid &uid)
{
    lastBadge.byteCount = uid.size;
    for (byte i = 0; i < uid.size; ++i)
    {
        lastBadge.uid[i] = uid.uidByte[i];
    }
    lastBadge.timestamp = millis();
}

// ================================
// SETUP & LOOP
// ================================

void setup()
{
    pinMode(SD_PWR_PIN, OUTPUT);
    digitalWrite(SD_PWR_PIN, LOW);
    pinMode(SD_CS_PIN, OUTPUT);
    pinMode(RFID_SS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);
    digitalWrite(RFID_SS_PIN, HIGH);

    Serial.begin(SERIAL_BAUD);
    while (!Serial)
    {
        delay(10);
    }

    Serial.println(F("========================================"));
    Serial.println(F("  Système de Badgeuse RFID avec WiFi  "));
    Serial.println(F("========================================"));

    SPI.begin();

    // Initialiser la carte SD
    tryMountSd();
    if (!sdReady)
    {
        Serial.println(F("[SD] ERREUR: Carte SD non détectée"));
    }
    else
    {
        Serial.println(F("[SD] Carte SD initialisée"));
    }

    // Initialiser le lecteur RFID
    rfid.PCD_Init();
    Serial.println(F("[RFID] Lecteur initialisé"));

    // Télécharger la base de données
    Serial.println(F("[DB] Téléchargement initial de la base..."));
    if (downloadDatabase())
    {
        Serial.println(F("[DB] Base de données prête"));
    }
    else
    {
        Serial.println(F("[DB] AVERTISSEMENT: Échec du téléchargement"));
        Serial.println(F("[DB] Les badges seront enregistrés avec UID uniquement"));
    }

    Serial.println(F("\n[SYSTÈME] Prêt - Présentez un badge\n"));
}

void loop()
{
    // Mettre à jour la base de données périodiquement
    if (millis() - lastDbUpdate > DB_UPDATE_INTERVAL_MS)
    {
        Serial.println(F("\n[DB] Mise à jour périodique de la base..."));
        downloadDatabase();
    }

    // Attendre un nouveau badge
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
    {
        delay(50);
        return;
    }

    if (isSameAsLast(rfid.uid))
    {
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        return;
    }

    const String uidString = formatUid(rfid.uid);
    const uint32_t timestamp = millis();

    Serial.println(F("\n----------------------------------------"));
    Serial.print(F("[RFID] Badge détecté: "));
    Serial.println(uidString);

    // Chercher l'étudiant dans la base
    Student student = findStudentByUid(uidString);

    if (student.found)
    {
        Serial.print(F("[DB] Étudiant trouvé: "));
        Serial.print(student.prenom);
        Serial.print(F(" "));
        Serial.println(student.nom);
    }
    else
    {
        Serial.println(F("[DB] Étudiant non trouvé dans la base"));
        // Si non trouvé, on garde quand même l'UID
        student.uid = uidString;
        student.nom = "INCONNU";
        student.prenom = "";
    }

    // Vérifier la carte SD
    if (!sdReady)
    {
        tryMountSd();
    }

    if (!sdReady)
    {
        Serial.println(F("[SD] ERREUR: Carte SD non disponible"));
    }
    else
    {
        const bool logged = logBadgeToSd(student, timestamp);

        if (logged)
        {
            Serial.println(F("[SD] Badgeage enregistré"));
            cacheLastBadge(rfid.uid);
        }
        else
        {
            Serial.println(F("[SD] ERREUR: Échec de l'enregistrement"));
        }
    }

    Serial.println(F("----------------------------------------\n"));

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(250);
}
