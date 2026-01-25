/*
  Modem Config Example for Quectel EC200U
  Demonstrates:
  - General modem configuration using setModemConfig
*/

#include <QuectelEC200U.h>

// Define serial pins for ESP32
#define RX_PIN 16
#define TX_PIN 17

#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial modemSerial(1);
  QuectelEC200U modem(modemSerial, 115200, RX_PIN, TX_PIN);
#elif defined(ARDUINO_ARCH_ZEPHYR)
  HardwareSerial& modemSerial = Serial1;
  QuectelEC200U modem(modemSerial, 115200, RX_PIN, TX_PIN);
#else
  HardwareSerial modemSerial(1);
  QuectelEC200U modem(modemSerial, 115200, RX_PIN, TX_PIN);
#endif

void setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ZEPHYR)
  modemSerial.begin(115200);
#else
  modemSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
#endif
  delay(1000);
  Serial.println("Starting Modem Config Example...");

  // Initialize modem
  if (!modem.begin()) {
    Serial.println("Modem initialization failed!");
    return;
  }

  Serial.println("Modem initialized.");

  // Example: Set network search mode to LTE only (Value depends on module, e.g., 3 for LTE only)
  // Refer to AT command manual for specific values
  if (modem.setModemConfig("nwscanmode", "3")) {
    Serial.println("Network scan mode set to LTE only.");
  } else {
    Serial.println("Failed to set network scan mode.");
  }

  // Example: Configure band (Example values, refer to manual)
  // if (modem.setModemConfig("band", "0,1")) { ... }
}

void loop() {
  // Nothing to do here
}
