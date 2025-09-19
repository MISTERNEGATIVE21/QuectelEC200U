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

  Serial.println("Starting GNSS...");
  if (modem.startGNSS()) {
    delay(2000); // wait briefly for a fix (demo)
    Serial.println("Reading GNSS location...");
    Serial.println(modem.getGNSSLocation());
  } else {
    Serial.println("Failed to start GNSS");
  }
}

void loop() {
}
