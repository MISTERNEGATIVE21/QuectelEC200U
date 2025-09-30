#include <QuectelEC200U.h>
#if defined(ARDUINO_ARCH_ESP32)
#include <EC200U_ESP32_Config.h>
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
