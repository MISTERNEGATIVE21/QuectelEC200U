# Changelog

## 1.3.0 - 2025-09-20
- **BREAKING CHANGE**: Refactored serial handling to use a single `Stream` constructor. Users are now required to call `.begin()` on their serial object (Hardware or Software) *before* calling `modem.begin()`. This provides greater flexibility, especially for ESP32 users needing to specify custom RX/TX pins.
- Unified source code under a single primary header `QuectelEC200U.h`. A compatibility header `QuectelEC200U_CN.h` is retained for older examples.
- Updated all examples to reflect the new, clearer serial initialization pattern.
- Renamed `USS DDemo` example folder to `USSDDemo` for consistency.
- Removed duplicate `SMSDEMO.ino` file.

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
