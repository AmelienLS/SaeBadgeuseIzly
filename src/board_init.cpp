#include <Arduino.h>
#include "board_init.h"

void initializeBoard() {
    // Set pin modes for the Arduino Uno Rev4
    pinMode(LED_BUILTIN, OUTPUT); // Initialize the built-in LED pin as an output
    // Add additional pin initialization as needed
}

void configurePeripherals() {
    // Initialize any necessary peripherals
    // For example, setting up serial communication
    Serial.begin(9600); // Start serial communication at 9600 baud
    // Add additional peripheral configuration as needed
}

void boardInit() {
    initializeBoard();
    configurePeripherals();
}