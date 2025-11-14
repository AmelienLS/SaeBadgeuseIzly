#!/bin/bash

# This script uploads the firmware to the Arduino Uno Rev4 board using PlatformIO.

# Navigate to the firmware directory
cd "$(dirname "$0")/../firmware"

# Upload the firmware
platformio run --target upload

# Notify the user of completion
echo "Firmware upload complete."