#include <QuectelEC200U.h>
#include "ca_cert.h"

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

QuectelEC200U* modem;

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial& SerialAT = Serial1;
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8);
#endif

const char* cert_path = "cloudflare_ca.pem";

void setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  modem = new QuectelEC200U(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
  modem = new QuectelEC200U(SerialAT);
  SerialAT.begin(9600);
#endif
  modem->begin();

  // Set your APN here
  modem->attachData("your.apn");
  modem->activatePDP(1);

  // Upload the Cloudflare CA certificate to the module's filesystem
  Serial.println("Uploading Cloudflare CA certificate...");
  if (modem->fsUpload(cert_path, cloudflare_ca_cert)) {
    Serial.println("Certificate uploaded.");
  } else {
    Serial.println("Failed to upload certificate.");
  }

  // Before making an HTTPS request, you must configure the SSL context.
  // This includes setting the path to your CA certificate on the module's filesystem.
  if (modem->sslConfigure(1, cert_path)) {
    Serial.println("SSL context configured.");
  } else {
    Serial.println("Failed to configure SSL context.");
  }

  String resp;
  Serial.println("HTTPS GET from Cloudflare...");
  if (modem->httpsGet("https://www.cloudflare.com/", resp)) {
    Serial.println("OK");
    Serial.println(resp);
  } else {
    Serial.println("HTTPS failed");
  }
}

void loop() {}