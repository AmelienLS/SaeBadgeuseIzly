# Getting Started with Arduino Uno Rev4

Welcome to the Arduino Uno Rev4 project! This guide will help you set up your development environment and get started with programming your Arduino board.

## Prerequisites

Before you begin, ensure you have the following installed:

- **Arduino IDE**: Download and install the latest version from the [Arduino website](https://www.arduino.cc/en/software).
- **PlatformIO**: If you prefer using PlatformIO, install it as an extension in your code editor (e.g., Visual Studio Code).
- **Python**: Make sure you have Python installed. You can download it from the [Python website](https://www.python.org/downloads/).
- **Python Dependencies**: Install the required Python packages by running the following command in your terminal:
  ```
  pip install -r src/host/requirements.txt
  ```

## Project Structure

The project is organized as follows:

```
arduino-uno-rev4-init
├── src
│   ├── firmware
│   ├── host
│   └── scripts
├── examples
├── docs
├── .vscode
├── .gitignore
├── LICENSE
└── README.md
```

## Setting Up the Firmware

1. Open the `src/firmware/platformio.ini` file and ensure the board is set to `arduino:avr:uno`.
2. Open the `src/firmware/src/main.ino` file to view the main Arduino sketch.
3. Connect your Arduino Uno Rev4 to your computer via USB.
4. Upload the firmware to the board using PlatformIO or the Arduino IDE.

## Running the Python Host

1. Navigate to the `src/host/python` directory.
2. Open `main.py` to see the main entry point for the Python application.
3. Ensure your Arduino is connected and the correct serial port is specified in the Python code.
4. Run the Python script:
   ```
   python main.py
   ```

## Example Usage

Check the `examples` directory for example scripts that demonstrate how to use the firmware and communicate with the Arduino using Python.

## Additional Resources

- Refer to `docs/wiring.md` for wiring diagrams and component connections.
- For troubleshooting and advanced configurations, consult the README.md and other documentation files in the `docs` directory.

Happy coding!