/*
  SoftwareSerial example for QuectelEC200U_Adv library
*/

#include <QuectelEC200U_Adv.h>

#if defined(ARDUINO_ARCH_ESP32)
// For ESP32, we use HardwareSerial
// Define the HardwareSerial pins. These are just examples and may need to be changed based on your board.
const int MODEM_RX_PIN = 16;
const int MODEM_TX_PIN = 17;
const int MODEM_PWR_PIN = 12; // Power pin for the modem

// Use a HardwareSerial port. On some ESP32 boards, this might be Serial2.
// The ESP32 core suggests Serial1 might be available. Check your board's pinout.
HardwareSerial& modemSerial = Serial1;

// Create a QuectelEC200U_Adv instance for ESP32
QuectelEC200U_Adv modem(modemSerial, 115200, MODEM_RX_PIN, MODEM_TX_PIN);

#else
// For other Arduino boards, we use SoftwareSerial
#include <SoftwareSerial.h>

// Define the SoftwareSerial pins
const int MODEM_RX_PIN = 10;
const int MODEM_TX_PIN = 11;
const int MODEM_PWR_PIN = 12; // Power pin for the modem

// Create a SoftwareSerial instance
SoftwareSerial modemSerial(MODEM_RX_PIN, MODEM_TX_PIN);

// Create a QuectelEC200U_Adv instance
QuectelEC200U_Adv modem(modemSerial);
#endif

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

#if !defined(ARDUINO_ARCH_ESP32)
  // Set up modem serial communication for SoftwareSerial
  modemSerial.begin(115200);
#endif

  // Enable debug output
  modem.enableDebug(Serial);

  // Initialize the modem
  // For ESP32, begin() will also initialize the HardwareSerial port with the pins specified in the constructor
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
