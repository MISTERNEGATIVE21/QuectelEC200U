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

  Serial.println("Starting GNSS...");
  if (modem.startGNSS()) {
    delay(2000); // wait briefly for a fix (demo)
    Serial.println("Reading GNSS location...");
    Serial.println(modem.getGNSSLocation());
  } else {
    Serial.println("Failed to start GNSS");
  }
}

void loop() {
}
