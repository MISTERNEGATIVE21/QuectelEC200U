/*
  SMS Commands example for QuectelEC200U library
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

  // Wait for network registration
  if (!modem.waitForNetwork()) {
    Serial.println("Failed to register on the network!");
    while (1);
  }

  // --- SMS Commands Example ---
  Serial.println("Setting SMS message format to text mode (1)...");
  if (modem.setMessageFormat(1)) {
    Serial.println("Message format set to text mode!");
  } else {
    Serial.println("Failed to set message format!");
  }

  Serial.println("Listing all unread messages...");
  String messages = modem.listMessages("REC UNREAD");
  Serial.println("Unread messages: " + messages);

  // Set URC to route SMS to serial port
  Serial.println("Setting new message indication to route to serial port...");
  if (modem.setNewMessageIndication(2, 1, 0, 0, 0)) {
    Serial.println("New message indication set!");
  } else {
    Serial.println("Failed to set new message indication!");
  }
  
  // Note: The following command will send an SMS.
  // Serial.println("Sending SMS...");
  // if (modem.sendSMS("+1234567890", "Hello from Gemini")) {
  //   Serial.println("SMS sent!");
  // } else {
  //   Serial.println("Failed to send SMS!");
  // }
}

void loop() {
  // Nothing to do here
}
