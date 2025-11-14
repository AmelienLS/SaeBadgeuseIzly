# Wiring Diagram and Instructions for Arduino Uno Rev4

## Wiring Diagram
![Wiring Diagram](path/to/wiring_diagram.png)

## Components Required
- Arduino Uno Rev4
- LED
- Resistor (220 ohm)
- Push Button
- Breadboard
- Jumper Wires

## Connections
1. **LED Connection**
   - Connect the longer leg (anode) of the LED to digital pin 13 on the Arduino.
   - Connect the shorter leg (cathode) of the LED to one end of the resistor.
   - Connect the other end of the resistor to the ground (GND) on the Arduino.

2. **Push Button Connection**
   - Connect one terminal of the push button to digital pin 2 on the Arduino.
   - Connect the other terminal of the push button to the ground (GND) on the Arduino.
   - Optionally, connect a pull-up resistor (10k ohm) between the digital pin and VCC (5V) to ensure stable readings.

## Additional Notes
- Ensure all connections are secure to prevent intermittent issues.
- Use a breadboard for easier connections and to avoid soldering components directly to the Arduino.
- Refer to the Arduino documentation for more details on pin configurations and electrical specifications.