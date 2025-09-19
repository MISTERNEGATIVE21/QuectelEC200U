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

const char* CA_PEM = "-----BEGIN CERTIFICATE-----\n...your CA PEM here...\n-----END CERTIFICATE-----\n";

void setup() {
  Serial.begin(115200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  modem.begin();

  Serial.println("Filesystem CA upload demo");
  bool ok = modem.fsUpload("ca.pem", CA_PEM);
  Serial.println(ok ? "Upload OK" : "Upload FAIL");

  String list;
  if (modem.fsList(list)) Serial.println(list);

  String content;
  if (modem.fsRead("ca.pem", content, 1024)) Serial.println(content);
}

void loop() {}