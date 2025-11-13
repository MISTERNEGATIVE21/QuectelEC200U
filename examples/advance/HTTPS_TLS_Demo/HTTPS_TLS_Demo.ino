#include <QuectelEC200U.h>

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8);
QuectelEC200U modem(SerialAT);
#endif

void setup() {
  Serial.begin(115200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  modem.begin();

  // Set your APN here
  modem.attachData("your.apn");
  modem.activatePDP(1);

  // Before making an HTTPS request, you must configure the SSL context.
  // This includes setting the path to your CA certificate on the module's filesystem.
  // Upload your CA certificate to the module's filesystem first.
  // In this example, we assume the certificate is named "cacert.pem".
  if (modem.sslConfigure(1, "cacert.pem")) {
    Serial.println("SSL context configured.");
  } else {
    Serial.println("Failed to configure SSL context.");
  }

  String resp;
  Serial.println("HTTPS GET...");
  if (modem.httpsGet("https://example.com", resp)) {
    Serial.println("OK");
    Serial.println(resp);
  } else {
    Serial.println("HTTPS failed");
  }
}

void loop() {}