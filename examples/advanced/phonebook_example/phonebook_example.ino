/*
  Phonebook Commands example for QuectelEC200U library
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
  HardwareSerial modemSerial(1);
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

  // --- Phonebook Commands Example ---
  Serial.println("Selecting SIM phonebook storage...");
  if (modem.selectPhonebookStorage("SM")) {
    Serial.println("Phonebook storage set to SIM!");

    Serial.println("Writing entry to index 1...");
    if (modem.writePhonebookEntry(1, "1234567890", "Test")) {
      Serial.println("Entry written!");
    } else {
      Serial.println("Failed to write entry!");
    }

    Serial.println("Reading entry from index 1...");
    String entry = modem.readPhonebookEntry(1);
    Serial.println("Entry 1: " + entry);

    Serial.println("Finding entry 'Test'...");
    String found = modem.findPhonebookEntries("Test");
    Serial.println("Found entries: " + found);
    
    Serial.println("Getting subscriber number...");
    String subNum = modem.getSubscriberNumber();
    Serial.println("Subscriber Number: " + subNum);

  } else {
    Serial.println("Failed to set phonebook storage!");
  }
}

void loop() {
  // Nothing to do here
}
