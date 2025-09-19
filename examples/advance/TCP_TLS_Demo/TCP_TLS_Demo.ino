#include <QuectelEC200U.h>

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
#ifndef AT_RX_PIN
#define AT_RX_PIN 16
#endif
#ifndef AT_TX_PIN
#define AT_TX_PIN 17
#endif
QuectelEC200U modem(SerialAT, 115200, AT_RX_PIN, AT_TX_PIN);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8);
QuectelEC200U modem(SerialAT);
#endif

void setup() {
  Serial.begin(115200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  modem.begin();

  modem.attachData("your.apn");
  modem.activatePDP(1);

  Serial.println("Opening TCP to example.com:80...");
  int sid = modem.tcpOpen("example.com", 80);
  if (sid >= 0) {
    String req = "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n";
    if (modem.tcpSend(sid, req)) {
      String out;
      if (modem.tcpRecv(sid, out, 512, 8000)) Serial.println(out);
    }
    modem.tcpClose(sid);
  } else {
    Serial.println("TCP open failed");
  }
}

void loop() {}