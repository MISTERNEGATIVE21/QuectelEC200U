#include <QuectelEC200U.h>
#if defined(ARDUINO_ARCH_ESP32)
#include "../../src/EC200U_ESP32_Config.h"
HardwareSerial& SerialAT = EC200U_UART; // Serial2
QuectelEC200U modem(SerialAT, 115200, EC200U_RX, EC200U_TX);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8);
QuectelEC200U modem(SerialAT);
#endif

void setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#else
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