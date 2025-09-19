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

  String resp;
  Serial.println("HTTPS GET...");
  if (modem.httpsGet("https://example.com", resp)) {
    Serial.println("OK");
    Serial.println(resp);
  } else {
    Serial.println("HTTPS failed");
  }
}

void loop() {}