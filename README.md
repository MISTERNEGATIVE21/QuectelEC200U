# QuectelEC200U_CN

Arduino library for Quectel EC200U (CN-AA firmware) providing a robust AT-command wrapper and advanced examples.

## Features
- SIM/registration, PDP attach/activation
- TCP sockets (QIOPEN/QISEND/QIRD), SSL/TLS (QSSLCFG/QSSLOPEN)
- HTTP/HTTPS, MQTT (with TLS), USSD, SMS
- NTP/time, filesystem (upload/list/read/delete), TLS CA management
- GNSS (start/stop, location, NMEA)
- Power saving: PSM (CPSMS) and eDRX (CEDRXS)

## Installation
- Using Arduino IDE: place this folder in your `libraries` directory or install via Library Manager after publishing.
- Requirements: Any MCU with a UART to EC200U. Tested at 115200 baud. Architectures: `*`.

## Usage
```cpp
#include <QuectelEC200U.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  modem.begin();
  modem.attachData("your.apn");
  modem.activatePDP(1);
  String resp;
  modem.httpsGet("https://example.com", resp);
  Serial.println(resp);
}
void loop() {}
```

See `examples/advance` and `examples/GNSSDemo`.

## API Surface
- Core: begin, sendCommand, readResponse
- Network: waitForNetwork, attachData, activatePDP, deactivatePDP, isSimReady
- HTTP(S): httpGet/httpsGet/httpPost/httpsPost
- MQTT: mqttConnect/mqttPublish/mqttSubscribe
- Sockets: tcpOpen/tcpSend/tcpRecv/tcpClose
- GNSS: startGNSS/stopGNSS/isGNSSOn/getGNSSLocation(getGNSSLocation(ms))/getNMEASentence
- Filesystem: fsUpload/fsList/fsRead/fsDelete
- TLS: sslConfigure
- Time: ntpSync/getClock/setClock
- Power: enablePSM

## Contributing
PRs welcome. Please keep examples minimal and add documentation.

## License
MIT. See LICENSE.

## Trademarks & Attribution
Quectel, EC200U, and related marks are trademarks or registered trademarks of Quectel Wireless Solutions Co., Ltd. This library is unofficial and not affiliated with Quectel.
