/*
  Call-Related Commands example for QuectelEC200U library
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

  // Wait for network registration
  if (!modem.waitForNetwork()) {
    Serial.println("Failed to register on the network!");
    while (1);
  }

  // --- Call-Related Commands Example ---
  Serial.println("Setting voice hangup control to ATH only...");
  if (modem.setVoiceHangupControl(0)) {
    Serial.println("Voice hangup control set!");
  } else {
    Serial.println("Failed to set voice hangup control!");
  }

  Serial.println("Setting connection timeout to 30 seconds...");
  if (modem.setConnectionTimeout(30)) {
    Serial.println("Connection timeout set!");
  } else {
    Serial.println("Failed to set connection timeout!");
  }
  
  // Note: The following commands will initiate a voice call.
  // Serial.println("Dialing number...");
  // if (modem.dial("1234567890")) {
  //   Serial.println("Dialing...");
  //   delay(10000); // Wait for 10 seconds
  //   Serial.println("Hanging up...");
  //   modem.hangup();
  // } else {
  //   Serial.println("Failed to dial!");
  // }
}

void loop() {
  // Nothing to do here
}
