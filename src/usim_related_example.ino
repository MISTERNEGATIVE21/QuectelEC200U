/*
  (U)SIM Related Commands example for QuectelEC200U_Adv library
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

  // --- (U)SIM Related Commands Example ---
  Serial.println("Getting IMSI...");
  String imsi = modem.getIMSI();
  Serial.println("IMSI: " + imsi);

  Serial.println("Getting ICCID...");
  String iccid = modem.getICCID();
  Serial.println("ICCID: " + iccid);

  Serial.println("Getting PIN Retries...");
  String pinRetries = modem.getPinRetries();
  Serial.println("PIN Retries: " + pinRetries);
  
  Serial.println("Getting SIM Status...");
  String simStatus = modem.getSIMStatus();
  Serial.println("SIM Status: " + simStatus);
}

void loop() {
  // Nothing to do here
}
