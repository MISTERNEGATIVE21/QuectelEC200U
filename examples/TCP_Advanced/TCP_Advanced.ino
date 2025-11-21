/*
  TCP Advanced Example for Quectel EC200U
  Demonstrates:
  - Configuring PDP context
  - Asynchronous PDP activation
  - Advanced TCP configuration
  - Socket status querying
  - Error handling
*/

#include <QuectelEC200U.h>

// Define serial pins for ESP32
#define RX_PIN 16
#define TX_PIN 17

HardwareSerial modemSerial(1);
QuectelEC200U modem(modemSerial, 115200, RX_PIN, TX_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting TCP Advanced Example...");

  // Initialize modem
  if (!modem.begin()) {
    Serial.println("Modem initialization failed!");
    return;
  }

  Serial.println("Modem initialized.");

  // Configure Context (APN, User, Pass, Auth)
  // Example: Context 1, IPv4, "internet", "", "", 0 (None)
  if (modem.configureContext(1, 1, "internet", "", "", 0)) {
    Serial.println("Context configured.");
  } else {
    Serial.println("Context configuration failed.");
  }

  // Asynchronous PDP Activation
  Serial.println("Activating PDP context asynchronously...");
  if (modem.activatePDPAsync(1)) {
    Serial.println("Activation command sent.");
    // In a real application, you would wait for the URC +QIACTEX: ...
    delay(5000); // Simulating wait
  } else {
    Serial.println("Activation failed.");
  }

  // Advanced TCP Configuration
  // Example: Set TCP retransmission count to 5
  if (modem.setTCPConfig("tcp/retranscfg", "5,10")) {
    Serial.println("TCP config set.");
  } else {
    Serial.println("TCP config failed.");
  }

  // Open a socket (Example)
  int socketId = modem.tcpOpen("example.com", 80);
  if (socketId != -1) {
    Serial.println("Socket opened.");
    
    // Check Socket Status
    String status = modem.getSocketStatus(socketId);
    Serial.println("Socket Status: " + status);

    // Close socket
    modem.tcpClose(socketId);
  } else {
    Serial.println("Socket open failed.");
    int err = modem.getTCPError();
    Serial.println("TCP Error Code: " + String(err));
  }
}

void loop() {
  // Nothing to do here
}
