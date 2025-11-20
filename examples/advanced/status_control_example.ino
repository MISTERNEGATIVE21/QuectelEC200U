/*
  Status Control and Extended Settings example for QuectelEC200U_Adv library
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

  // --- Status Control and Extended Settings Example ---
  Serial.println("Getting Activity Status...");
  String activityStatus = modem.getActivityStatus();
  Serial.println("Activity Status: " + activityStatus);

  Serial.println("Enabling 'csq' URC indication...");
  if (modem.setURCIndication("csq", true)) {
    Serial.println("'csq' URC indication enabled!");
  } else {
    Serial.println("Failed to enable 'csq' URC indication!");
  }
}

void loop() {
  // Nothing to do here
}
