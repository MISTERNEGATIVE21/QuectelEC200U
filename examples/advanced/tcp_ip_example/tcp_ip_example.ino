/*
  TCP/IP Commands example for QuectelEC200U_Adv library
*/

#include <QuectelEC200U_Adv.h>
#include <ArduinoJson.h>

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

  // Wait for network registration
  if (!modem.waitForNetwork()) {
    Serial.println("Failed to register on the network!");
    while (1);
  }
  
  // Get APN dynamically
  QuectelEC200U_Adv::PDPContext pdp_ctx = modem.getPDPContext(1);
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
  
  // --- TCP/IP Commands Example ---
  Serial.println("Opening TCP connection to httpbin.org on port 80...");
  int socketId = modem.tcpOpen("httpbin.org", 80);
  if (socketId != -1) {
    Serial.println("TCP connection successful! Socket ID: " + String(socketId));
    
    Serial.println("Sending data...");
    String dataToSend = "GET /get HTTP/1.1\r\nHost: httpbin.org\r\n\r\n";
    if (modem.tcpSend(socketId, dataToSend)) {
      Serial.println("Data sent!");
      
      delay(2000); // Wait for response
      
      Serial.println("Receiving data...");
      String response;
      if (modem.tcpRecv(socketId, response)) {
        Serial.println("Response: " + response);
      } else {
        Serial.println("Failed to receive data!");
      }
    } else {
      Serial.println("Failed to send data!");
    }
    
    Serial.println("Closing TCP connection...");
    modem.tcpClose(socketId);
    
  } else {
    Serial.println("Failed to open TCP connection!");
  }
}

void loop() {
  // Nothing to do here
}
