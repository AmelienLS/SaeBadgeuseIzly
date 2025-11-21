#include <Arduino.h>

// Environment-specific duplicate of the basic test sketch.

const int LED_PIN = LED_BUILTIN;
unsigned long lastMillis = 0;
int blinkCount = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 2000) {}
  Serial.println("arduino-uno: Basic test starting...");
}

void loop() {
  unsigned long now = millis();
  if (now - lastMillis >= 500) {
    lastMillis = now;
    digitalWrite(LED_PIN, (blinkCount % 2) ? HIGH : LOW);
    Serial.print("arduino-uno Blink: ");
    Serial.println(blinkCount);
    blinkCount++;
  }
}
