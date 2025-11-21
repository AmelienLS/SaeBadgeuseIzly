# SAE Badgeuse (Arduino UNO R4)

Firmware scaffold for an Arduino UNO R4 (Minima or WiFi) badge reader that buffers badge events locally and exposes a simple serial protocol for a future database/backend bridge.

## Project Structure

- `src/main.cpp` – Firmware entry point with the serial command router and badge buffer implementation.
- `include/types.h` – Shared constants and data structures describing badge events.
- `platformio.ini` – PlatformIO environments for the UNO R4 Minima and UNO R4 WiFi boards.
- `arduino-uno/` – Legacy sketches kept for reference.

## Getting Started

1. Install [PlatformIO](https://platformio.org/install) (VS Code extension or CLI).
2. Select the appropriate board by editing `platformio.ini` (`uno_r4_minima` is the default).
3. Connect the UNO R4 via USB and run `pio run -t upload` (or use the VS Code PlatformIO UI).
4. Open a serial monitor at `115200` baud to interact with the firmware.

### Serial Commands

| Command | Description |
| --- | --- |
| `HELP` | Show the available commands. |
| `PING` | Check connectivity (responds with `PONG`). |
| `STATUS` | Display buffer usage. |
| `LIST` / `LIST_PENDING` | Dump all badge events or only those pending synchronization. |
| `ADD <ID>` | Inject a badge event (e.g., from a host application). |
| `ACK <ID>` | Mark a badge as synchronized once persisted in the database. |
| `CLEAR` | Remove every stored badge. |
| `SET_LED ON|OFF` | Manually control the status LED. |

Pin `D2` is configured as an input with the internal pull-up resistor. Shorting it to ground simulates a badge read and creates a synthetic event, which is useful for testing without the database layer.

## Next Steps

- Replace the simulated badge input with the actual reader hardware.
- Bridge the serial protocol to a host application that forwards `BadgeEvent` JSON blobs to the target database.
- Extend the command set with authentication and configuration features as needed.