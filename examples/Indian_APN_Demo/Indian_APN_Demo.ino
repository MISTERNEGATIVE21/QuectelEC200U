/*
  Indian APN Demo for Quectel EC200U

  This example demonstrates how to use the QuectelEC200U library with common Indian mobile carriers.
  It shows how to get the network operator and set the appropriate APN.
*/

#include <QuectelEC200U.h>

// Pin Definitions
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17
#define EC200U_PW_KEY_PIN 10

#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial& SerialAT = Serial1;
  QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
  #include <SoftwareSerial.h>
  SoftwareSerial SerialAT(7, 8); // RX, TX
  QuectelEC200U modem(SerialAT);
#endif

// Power on the modem (one-time operation)
void EC200U_powerOn() {
#if defined(ARDUINO_ARCH_ESP32)
  pinMode(EC200U_PW_KEY_PIN, OUTPUT);
  digitalWrite(EC200U_PW_KEY_PIN, LOW);
  delay(2000); // Wait for modem to power on
#endif
}

void setup() {
  Serial.begin(115200);
  #if defined(ARDUINO_ARCH_ESP32)
    EC200U_powerOn();
  #else
    SerialAT.begin(9600);
  #endif

  Serial.println("Initializing modem...");
  if (modem.begin()) {
    Serial.println("Modem initialized.");
  } else {
    Serial.println("Failed to initialize modem.");
    while (1);
  }

  Serial.println("Waiting for network...");
  if (modem.waitForNetwork()) {
    Serial.println("Network connected.");
  } else {
    Serial.println("Failed to connect to network.");
    while (1);
  }

  // Get the network operator
  String operatorName = modem.getOperator();
  Serial.print("Operator: ");
  Serial.println(operatorName);

  // Set the APN based on the operator
  String apn = "";
  if (operatorName.indexOf("Jio") != -1) {
    apn = JIO_APN;
  } else if (operatorName.indexOf("Airtel") != -1) {
    apn = AIRTEL_APN;
  } else if (operatorName.indexOf("Vodafone") != -1 || operatorName.indexOf("Idea") != -1 || operatorName.indexOf("Vi") != -1) {
    apn = VI_APN;
  } else if (operatorName.indexOf("BSNL") != -1) {
    apn = BSNL_APN;
  } else {
    Serial.println("Unknown operator. Please set the APN manually.");
    // You can set a default APN here if you want
    // apn = "your.apn";
  }

  if (apn.length() > 0) {
    Serial.print("Setting APN to: ");
    Serial.println(apn);
    modem.attachData(apn.c_str());
  }

  String response;
  Serial.println("Performing HTTP GET...");
  if (modem.httpGet("http://example.com", response)) {
    Serial.println("Response:");
    Serial.println(response);
  } else {
    Serial.println("HTTP GET failed.");
  }
}

void loop() {
  // Your code here
}
