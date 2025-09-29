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
  delay(200);
#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
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
