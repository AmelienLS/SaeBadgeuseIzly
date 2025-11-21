#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

#include "types.h"

namespace appcfg {

// Path on the SD card that stores the device-level configuration (JSON).
constexpr char CONFIG_FILE_PATH[] = "/device_config.json";

// Build-time defaults used when the SD config is missing. Safe placeholders
// that must be overridden before deploying to production.
constexpr char DEFAULT_WIFI_SSID[] = "YOUR_WIFI_SSID";
constexpr char DEFAULT_WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
constexpr char DEFAULT_SERVER_HOST[] = "192.168.0.10";
constexpr uint16_t DEFAULT_SERVER_PORT = 80;  // Use 443 for HTTPS.
constexpr char DEFAULT_SERVER_PATH[] = "/Base_Seances.sql";
constexpr char DEFAULT_DEVICE_UID[] = "DEVICE-0000";
constexpr char DEFAULT_OWNER_CARD_ID[] = "CARD-OWNER-0001";

// Set to 1 if the endpoint requires TLS (HTTPS). When enabled, also configure
// the certificate store via WiFiSSLClient before flashing the firmware.
#define APPCFG_USE_TLS 0

// Destination paths on the SD card (for the downloaded SQL dump).
constexpr char SD_TARGET_PATH[] = "/Base_Seances.sql";
constexpr char SD_TEMP_PATH[] = "/Base_Seances.tmp";

// Scheduling configuration (milliseconds).
constexpr unsigned long DAILY_SYNC_INTERVAL_MS = 24UL * 60UL * 60UL * 1000UL;  // once per day
constexpr unsigned long SYNC_RETRY_INTERVAL_MS = 10UL * 60UL * 1000UL;          // retry every 10 min on failure
constexpr unsigned long STARTUP_SYNC_DELAY_MS = 15UL * 1000UL;                  // wait after boot before first sync
constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 25UL * 1000UL;                // 25 seconds

struct DeviceConfig {
	char wifiSsid[33];
	char wifiPassword[65];
	char serverHost[65];
	uint16_t serverPort;
	char serverPath[96];
	char deviceUid[40];
	char ownerCardId[CARD_ID_LENGTH + 1];
};

}  // namespace appcfg

#endif  // CONFIG_H
