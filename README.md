# QuectelEC200U_CN

Arduino library for Quectel EC200U (CN-AA firmware) providing a robust AT-command wrapper and advanced examples.

Repository: [https://github.com/misternegative21Quaanctelec200u](https://github.com/MISTERNEGATIVE21/QuectelEC200U_CN)

## Features
- SIM/registration, PDP attach/activation
- TCP sockets (QIOPEN/QISEND/QIRD), SSL/TLS (QSSLCFG/QSSLOPEN)
- HTTP/HTTPS, MQTT (with TLS), USSD, SMS, Voice calls
- NTP/time, filesystem (upload/list/read/delete), TLS CA management
- GNSS (start/stop, location, NMEA)
- Power saving: PSM (CPSMS) and eDRX (CEDRXS)
- Audio: Speaker volume, mic gain, sidetone, audio routing

## Installation
- Using Arduino IDE: place this folder in your `libraries` directory or install via Library Manager after publishing.
- Requirements: Any MCU with a UART to EC200U. Tested at 115200 baud. Architectures: `*`.

## Serial setup
- ESP32 (HardwareSerial with optional RX/TX pins):
  ```cpp
  #include <QuectelEC200U.h>
  HardwareSerial SerialAT(1);
  QuectelEC200U modem(SerialAT, 115200, /* RX */ 16, /* TX */ 17);
  // in setup(): modem.begin(); // auto-configures UART on ESP32
  ```
- Other boards (SoftwareSerial):
  ```cpp
  #include <SoftwareSerial.h>
  #include <QuectelEC200U.h>
  SoftwareSerial SerialAT(7, 8); // RX, TX
  QuectelEC200U modem(SerialAT);
  // in setup(): SerialAT.begin(9600); modem.begin();
  ```

## Usage
```cpp
#include <QuectelEC200U.h>

HardwareSerial& modemSerial = Serial1; // or SoftwareSerial as shown above
QuectelEC200U modem(modemSerial);

void setup() {
  Serial.begin(115200);
  modemSerial.begin(115200);
  modem.begin();
  modem.attachData("your.apn");
  modem.activatePDP(1);
  String resp;
  if (modem.httpsGet("https://example.com", resp)) Serial.println(resp);
}
void loop() {}
```

See `examples` and `examples/advance` for more.

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
- Voice: dial/hangup/answer/getCallList/enableCallerId
- Audio: setSpeakerVolume/setRingerVolume/setMicMute/setMicGain/setSidetone/setAudioChannel/setAudioInterface/audioLoopback

### Audio quick start
- Route audio to loudspeaker and set volume:
  ```cpp
  modem.setAudioChannel(2);
  modem.setSpeakerVolume(80);
  ```
- Unmute mic and set gain:
  ```cpp
  modem.setMicMute(false);
  modem.setMicGain(0, 8);
  ```
- Enable sidetone at medium level:
  ```cpp
  modem.setSidetone(true, 5);
  ```

## Contributing
PRs welcome. Please keep examples minimal and add documentation.

## Maintainer
MisterNegative21 <misternegative21@gmail.com>

## License
MIT. See LICENSE.

## Trademarks & Attribution
Quectel, EC200U, and related marks are trademarks or registered trademarks of Quectel Wireless Solutions Co., Ltd. This library is unofficial and not affiliated with Quectel.
