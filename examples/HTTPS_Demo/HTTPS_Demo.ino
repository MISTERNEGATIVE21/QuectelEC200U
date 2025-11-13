#include <QuectelEC200U.h>
#include "ca_cert.h"

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

// Set the EC200U modem power key pin
#define EC200U_PW_KEY_PIN 10

// Set the EC200U modem power status pin
#define EC200U_STATUS_PIN 2

QuectelEC200U* modem;

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial& SerialAT = Serial2;

void EC200U_powerOn() {
  pinMode(EC200U_PW_KEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);

  // Check if the modem is already on
  if (digitalRead(EC200U_STATUS_PIN) == LOW) {
    // Power on the modem
    digitalWrite(EC200U_PW_KEY_PIN, LOW);
    delay(500);
    digitalWrite(EC200U_PW_KEY_PIN, HIGH);
    delay(3000);
  }
}
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
#endif

const char* cert_path = "https_ca.pem";

void setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  modem = new QuectelEC200U(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
  EC200U_powerOn();
#else
  modem = new QuectelEC200U(SerialAT);
  SerialAT.begin(9600);
#endif
  modem->begin();

  modem->attachData("your.apn");
  modem->activatePDP(1);

  // Upload the CA certificate to the module's filesystem
  // Note: You should use the correct CA certificate for your host
  Serial.println("Uploading CA certificate...");
  if (modem->fsUpload(cert_path, cloudflare_ca_cert)) {
    Serial.println("Certificate uploaded.");
  } else {
    Serial.println("Failed to upload certificate.");
  }

  // Before making an HTTPS request, you must configure the SSL context.
  if (modem->sslConfigure(1, cert_path)) {
    Serial.println("SSL context configured.");
  } else {
    Serial.println("Failed to configure SSL context.");
  }

  String response;
  if (modem->httpsGet("https://www.cloudflare.com/", response)) {
    Serial.println("HTTPS Response:");
    Serial.println(response);
  } else {
    Serial.println("HTTPS request failed");
  }
}

void loop() {}