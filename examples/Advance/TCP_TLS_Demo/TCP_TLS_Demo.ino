#include <QuectelEC200U.h>

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
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