/*
  SoftwareSerial example for QuectelEC200U_Adv library
*/

#include <QuectelEC200U_Adv.h>
#include <SoftwareSerial.h>

// Define the SoftwareSerial pins
const int MODEM_RX_PIN = 10;
const int MODEM_TX_PIN = 11;
const int MODEM_PWR_PIN = 12; // Power pin for the modem

// Create a SoftwareSerial instance
SoftwareSerial modemSerial(MODEM_RX_PIN, MODEM_TX_PIN);

// Create a QuectelEC200U_Adv instance
QuectelEC200U_Adv modem(modemSerial);

void powerOnModem() {
  Serial.println("Powering on the modem...");
  pinMode(MODEM_PWR_PIN, OUTPUT);
  digitalWrite(MODEM_PWR_PIN, LOW);
  delay(1000);
  digitalWrite(MODEM_PWR_PIN, HIGH);
  delay(2000);
  digitalWrite(MODEM_PWR_PIN, LOW);
  delay(3000);
  Serial.println("Modem powered on.");
}

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  powerOnModem();

  // Set up modem serial communication
  modemSerial.begin(115200);

  // Enable debug output
  modem.enableDebug(Serial);

  // Initialize the modem
  if (!modem.begin()) {
    Serial.println("Failed to initialize modem!");
    while (1);
  }
  
  Serial.println("Modem initialized successfully!");

  // Get and print modem info
  String modemInfo = modem.getModemInfo();
  Serial.println("Modem Info:");
  Serial.println(modemInfo);
}

void loop() {
  // Nothing to do here
}
