#ifndef TYPES_H
#define TYPES_H

// Define custom types and constants here

typedef enum {
    LED_PIN = 13,
    BUTTON_PIN = 2
} PinConfig;

typedef struct {
    int value;
    bool state;
} SensorData;

#endif // TYPES_H