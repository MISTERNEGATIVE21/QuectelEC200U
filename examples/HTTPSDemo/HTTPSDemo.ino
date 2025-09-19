#include <QuectelEC200U.h>

// For ESP32/ESP8266 or other boards with a second hardware serial
HardwareSerial& modemSerial = Serial1;
QuectelEC200U modem(modemSerial);

// --- OR ---
// For boards like Arduino Uno, use SoftwareSerial
// #include <SoftwareSerial.h>
// SoftwareSerial modemSerial(7, 8); // RX, TX
// QuectelEC200U modem(modemSerial);


void setup() {
  Serial.begin(115200);
  
  // Initialize your chosen serial port
  modemSerial.begin(115200);

  // Initialize the modem
  modem.begin();

  String response;
  if (modem.httpsGet("https://example.com", response)) {
    Serial.println("HTTPS Response:");
    Serial.println(response);
  } else {
    Serial.println("HTTPS request failed");
  }
}

void loop() {}