/*
  General Commands example for QuectelEC200U library
*/

#include <QuectelEC200U.h>

// Use Hardware Serial for communication with the modem


// Create a QuectelEC200U instance

#if defined(ARDUINO_ARCH_ESP32)
  
#else
  #include <SoftwareSerial.h>
  SoftwareSerial modemSerial(10, 11);
#endif

#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial& modemSerial = Serial1;
#else
  #include <SoftwareSerial.h>
  SoftwareSerial modemSerial(10, 11);
#endif
QuectelEC200U modem(modemSerial);

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Set up modem serial communication
  modemSerial.begin(115200);

  // Enable debug output
  modem.enableDebug(Serial);

  // Initialize the modem
  if (!modem.begin()) {
    Serial.println("Failed to initialize modem!");
    while (1);
  }

  // --- General Commands Example ---
  Serial.println("Setting command echo OFF...");
  modem.setCommandEcho(false);
  delay(1000); // Give modem time to process

  Serial.println("Setting command echo ON...");
  modem.setCommandEcho(true);
  delay(1000);

  Serial.println("Setting verbose error messages...");
  modem.setErrorMessageFormat(2);
  delay(1000);
  
  Serial.println("Getting Manufacturer Identification...");
  String manufacturer = modem.getManufacturerIdentification();
  Serial.println("Manufacturer: " + manufacturer);

  Serial.println("Getting Model Identification...");
  String model = modem.getModelIdentification();
  Serial.println("Model: " + model);

  Serial.println("Getting Firmware Revision...");
  String firmware = modem.getFirmwareRevision();
  Serial.println("Firmware Revision: " + firmware);

  // Note: restoreFactoryDefaults will reset the modem's settings
  // Serial.println("Restoring factory defaults...");
  // modem.restoreFactoryDefaults();
  // delay(5000); // Wait for modem to reset
}

void loop() {
  // Nothing to do here
}
