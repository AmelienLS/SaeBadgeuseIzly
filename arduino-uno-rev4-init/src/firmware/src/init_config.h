#ifndef INIT_CONFIG_H
#define INIT_CONFIG_H

// Configuration constants
const int LED_PIN = 13; // Pin number for the onboard LED
const int BUTTON_PIN = 2; // Pin number for the button

// Function declarations
void initHardware();
void initLED();
void initButton();

#endif // INIT_CONFIG_H