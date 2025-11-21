#include <Arduino.h>
#include <cctype>
#include <cstdio>
#include <cstring>

#include "config.h"
#include "types.h"

#if defined(ARDUINO_UNOR4_WIFI)
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <SD.h>
#include <WiFiS3.h>
#endif

namespace {

BadgeEvent badgeBuffer[MAX_BADGE_BUFFER];
size_t badgeCount = 0;
char serialLine[SERIAL_LINE_BUFFER];
size_t serialLen = 0;
unsigned long lastFlushMs = 0;
bool statusLed = false;
bool lastBadgeInput = true;
appcfg::DeviceConfig deviceConfig;
bool configLoaded = false;

constexpr unsigned long SERIAL_READY_TIMEOUT_MS = 2000;
constexpr unsigned long AUTO_FLUSH_INTERVAL_MS = 2000;

void printBanner();
void pollSerial();
void processCommand(const char* line);
void handleAdd(const char* payload);
void handleAck(const char* payload);
void listEvents(bool pendingOnly);
void flushPending(bool force = false);
void emitEvent(size_t index, const BadgeEvent& evt);
void simulateBadgeFromInput();
void setStatusLed(bool enabled);
void printStatus();
const __FlashStringHelper* resultToMessage(CommandResult result);
CommandResult addBadgeEvent(const char* cardId);
CommandResult markAcknowledged(const char* cardId);
bool equalsIgnoreCase(const char* lhs, const char* rhs);
void applyDefaultConfig(appcfg::DeviceConfig& cfg);
bool loadDeviceConfig(bool verbose);

template <size_t N>
void copyStringSafe(const char* src, char (&dest)[N]);

#if defined(ARDUINO_UNOR4_WIFI)
void initSyncSubsystem();
void handlePeriodicSync();
CommandResult syncDatabaseNow(bool userInitiated);
bool ensureWifi();
bool ensureSdCard();
CommandResult downloadDatabaseFile();
#else
inline void initSyncSubsystem() {}
inline void handlePeriodicSync() {}
inline CommandResult syncDatabaseNow(bool) {
  return CommandResult::FeatureUnavailable;
}
#endif

}  // namespace

void setup() {
  pinMode(STATUS_LED_PIN, OUTPUT);
  setStatusLed(false);
  pinMode(BADGE_INPUT_PIN, INPUT_PULLUP);

  Serial.begin(115200);
  const unsigned long start = millis();
  while (!Serial && (millis() - start) < SERIAL_READY_TIMEOUT_MS) {
    delay(10);
  }

  loadDeviceConfig(true);
  initSyncSubsystem();
  printBanner();
}

void loop() {
  pollSerial();
  simulateBadgeFromInput();
  flushPending();
  handlePeriodicSync();
}

namespace {

void printBanner() {
  Serial.println(F("\n=== SAE Badgeuse (UNO R4) ==="));
  Serial.println(F("Commands:"));
  Serial.println(F("  HELP                -> show this message"));
  Serial.println(F("  PING                -> connectivity check"));
  Serial.println(F("  STATUS              -> buffer + sync state"));
  Serial.println(F("  LIST                -> dump every known badge"));
  Serial.println(F("  LIST_PENDING        -> dump unsent entries"));
  Serial.println(F("  ADD <ID>            -> create badge event"));
  Serial.println(F("  ACK <ID>            -> mark badge as synced"));
  Serial.println(F("  CLEAR               -> wipe buffer"));
  Serial.println(F("  SET_LED <ON|OFF>    -> control status LED"));
  Serial.println(F("  SYNC                -> force DB download (UNO R4 WiFi)"));
  Serial.println();
}

void pollSerial() {
  while (Serial.available() > 0) {
    const char incoming = static_cast<char>(Serial.read());
    if (incoming == '\r') {
      continue;
    }
    if (incoming == '\n') {
      if (serialLen > 0) {
        serialLine[serialLen] = '\0';
        processCommand(serialLine);
        serialLen = 0;
      }
      continue;
    }
    if (serialLen < (SERIAL_LINE_BUFFER - 1)) {
      serialLine[serialLen++] = incoming;
    }
  }
}

void processCommand(const char* line) {
  if (strcmp(line, "PING") == 0) {
    Serial.println(F("PONG"));
    return;
  }
  if (strcmp(line, "HELP") == 0) {
    printBanner();
    return;
  }
  if (strcmp(line, "STATUS") == 0) {
    printStatus();
    return;
  }
  if (strcmp(line, "LIST") == 0) {
    listEvents(false);
    return;
  }
  if (strcmp(line, "LIST_PENDING") == 0) {
    listEvents(true);
    return;
  }
  if (strcmp(line, "CLEAR") == 0) {
    badgeCount = 0;
    Serial.println(F("Buffer cleared."));
    return;
  }
  if (strcmp(line, "SYNC") == 0 || strcmp(line, "SYNC_NOW") == 0) {
    CommandResult result = syncDatabaseNow(true);
    Serial.println(resultToMessage(result));
    return;
  }
  if (strncmp(line, "ADD ", 4) == 0) {
    handleAdd(line + 4);
    return;
  }
  if (strncmp(line, "ACK ", 4) == 0) {
    handleAck(line + 4);
    return;
  }
  if (strncmp(line, "SET_LED ", 8) == 0) {
    const char* payload = line + 8;
    if (equalsIgnoreCase(payload, "ON")) {
      setStatusLed(true);
      Serial.println(F("LED=ON"));
      return;
    }
    if (equalsIgnoreCase(payload, "OFF")) {
      setStatusLed(false);
      Serial.println(F("LED=OFF"));
      return;
    }
    Serial.println(F("Invalid SET_LED payload."));
    return;
  }

  Serial.print(F("Unknown command: "));
  Serial.println(line);
}

void handleAdd(const char* payload) {
  while (*payload == ' ') {
    ++payload;
  }
  CommandResult result = addBadgeEvent(payload);
  Serial.println(resultToMessage(result));
}

void handleAck(const char* payload) {
  while (*payload == ' ') {
    ++payload;
  }
  CommandResult result = markAcknowledged(payload);
  Serial.println(resultToMessage(result));
}

CommandResult addBadgeEvent(const char* cardId) {
  if (cardId == nullptr || *cardId == '\0') {
    return CommandResult::InvalidPayload;
  }
  const size_t length = strlen(cardId);
  if (length > CARD_ID_LENGTH) {
    return CommandResult::InvalidPayload;
  }
  if (badgeCount >= MAX_BADGE_BUFFER) {
    return CommandResult::BufferFull;
  }

  BadgeEvent& evt = badgeBuffer[badgeCount++];
  strncpy(evt.cardId, cardId, CARD_ID_LENGTH);
  evt.cardId[CARD_ID_LENGTH] = '\0';
  evt.timestamp = millis();
  evt.pendingSync = true;
  setStatusLed(true);
  return CommandResult::Ok;
}

CommandResult markAcknowledged(const char* cardId) {
  if (cardId == nullptr || *cardId == '\0') {
    return CommandResult::InvalidPayload;
  }
  for (size_t i = 0; i < badgeCount; ++i) {
    if (strcmp(badgeBuffer[i].cardId, cardId) == 0) {
      badgeBuffer[i].pendingSync = false;
      return CommandResult::Ok;
    }
  }
  return CommandResult::Error;
}

void listEvents(bool pendingOnly) {
  Serial.print(F("EVENTS="));
  Serial.print(badgeCount);
  Serial.println();
  for (size_t i = 0; i < badgeCount; ++i) {
    if (pendingOnly && !badgeBuffer[i].pendingSync) {
      continue;
    }
    emitEvent(i, badgeBuffer[i]);
  }
}

void flushPending(bool force) {
  const unsigned long now = millis();
  if (!force && (now - lastFlushMs) < AUTO_FLUSH_INTERVAL_MS) {
    return;
  }
  lastFlushMs = now;
  for (size_t i = 0; i < badgeCount; ++i) {
    if (badgeBuffer[i].pendingSync) {
      emitEvent(i, badgeBuffer[i]);
    }
  }
}

void emitEvent(size_t index, const BadgeEvent& evt) {
  Serial.print(F("{\"idx\":"));
  Serial.print(index);
  Serial.print(F(",\"cardId\":\""));
  Serial.print(evt.cardId);
  Serial.print(F("\",\"timestampMs\":"));
  Serial.print(evt.timestamp);
  Serial.print(F(",\"pending\":"));
  Serial.print(evt.pendingSync ? F("true") : F("false"));
  Serial.println(F("}"));
}

void simulateBadgeFromInput() {
  const bool currentState = digitalRead(BADGE_INPUT_PIN) == HIGH;
  if (lastBadgeInput && !currentState) {
    static uint32_t syntheticCounter = 0;
    char cardId[CARD_ID_LENGTH + 1];
    snprintf(cardId, sizeof(cardId), "BTN%lu", static_cast<unsigned long>(syntheticCounter++));
    CommandResult result = addBadgeEvent(cardId);
    if (result == CommandResult::Ok) {
      Serial.print(F("Simulated badge: "));
      Serial.println(cardId);
    }
  }
  lastBadgeInput = currentState;
}

void setStatusLed(bool enabled) {
  statusLed = enabled;
  digitalWrite(STATUS_LED_PIN, enabled ? HIGH : LOW);
}

void printStatus() {
  Serial.print(F("Buffer: "));
  Serial.print(badgeCount);
  Serial.print(F(" / "));
  Serial.println(MAX_BADGE_BUFFER);
  Serial.print(F("Device UID: "));
  Serial.println(deviceConfig.deviceUid);
  Serial.print(F("Owner card: "));
  Serial.println(deviceConfig.ownerCardId);
  Serial.print(F("Config source: "));
  Serial.println(configLoaded ? F("device_config.json") : F("defaults"));
#if defined(ARDUINO_UNOR4_WIFI)
  Serial.print(F("WiFi: "));
  Serial.println((ensureWifi()) ? F("connected") : F("not connected"));
  Serial.print(F("SD: "));
  Serial.println(ensureSdCard() ? F("mounted") : F("unavailable"));
  Serial.print(F("Last sync (ms): "));
  Serial.println(lastSyncMs);
  Serial.print(F("Next sync (ms): "));
  Serial.println(nextPlannedSyncMs);
#endif
}

const __FlashStringHelper* resultToMessage(CommandResult result) {
  switch (result) {
    case CommandResult::Ok:
      return F("OK");
    case CommandResult::Error:
      return F("ERR");
    case CommandResult::BufferFull:
      return F("ERR_BUFFER_FULL");
    case CommandResult::UnknownCommand:
      return F("ERR_UNKNOWN_CMD");
    case CommandResult::InvalidPayload:
      return F("ERR_INVALID_PAYLOAD");
    case CommandResult::NetworkUnavailable:
      return F("ERR_NETWORK");
    case CommandResult::StorageUnavailable:
      return F("ERR_STORAGE");
    case CommandResult::TransferFailed:
      return F("ERR_TRANSFER");
    case CommandResult::Busy:
      return F("ERR_BUSY");
    case CommandResult::FeatureUnavailable:
      return F("ERR_UNSUPPORTED");
  }
  return F("ERR");
}

bool equalsIgnoreCase(const char* lhs, const char* rhs) {
  while (*lhs && *rhs) {
    if (tolower(static_cast<unsigned char>(*lhs)) != tolower(static_cast<unsigned char>(*rhs))) {
      return false;
    }
    ++lhs;
    ++rhs;
  }
  return (*lhs == '\0') && (*rhs == '\0');
}

template <size_t N>
void copyStringSafe(const char* src, char (&dest)[N]) {
  if (N == 0) {
    return;
  }
  if (src == nullptr) {
    dest[0] = '\0';
    return;
  }
  strncpy(dest, src, N - 1);
  dest[N - 1] = '\0';
}

void applyDefaultConfig(appcfg::DeviceConfig& cfg) {
  copyStringSafe(appcfg::DEFAULT_WIFI_SSID, cfg.wifiSsid);
  copyStringSafe(appcfg::DEFAULT_WIFI_PASSWORD, cfg.wifiPassword);
  copyStringSafe(appcfg::DEFAULT_SERVER_HOST, cfg.serverHost);
  cfg.serverPort = appcfg::DEFAULT_SERVER_PORT;
  copyStringSafe(appcfg::DEFAULT_SERVER_PATH, cfg.serverPath);
  copyStringSafe(appcfg::DEFAULT_DEVICE_UID, cfg.deviceUid);
  copyStringSafe(appcfg::DEFAULT_OWNER_CARD_ID, cfg.ownerCardId);
}

bool loadDeviceConfig(bool verbose) {
  applyDefaultConfig(deviceConfig);
#if defined(ARDUINO_UNOR4_WIFI)
  configLoaded = false;
  if (!ensureSdCard()) {
    if (verbose) {
      Serial.println(F("[CFG] SD unavailable, using defaults"));
    }
    return false;
  }
  File cfgFile = SD.open(appcfg::CONFIG_FILE_PATH, FILE_READ);
  if (!cfgFile) {
    if (verbose) {
      Serial.println(F("[CFG] No device_config.json found, using defaults"));
    }
    return false;
  }

  StaticJsonDocument<768> doc;
  DeserializationError err = deserializeJson(doc, cfgFile);
  cfgFile.close();
  if (err) {
    if (verbose) {
      Serial.print(F("[CFG] JSON error: "));
      Serial.println(err.c_str());
    }
    return false;
  }

  JsonObjectConst wifi = doc["wifi"].as<JsonObjectConst>();
  if (!wifi.isNull()) {
    if (wifi["ssid"].is<const char*>()) {
      copyStringSafe(wifi["ssid"].as<const char*>(), deviceConfig.wifiSsid);
    }
    if (wifi["password"].is<const char*>()) {
      copyStringSafe(wifi["password"].as<const char*>(), deviceConfig.wifiPassword);
    }
  }

  JsonObjectConst backend = doc["backend"].as<JsonObjectConst>();
  if (!backend.isNull()) {
    if (backend["host"].is<const char*>()) {
      copyStringSafe(backend["host"].as<const char*>(), deviceConfig.serverHost);
    }
    if (backend["path"].is<const char*>()) {
      copyStringSafe(backend["path"].as<const char*>(), deviceConfig.serverPath);
    }
    if (backend["port"].is<uint16_t>()) {
      deviceConfig.serverPort = backend["port"].as<uint16_t>();
    } else if (backend["port"].is<long>()) {
      deviceConfig.serverPort = static_cast<uint16_t>(backend["port"].as<long>());
    }
  }

  JsonObjectConst device = doc["device"].as<JsonObjectConst>();
  if (!device.isNull()) {
    if (device["uid"].is<const char*>()) {
      copyStringSafe(device["uid"].as<const char*>(), deviceConfig.deviceUid);
    }
    if (device["ownerCardId"].is<const char*>()) {
      copyStringSafe(device["ownerCardId"].as<const char*>(), deviceConfig.ownerCardId);
    }
  }

  configLoaded = true;
  if (verbose) {
    Serial.print(F("[CFG] Loaded config for UID="));
    Serial.print(deviceConfig.deviceUid);
    Serial.print(F(" (owner="));
    Serial.print(deviceConfig.ownerCardId);
    Serial.println(')');
  }
  return true;
#else
  (void)verbose;
  configLoaded = false;
  return false;
#endif
}

#if defined(ARDUINO_UNOR4_WIFI)

#if defined(SDCARD_SS_PIN)
constexpr uint8_t SD_CS_PIN = SDCARD_SS_PIN;
#elif defined(PIN_SPI_SS1)
constexpr uint8_t SD_CS_PIN = PIN_SPI_SS1;
#else
constexpr uint8_t SD_CS_PIN = SS;
#endif

bool wifiReady = false;
bool sdReady = false;
bool syncInProgress = false;
unsigned long nextPlannedSyncMs = 0;
unsigned long lastSyncMs = 0;

void initSyncSubsystem() {
  Serial.println(F("[SYNC] Initializing WiFi + SD subsystem"));
  ensureSdCard();
  ensureWifi();
  nextPlannedSyncMs = millis() + appcfg::STARTUP_SYNC_DELAY_MS;
}

void handlePeriodicSync() {
  if (syncInProgress) {
    return;
  }
  if (nextPlannedSyncMs == 0) {
    nextPlannedSyncMs = millis() + appcfg::STARTUP_SYNC_DELAY_MS;
  }
  const unsigned long now = millis();
  if ((long)(now - nextPlannedSyncMs) < 0) {
    return;
  }

  CommandResult result = syncDatabaseNow(false);
  const unsigned long interval = (result == CommandResult::Ok) ? appcfg::DAILY_SYNC_INTERVAL_MS
                                                               : appcfg::SYNC_RETRY_INTERVAL_MS;
  nextPlannedSyncMs = millis() + interval;
}

CommandResult syncDatabaseNow(bool userInitiated) {
  if (syncInProgress) {
    return CommandResult::Busy;
  }

  syncInProgress = true;
  CommandResult result = CommandResult::Error;

  if (!ensureSdCard()) {
    result = CommandResult::StorageUnavailable;
  } else if (!ensureWifi()) {
    result = CommandResult::NetworkUnavailable;
  } else {
    result = downloadDatabaseFile();
  }

  if (result == CommandResult::Ok) {
    lastSyncMs = millis();
    nextPlannedSyncMs = lastSyncMs + appcfg::DAILY_SYNC_INTERVAL_MS;
    Serial.print(F("[SYNC] Completed at ms="));
    Serial.println(lastSyncMs);
  } else if (userInitiated) {
    Serial.print(F("[SYNC] Failed: "));
    Serial.println(resultToMessage(result));
  }

  syncInProgress = false;
  return result;
}

bool ensureWifi() {
  if (wifiReady && WiFi.status() == WL_CONNECTED) {
    return true;
  }

  if (deviceConfig.wifiSsid[0] == '\0') {
    Serial.println(F("[WiFi] Missing SSID in config"));
    wifiReady = false;
    return false;
  }

  wifiReady = false;
  WiFi.disconnect();
  WiFi.begin(deviceConfig.wifiSsid, deviceConfig.wifiPassword);
  Serial.print(F("[WiFi] Connecting to "));
  Serial.print(deviceConfig.wifiSsid);
  const unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - start) < appcfg::WIFI_CONNECT_TIMEOUT_MS) {
    Serial.print('.');
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiReady = true;
    Serial.print(F(" connected. IP="));
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(F(" failed."));
  }
  return wifiReady;
}

bool ensureSdCard() {
  if (sdReady) {
    return true;
  }
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("[SD] Mount failed"));
    sdReady = false;
    return false;
  }
  sdReady = true;
  Serial.println(F("[SD] Ready"));
  return true;
}

CommandResult downloadDatabaseFile() {
  if (deviceConfig.serverHost[0] == '\0' || deviceConfig.serverPath[0] == '\0') {
    Serial.println(F("[SYNC] Missing server host/path in config"));
    return CommandResult::InvalidPayload;
  }

  Serial.print(F("[SYNC] Downloading from "));
  Serial.print(deviceConfig.serverHost);
  Serial.print(':');
  Serial.print(deviceConfig.serverPort);
  Serial.println(deviceConfig.serverPath);
#if APPCFG_USE_TLS
  WiFiSSLClient netClient;
#else
  WiFiClient netClient;
#endif
  HttpClient http(netClient, deviceConfig.serverHost, deviceConfig.serverPort);
  const int err = http.get(deviceConfig.serverPath);
  if (err != 0) {
    http.stop();
    Serial.print(F("[SYNC] HTTP error: "));
    Serial.println(err);
    return CommandResult::NetworkUnavailable;
  }
  const int statusCode = http.responseStatusCode();
  if (statusCode != 200) {
    http.stop();
    Serial.print(F("[SYNC] Unexpected status: "));
    Serial.println(statusCode);
    return CommandResult::TransferFailed;
  }
  const int skipResult = http.skipResponseHeaders();
  if (skipResult < 0) {
    http.stop();
    Serial.println(F("[SYNC] Failed to skip headers"));
    return CommandResult::TransferFailed;
  }

  if (SD.exists(appcfg::SD_TEMP_PATH)) {
    SD.remove(appcfg::SD_TEMP_PATH);
  }
  File temp = SD.open(appcfg::SD_TEMP_PATH, FILE_WRITE);
  if (!temp) {
    http.stop();
    Serial.println(F("[SYNC] Cannot open temp file"));
    return CommandResult::StorageUnavailable;
  }

  uint8_t buffer[512];
  while (true) {
    int available = http.available();
    if (available <= 0) {
      if (!http.connected()) {
        break;
      }
      delay(1);
      continue;
    }
    const int toRead = (available > static_cast<int>(sizeof(buffer))) ? sizeof(buffer) : available;
    const int bytesRead = http.read(reinterpret_cast<char*>(buffer), toRead);
    if (bytesRead <= 0) {
      temp.close();
      SD.remove(appcfg::SD_TEMP_PATH);
      http.stop();
      return CommandResult::TransferFailed;
    }
    const size_t written = temp.write(buffer, bytesRead);
    if (written != static_cast<size_t>(bytesRead)) {
      temp.close();
      SD.remove(appcfg::SD_TEMP_PATH);
      http.stop();
      return CommandResult::StorageUnavailable;
    }
  }

  temp.close();
  http.stop();

  if (SD.exists(appcfg::SD_TARGET_PATH)) {
    SD.remove(appcfg::SD_TARGET_PATH);
  }
  if (!SD.rename(appcfg::SD_TEMP_PATH, appcfg::SD_TARGET_PATH)) {
    SD.remove(appcfg::SD_TEMP_PATH);
    Serial.println(F("[SYNC] Rename failed"));
    return CommandResult::StorageUnavailable;
  }

  Serial.print(F("[SYNC] Stored at "));
  Serial.println(appcfg::SD_TARGET_PATH);
  return CommandResult::Ok;
}

#endif  // ARDUINO_UNOR4_WIFI

}  // namespace
