#include <QuectelEC200U.h>

#if defined(ARDUINO_ARCH_ESP32)
// ESP32: use UART1 with configurable pins
HardwareSerial SerialAT(1);
#ifndef AT_RX_PIN
#define AT_RX_PIN 16
#endif
#ifndef AT_TX_PIN
#define AT_TX_PIN 17
#endif
QuectelEC200U modem(SerialAT, 115200, AT_RX_PIN, AT_TX_PIN);
#else
// Other boards: use SoftwareSerial
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

  Serial.println("Sending SMS...");
  if (modem.sendSMS("+1234567890", "Hello from EC200U!")) {
    Serial.println("SMS sent");
  } else {
    Serial.println("Failed to send SMS");
  }
}

void loop() {}
