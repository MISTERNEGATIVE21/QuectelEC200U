/*
  Network Service Commands example for QuectelEC200U_Adv library
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

  // Wait for network registration
  if (!modem.waitForNetwork()) {
    Serial.println("Failed to register on the network!");
    while (1);
  }

  // --- Network Service Commands Example ---
  Serial.println("Getting Detailed Signal Quality...");
  String csq = modem.getDetailedSignalQuality();
  Serial.println("Detailed Signal Quality: " + csq);

  Serial.println("Getting Network Time...");
  String nitz = modem.getNetworkTime();
  Serial.println("Network Time: " + nitz);

  Serial.println("Getting Network Info...");
  String nwInfo = modem.getNetworkInfo();
  Serial.println("Network Info: " + nwInfo);
  
  Serial.println("Getting Operator...");
  String op = modem.getOperator();
  Serial.println("Operator: " + op);
}

void loop() {
  // Nothing to do here
}
