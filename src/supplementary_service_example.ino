/*
  Supplementary Service Commands example for QuectelEC200U_Adv library
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

  // --- Supplementary Service Commands Example ---
  Serial.println("Enabling Call Waiting...");
  if (modem.setCallWaiting(1)) {
    Serial.println("Call Waiting enabled!");
  } else {
    Serial.println("Failed to enable Call Waiting!");
  }

  Serial.println("Enabling Calling Line Identification Presentation...");
  if (modem.setCallingLineIdentificationPresentation(true)) {
    Serial.println("Calling Line Identification Presentation enabled!");
  } else {
    Serial.println("Failed to enable Calling Line Identification Presentation!");
  }
  
  // Note: The following command will set call forwarding.
  // Serial.println("Setting call forwarding...");
  // if (modem.setCallForwarding(0, 1, "+1234567890")) {
  //   Serial.println("Call forwarding set!");
  // } else {
  //   Serial.println("Failed to set call forwarding!");
  // }
}

void loop() {
  // Nothing to do here
}
