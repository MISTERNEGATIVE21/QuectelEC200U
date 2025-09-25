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
  delay(200);
#if defined(ARDUINO_ARCH_ESP32)
  // HardwareSerial is configured in modem.begin() for ESP32
#else
  SerialAT.begin(9600);
#endif
  modem.begin();

  // Enable caller ID notifications
  modem.enableCallerId(true);

  // Dial a number (replace with a valid number)
  Serial.println("Dialing...");
  if (modem.dial("+1234567890")) {
    Serial.println("Dial command sent. Hanging up in 5 seconds...");
    delay(5000);
    modem.hangup();
  } else {
    Serial.println("Dial failed");
  }

  // Print current calls (if any)
  Serial.println(modem.getCallList());
}

void loop() {}
