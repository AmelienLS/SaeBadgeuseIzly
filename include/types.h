#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

// Shared constants used throughout the firmware.
constexpr uint8_t STATUS_LED_PIN = LED_BUILTIN;
constexpr uint8_t BADGE_INPUT_PIN = 2;          // Placeholder for a badge reader input line
constexpr size_t MAX_BADGE_BUFFER = 32;         // Max number of badge events kept locally
constexpr size_t CARD_ID_LENGTH = 16;           // Length (chars) of badge IDs accepted
constexpr size_t SERIAL_LINE_BUFFER = 96;       // Serial command buffer size

struct BadgeEvent {
    char cardId[CARD_ID_LENGTH + 1];
    uint32_t timestamp;      // milliseconds since boot
    bool pendingSync;        // true if not yet acknowledged by the host/database
};

enum class CommandResult : uint8_t {
    Ok,
    Error,
    BufferFull,
    UnknownCommand,
    InvalidPayload,
    NetworkUnavailable,
    StorageUnavailable,
    TransferFailed,
    Busy,
    FeatureUnavailable
};

#endif  // TYPES_H