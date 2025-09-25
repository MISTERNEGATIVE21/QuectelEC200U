# v1.4.0 Release Notes (2025-09-25)

Highlights
- New examples:
	- Bluetooth_and_WiFi_Demo: Combined Bluetooth setup (AT+QBT*) plus Wi‑Fi (module probe) and ESP32 WiFi fallback.
	- Advance/WiFi_Generic_Demo: Probes module Wi‑Fi AT support and falls back to ESP32 native WiFi when available.
- Example naming normalized to {Name_Demo} for folders and sketches (e.g., Voice_Call_Demo/Voice_Call_Demo.ino), improving discoverability in Arduino IDE.
- No breaking API changes; existing sketches continue to work.

Notes
- Wi‑Fi AT commands may not exist on all EC200U variants/firmwares. The demos will print ERROR when unsupported; this is expected.
- Define WIFI_SSID and WIFI_PASS at the top of the Wi‑Fi examples before uploading.
- For cellular data features, set your APN in the relevant examples.

Links
- Repository: https://github.com/MISTERNEGATIVE21/QuectelEC200U_CN
- Changelog: ./CHANGELOG.md
