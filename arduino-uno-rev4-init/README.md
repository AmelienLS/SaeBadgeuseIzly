# Arduino Uno Rev4 Initialization Project

This project provides a comprehensive setup for initializing and communicating with the Arduino Uno Rev4 board using both C++ and Python. It includes firmware for the Arduino, a Python host application, and example scripts to demonstrate functionality.

## Project Structure

```
arduino-uno-rev4-init
├── src
│   ├── firmware
│   │   ├── src
│   │   │   ├── main.ino
│   │   │   └── init_config.h
│   │   └── platformio.ini
│   ├── host
│   │   ├── python
│   │   │   ├── main.py
│   │   │   └── serial_comm.py
│   │   └── requirements.txt
│   └── scripts
│       └── upload.sh
├── examples
│   ├── python_example.py
│   └── firmware_example.ino
├── docs
│   ├── getting-started.md
│   └── wiring.md
├── .vscode
│   └── launch.json
├── .gitignore
├── LICENSE
└── README.md
```

## Getting Started

1. **Clone the Repository**: 
   Clone this repository to your local machine using:
   ```
   git clone <repository-url>
   ```

2. **Install Python Dependencies**: 
   Navigate to the `src/host` directory and install the required Python packages:
   ```
   pip install -r requirements.txt
   ```

3. **Upload Firmware to Arduino**: 
   Use the `upload.sh` script to upload the firmware to your Arduino Uno Rev4:
   ```
   cd src/scripts
   ./upload.sh
   ```

4. **Run the Python Application**: 
   Execute the main Python script to start communication with the Arduino:
   ```
   cd src/host/python
   python main.py
   ```

## Usage

- The `src/firmware/src/main.ino` file contains the main logic for the Arduino.
- The `src/host/python/main.py` file serves as the entry point for the Python application.
- Examples of usage can be found in the `examples` directory.

## Contribution

Contributions are welcome! Please feel free to submit a pull request or open an issue for any enhancements or bug fixes.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.