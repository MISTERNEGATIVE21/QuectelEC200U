#include <QuectelEC200U.h>
#include <QuectelEC200U_CN.h>

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

  if (!modem.isSimReady()) Serial.println("SIM not ready");
  if (!modem.waitForNetwork()) Serial.println("No network");

  // Set APN and bring up PDP
  modem.attachData("your.apn.here");
  modem.activatePDP(1);

  int sock = modem.tcpOpen("example.com", 80, 1, 0);
  if (sock >= 0) {
    modem.tcpSend(sock, "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n");
    String body;
    if (modem.tcpRecv(sock, body, 1024, 8000)) {
      Serial.println(body);
    }
    modem.tcpClose(sock);
  } else {
    Serial.println("TCP open failed");
  }
}

void loop() {}