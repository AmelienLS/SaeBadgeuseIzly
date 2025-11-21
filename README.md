# SAE Badgeuse (Arduino UNO R4)

Firmware scaffold for an Arduino UNO R4 (Minima or WiFi) badge reader that buffers badge events locally and exposes a simple serial protocol for a future database/backend bridge.

## Project Structure

- `src/main.cpp` – Firmware entry point with the serial command router and badge buffer implementation.
- `include/types.h` – Shared constants and data structures describing badge events.
- `include/config.h` – Scheduling constants, SD paths, and default values consumed by the automated database sync.
- `config/device_config.example.json` – Template JSON file that must be copied to the SD card (as `device_config.json`) and filled with WiFi/backend/device credentials.
- `platformio.ini` – PlatformIO environments for the UNO R4 Minima and UNO R4 WiFi boards.
- `arduino-uno/` – Legacy sketches kept for reference.

## Getting Started

1. Install [PlatformIO](https://platformio.org/install) (VS Code extension or CLI).
2. If you plan to use the automated WiFi/SD synchronization, switch to the `uno_r4_wifi` environment in `platformio.ini` and populate an SD card with a `device_config.json` file (see below) that contains your WiFi credentials plus the HTTP server that exposes `Base_Seances.sql`.
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
| `SYNC` | Force the WiFi+SD database download (UNO R4 WiFi). |

Pin `D2` is configured as an input with the internal pull-up resistor. Shorting it to ground simulates a badge read and creates a synthetic event, which is useful for testing without the database layer.

## Device Configuration File

Place a `device_config.json` file at the root of the UNO R4 WiFi's micro-SD card. You can start from `config/device_config.example.json` and adjust the fields:

| Section | Field | Description |
| --- | --- | --- |
| `wifi` | `ssid`, `password` | Credentials for the protected internal network. |
| `backend` | `host`, `port`, `path` | HTTP(S) endpoint that serves `Base_Seances.sql`. |
| `device` | `uid` | Unique identifier for the badge reader enclosure/asset. |
| `device` | `ownerCardId` | Badge/card ID of the authorized person responsible for the unit. |

If the file is missing or malformed, the firmware falls back to the placeholder values compiled into `include/config.h`, so update that file with safe defaults before distributing binaries.

## Daily Database Refresh (UNO R4 WiFi)

- The firmware connects to the protected WiFi network, fetches `Base_Seances.sql` from the configured server, and stores it on the on-board micro-SD card once every 24 hours.
- A temporary file is written first; the previous dump is deleted only after the new version is completely stored to avoid corruption.
- Use the `SYNC` command to trigger the procedure immediately (useful for debugging).
- Progress appears on the serial console (`[WiFi]`, `[SD]`, `[SYNC]` logs). The `STATUS` command reports WiFi/SD readiness plus the last/next sync timestamps.

## Next Steps

- Replace the simulated badge input with the actual reader hardware.
- Bridge the serial protocol to a host application that forwards `BadgeEvent` JSON blobs to the target database.
- Wire the SD card dump into whichever middleware will import the SQL data and keep the database synchronized.
- Extend the command set with authentication and configuration features as needed.