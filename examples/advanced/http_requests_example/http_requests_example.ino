/*
  HTTP/S GET and POST example for QuectelEC200U library
*/

#include <QuectelEC200U.h>
#include <ArduinoJson.h>

// Use Hardware Serial for communication with the modem


// Create a QuectelEC200U instance

#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial modemSerial(1);
#elif defined(ARDUINO_ARCH_ZEPHYR)
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
  
  // Get APN dynamically
  QuectelEC200U::PDPContext pdp_ctx = modem.getPDPContext(1);
  if (pdp_ctx.cid == -1 || pdp_ctx.apn.length() == 0) {
    Serial.println("Failed to retrieve APN dynamically. Please check modem configuration or set APN manually.");
    while (1);
  }
  const char* APN = pdp_ctx.apn.c_str();

  // Attach to data network
  if (!modem.attachData(APN)) {
    Serial.println("Failed to attach to data network!");
    while (1);
  }

  // Activate PDP context
  if (!modem.activatePDP()) {
    Serial.println("Failed to activate PDP context!");
    while (1);
  }
  
  // --- HTTP GET Example ---
  Serial.println("Performing HTTP GET request...");
  String httpGetResponse;
  if (modem.httpGet("http://httpbin.org/get", httpGetResponse)) {
    Serial.println("HTTP GET successful!");
    Serial.println("Response: " + httpGetResponse);
  } else {
    Serial.println("HTTP GET failed!");
  }

  // --- HTTP POST Example ---
  Serial.println("\nPerforming HTTP POST request...");
  String httpPostResponse;
  String postData = "param1=value1&param2=value2";
  if (modem.httpPost("http://httpbin.org/post", postData, httpPostResponse)) {
    Serial.println("HTTP POST successful!");
    Serial.println("Response: " + httpPostResponse);
  } else {
    Serial.println("HTTP POST failed!");
  }

  // --- HTTPS GET Example ---
  Serial.println("\nPerforming HTTPS GET request...");
  String httpsGetResponse;
  if (modem.httpsGet("https://httpbin.org/get", httpsGetResponse)) {
    Serial.println("HTTPS GET successful!");
    Serial.println("Response: " + httpsGetResponse);
  } else {
    Serial.println("HTTPS GET failed!");
  }

  // --- HTTPS POST with JSON Example ---
  Serial.println("\nPerforming HTTPS POST request with JSON...");
  String httpsPostJsonResponse;
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["sensor"] = "gps";
  jsonDoc["time"] = 1351824120;
  JsonArray data = jsonDoc.createNestedArray("data");
  data.add(48.756080);
  data.add(2.302038);
  
  if (modem.httpsPost("https://httpbin.org/post", jsonDoc, httpsPostJsonResponse)) {
    Serial.println("HTTPS POST with JSON successful!");
    Serial.println("Response: " + httpsPostJsonResponse);
  } else {
    Serial.println("HTTPS POST with JSON failed!");
  }
}

void loop() {
  // Nothing to do here
}
