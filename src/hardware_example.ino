/*
  Hardware Related Commands example for QuectelEC200U_Adv library
*/

#include <QuectelEC200U_Adv.h>

// Use Hardware Serial for communication with the modem
HardwareSerial& modemSerial = Serial1;

// Create a QuectelEC200U_Adv instance
QuectelEC200U_Adv modem(modemSerial);

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

  // --- Hardware Example ---
  Serial.println("Getting battery charge status...");
  String batteryStatus = modem.getBatteryCharge();
  Serial.println("Battery Status: " + batteryStatus);

  Serial.println("Performing Wi-Fi scan...");
  String wifiScan = modem.getWifiScan();
  Serial.println("Wi-Fi Scan Results: " + wifiScan);

  Serial.println("Reading ADC value...");
  int adcValue = modem.readADC();
  if (adcValue != -1) {
    Serial.println("ADC value: " + String(adcValue));
  } else {
    Serial.println("Failed to read ADC value!");
  }
  
  // Note: This command will power off the modem.
  // Serial.println("Powering off modem...");
  // if (modem.powerOff()) {
  //   Serial.println("Modem powered off!");
  // } else {
  //   Serial.println("Failed to power off modem!");
  // }
}

void loop() {
  // Nothing to do here
}
