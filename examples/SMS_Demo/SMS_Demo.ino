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

  Serial.println("Sending SMS...");
  if (modem.sendSMS("+1234567890", "Hello from EC200U!")) {
    Serial.println("SMS sent");
  } else {
    Serial.println("Failed to send SMS");
  }
}

void loop() {}
