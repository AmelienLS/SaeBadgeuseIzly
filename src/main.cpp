#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SD.h>

// === Pin mapping (adjust to match your wiring) ===
constexpr uint8_t SD_CS_PIN = 10;     // CS (chip select) for the SD module
constexpr uint8_t SD_PWR_PIN = 8;     // OFF pin on the SD breakout (LOW keeps the module powered)
constexpr uint8_t RFID_SS_PIN = 7;    // NSS / SDA on the RC522 - must be different from SD_CS_PIN
constexpr uint8_t RFID_RST_PIN = 9;   // RST on the RC522

constexpr uint32_t SERIAL_BAUD = 115200;
constexpr uint32_t READ_COOLDOWN_MS = 1500;   // Avoid logging the same badge multiple times in a row
constexpr char LOG_FILE_NAME[] = "/badges.csv";

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
bool sdReady = false;

struct LastBadge
{
    byte uid[10] = {0};
    byte byteCount = 0;
    uint32_t timestamp = 0;
} lastBadge;

void ensureSdHeader()
{
    if (!SD.exists(LOG_FILE_NAME))
    {
        File file = SD.open(LOG_FILE_NAME, FILE_WRITE);
        if (file)
        {
            file.println(F("timestamp_ms,uid_hex"));
            file.close();
        }
    }
}

bool logBadgeToSd(const String &uid, uint32_t timestamp)
{
    File file = SD.open(LOG_FILE_NAME, FILE_WRITE);
    if (!file)
    {
        Serial.println(F("[SD] Impossible d'ouvrir le fichier de log"));
        return false;
    }

    file.print(timestamp);
    file.print(',');
    file.println(uid);
    file.close();
    return true;
}

bool createSnapshotFile(const String &uid, uint32_t timestamp)
{
    char fileName[32];
    snprintf(fileName, sizeof(fileName), "/badge_%lu.txt", static_cast<unsigned long>(timestamp));

    File file = SD.open(fileName, FILE_WRITE);
    if (!file)
    {
        Serial.println(F("[SD] Impossible de créer le fichier badge"));
        return false;
    }

    file.print(F("timestamp_ms="));
    file.println(timestamp);
    file.print(F("uid_hex="));
    file.println(uid);
    file.close();
    Serial.print(F("[SD] Fichier créé: "));
    Serial.println(fileName);
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

void setup()
{
    pinMode(SD_PWR_PIN, OUTPUT);
    digitalWrite(SD_PWR_PIN, LOW); // LOW keeps the SD module active on most breakouts
    pinMode(SD_CS_PIN, OUTPUT);
    pinMode(RFID_SS_PIN, OUTPUT);
    digitalWrite(SD_CS_PIN, HIGH);
    digitalWrite(RFID_SS_PIN, HIGH);

    Serial.begin(SERIAL_BAUD);
    while (!Serial)
    {
        delay(10);
    }

    SPI.begin();

    tryMountSd();
    if (!sdReady)
    {
        Serial.println(F("[SD] Initialisation échouée - vérifier la carte et le câblage"));
    }
    else
    {
        Serial.println(F("[SD] Carte initialisée"));
    }

    rfid.PCD_Init();
    Serial.println(F("[RFID] Lecteur initialisé - présenter un badge"));
}

void loop()
{
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

    Serial.print(F("[RFID] Badge détecté: "));
    Serial.println(uidString);

    if (!sdReady)
    {
        tryMountSd();
    }

    if (!sdReady)
    {
        Serial.println(F("[SD] Carte non détectée - impossible d'écrire"));
    }
    else
    {
        const bool csvLogged = logBadgeToSd(uidString, timestamp);
        const bool snapshotCreated = createSnapshotFile(uidString, timestamp);

        if (csvLogged)
        {
            Serial.println(F("[SD] UID enregistré"));
        }

        if (snapshotCreated)
        {
            Serial.println(F("[SD] Fichier individuel enregistré"));
        }

        if (csvLogged || snapshotCreated)
        {
            cacheLastBadge(rfid.uid);
        }
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(250);
}
