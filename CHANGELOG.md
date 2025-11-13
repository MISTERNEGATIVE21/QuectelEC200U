# Changelog

## 1.6.0 - 2025-11-12
- ESP32 standardization: Fixes for terminal aand sending messages
## 1.5.0 - 2025-09-30
- ESP32 standardization: all examples now use Serial2 with default pins EC200U_RX=18, EC200U_TX=17 and PW_KEY=10 via a new shared header `src/EC200U_ESP32_Config.h` (includes EC200U_powerOn helper).
- Examples updated: HTTP, HTTPS, MQTT, NTP Sync, GNSS, FTP, Audio Setup, SMS, TCP, TTS, Voice Call, Bluetooth_and_WiFi switched to Serial2 config and cleaned legacy includes.
- Docs: README updated to show the circuit image, corrected repository link, and added “More information” link to the ESP32‑S3 with EC200U reference project.
- Breaking changes: none; library API unchanged. Non‑ESP32 boards continue to use SoftwareSerial paths.

## 1.4.0 - 2025-09-25
- Examples: added Bluetooth_and_WiFi_Demo (combined Bluetooth + Wi‑Fi) and Advance/WiFi_Generic_Demo (probes module Wi‑Fi AT support; falls back to ESP32 native WiFi).
- Examples naming: normalized folders and .ino filenames to {Name_Demo} pattern (e.g., Voice_Call_Demo/Voice_Call_Demo.ino). This improves Arduino IDE discovery and consistency.
- Notes: module Wi‑Fi AT commands may not be available on all EC200U variants; demos print ERROR if unsupported, which is expected.
- Breaking changes: none; additions are backward compatible.

## 1.3.0 - 2025-09-20
- Cross-board serial support: added HardwareSerial constructor with optional RX/TX pins (ESP32 auto-configured in begin) and a generic Stream constructor for SoftwareSerial/USB CDC.
- Examples standardized to work on ESP32 (HardwareSerial) and classic boards (SoftwareSerial). All .ino files select the appropriate serial at compile time and initialize it before modem.begin().
- Removed all includes of QuectelEC200U_CN.h from examples; primary header is QuectelEC200U.h. Compatibility header remains in src for legacy sketches.
- FTP, SMS, GNSS, HTTP/HTTPS, MQTT, NTP, PSM, TCP, TTS, and all advanced demos updated to the new initialization pattern.
- Packaging: library.properties includes set to QuectelEC200U.h; repository URL normalized.
- Docs: README updated with serial setup guidance for both HardwareSerial and SoftwareSerial.

## 1.2.3 - 2025-09-19
- Library naming synchronized to QuectelEC200U_CN; added primary wrapper header (QuectelEC200U_CN.h) and updated all examples to include it.
- Repository URL standardized to https://github.com/MISTERNEGATIVE21/QuectelEC200U_CN across sources and docs.
- Arduino Library Manager compliance: fixed header name rule (LS008), primary sketch names for folders (SS001), valid url field (LP042).
- GNSS improvements: stopGNSS, isGNSSOn, setGNSSConfig, getNMEASentence, getGNSSLocation(timeout); GNSS demo waits for fix and prints NMEA.
- Advanced examples under examples/advance: HTTPS TLS, MQTT over TLS, raw SSL sockets, PSM/eDRX, filesystem CA upload, NTP/time, USSD; added advance.ino hub.
- Serial portability: core now uses Stream for I/O (supports HardwareSerial and SoftwareSerial); added SoftwareSerial_HTTP_Demo.
- Core robustness: echo-off and input flushing in begin, improved readResponse idle exit; safer HTTP/HTTPS flows; MQTT connect includes QMTCONN; TCP helpers and filesystem functions.
- Documentation and metadata: README updated (usage/include, serial options, URL), LICENSE (MIT), AUTHORS, NOTICE, CHANGELOG, RELEASE_NOTES; library.properties updated; keywords.txt added.
- Breaking changes: none; additions are backward compatible.

## 1.1.1 - 2025-09-19
- Bump version for Arduino Library Manager.
- Added professional headers and LICENSE/NOTICE/AUTHORS.
- Updated author/maintainer and repo URL (now https://github.com/MISTERNEGATIVE21/QuectelEC200U_CN).
- GNSS improvements: stopGNSS, isGNSSOn, setGNSSConfig, getNMEASentence, getGNSSLocation(timeout).
- Fixed GNSS demo to wait for fix and print NMEA.
- Added advanced examples under `examples/advance` (TLS/HTTPS, MQTT over TLS, raw SSL, PSM/eDRX, FS CA upload, NTP/time, USSD).

## 1.0.0 - 2025-09-18
- Initial public version with EC200U core features: network/PDP, TCP, HTTP/HTTPS, MQTT, SMS, FS, USSD, NTP, TLS, PSM.
