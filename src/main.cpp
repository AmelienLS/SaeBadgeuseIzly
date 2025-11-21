#include <Arduino.h>
#include <cctype>
#include <cstdio>
#include <cstring>

#include "types.h"

namespace {

BadgeEvent badgeBuffer[MAX_BADGE_BUFFER];
size_t badgeCount = 0;
char serialLine[SERIAL_LINE_BUFFER];
size_t serialLen = 0;
unsigned long lastFlushMs = 0;
bool statusLed = false;
bool lastBadgeInput = true;

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
const __FlashStringHelper* resultToMessage(CommandResult result);
CommandResult addBadgeEvent(const char* cardId);
CommandResult markAcknowledged(const char* cardId);
bool equalsIgnoreCase(const char* lhs, const char* rhs);

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

  printBanner();
}

void loop() {
  pollSerial();
  simulateBadgeFromInput();
  flushPending();
}

namespace {

void printBanner() {
  Serial.println(F("\n=== SAE Badgeuse (UNO R4) ==="));
  Serial.println(F("Commands:"));
  Serial.println(F("  HELP                -> show this message"));
  Serial.println(F("  PING                -> connectivity check"));
  Serial.println(F("  STATUS              -> buffer usage"));
  Serial.println(F("  LIST                -> dump every known badge"));
  Serial.println(F("  LIST_PENDING        -> dump unsent entries"));
  Serial.println(F("  ADD <ID>            -> create badge event"));
  Serial.println(F("  ACK <ID>            -> mark badge as synced"));
  Serial.println(F("  CLEAR               -> wipe buffer"));
  Serial.println(F("  SET_LED <ON|OFF>    -> control status LED"));
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
    Serial.print(F("Buffer: "));
    Serial.print(badgeCount);
    Serial.print(F(" / "));
    Serial.println(MAX_BADGE_BUFFER);
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

}  // namespace
