/*
  Packet Domain Commands example for QuectelEC200U_Adv library
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

  // --- Packet Domain Commands Example ---
  Serial.println("Attaching to GPRS...");
  if (modem.gprsAttach(true)) {
    Serial.println("GPRS attached!");
  } else {
    Serial.println("Failed to attach to GPRS!");
  }

  Serial.println("Setting Packet Domain Event Reporting...");
  if (modem.setPacketDomainEventReporting(1)) {
    Serial.println("Packet Domain Event Reporting set!");
  } else {
    Serial.println("Failed to set Packet Domain Event Reporting!");
  }
  
  Serial.println("Getting packet data counter...");
  String dataCounter = modem.getPacketDataCounter();
  Serial.println("Packet data counter: " + dataCounter);

  Serial.println("Reading dynamic PDN parameters...");
  String pdnParams = modem.readDynamicPDNParameters();
  Serial.println("Dynamic PDN parameters: " + pdnParams);
}

void loop() {
  // Nothing to do here
}
