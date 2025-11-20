/*
  Audio Commands example for QuectelEC200U_Adv library
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

  // --- Audio Example ---
  Serial.println("Setting speaker volume to 50...");
  if (modem.setSpeakerVolume(50)) {
    Serial.println("Speaker volume set!");
  } else {
    Serial.println("Failed to set speaker volume!");
  }

  Serial.println("Enabling audio loopback...");
  if (modem.audioLoopback(true)) {
    Serial.println("Audio loopback enabled!");
    delay(5000);
    modem.audioLoopback(false);
    Serial.println("Audio loopback disabled!");
  } else {
    Serial.println("Failed to enable audio loopback!");
  }

  // --- More Audio Commands Example ---
  Serial.println("Playing Text-to-Speech...");
  if (modem.playTextToSpeech("Hello from Gemini")) {
    Serial.println("TTS command sent!");
  } else {
    Serial.println("Failed to send TTS command!");
  }
}

void loop() {
  // Nothing to do here
}
