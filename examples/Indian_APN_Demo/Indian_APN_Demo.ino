/*
  Indian APN Demo for Quectel EC200U

  This example demonstrates how to use the QuectelEC200U library with common Indian mobile carriers.
  It shows how to get the network operator and set the appropriate APN.
*/

#include <QuectelEC200U.h>

// For ESP32, you can use HardwareSerial
#if defined(ARDUINO_ARCH_ESP32)
 // Use Serial1, Serial2, etc. as available
#else
// For other boards, you might need SoftwareSerial
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8); // RX, TX
#endif

// APN settings for common Indian carriers
const char* JIO_APN = "jionet";
const char* AIRTEL_APN = "airtelgprs.com";
const char* VI_APN = "www";
const char* BSNL_APN = "bsnlnet";

void setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  
#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial& SerialAT = Serial1;
#else
  #include <SoftwareSerial.h>
  SoftwareSerial SerialAT(10, 11);
#endif
QuectelEC200U modem(SerialAT);
#else
  QuectelEC200U modem(SerialAT);
#endif
  SerialAT.begin(115200); // Or your desired baud rate

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
