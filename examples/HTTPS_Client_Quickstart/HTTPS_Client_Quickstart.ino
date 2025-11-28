#include <QuectelEC200U.h>
#include "ca_cert.h"

#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17
#define EC200U_PWRKEY_PIN 10
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
QuectelEC200U modem(SerialAT);
#endif

static const char APN[] = "your.apn.here";
static const int PDP_CONTEXT_ID = 1;
static const int SSL_CTX_ID = 1;
static const char CERT_PATH[] = "cloudflare.pem";

static const char HTTPS_URL[] = "https://www.cloudflare.com/";

#if defined(ARDUINO_ARCH_ESP32)
static void powerOnModem() {
  pinMode(EC200U_PWRKEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);
  if (digitalRead(EC200U_STATUS_PIN) == LOW) {
    digitalWrite(EC200U_PWRKEY_PIN, LOW);
    delay(2000);
    digitalWrite(EC200U_PWRKEY_PIN, HIGH);
    delay(200);
  }
}
#endif

static void haltForever(const __FlashStringHelper *reason) {
  Serial.println(reason);
  while (true) {
    delay(1000);
  }
}

static void requireStep(bool ok, const __FlashStringHelper *label) {
  if (ok) {
    Serial.print(F("[ OK ] "));
    Serial.println(label);
  } else {
    Serial.print(F("[FAIL] "));
    Serial.println(label);
    Serial.print(F("Reason: "));
    Serial.println(modem.getLastErrorString());
    haltForever(F("Stopping demo"));
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

#if defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(115200, SERIAL_8N1, EC200U_RX_PIN, EC200U_TX_PIN);
  powerOnModem();
#else
  SerialAT.begin(9600);
#endif

  Serial.println(F("\n== EC200U HTTPS Client Quickstart =="));
  requireStep(modem.begin(), F("Modem ready"));
  requireStep(modem.waitForNetwork(), F("Network registered"));
  requireStep(modem.attachData(APN), F("Data attached"));
  requireStep(modem.activatePDP(PDP_CONTEXT_ID), F("PDP active"));

  Serial.println(F("\nUploading CA certificate (blocking)..."));
  requireStep(modem.sslUploadCert(cloudflare_ca_cert, CERT_PATH), F("CA stored"));
  requireStep(modem.sslConfigure(SSL_CTX_ID, CERT_PATH), F("SSL configured"));

  Serial.print(F("Issuing HTTPS GET to: "));
  Serial.println(HTTPS_URL);

  String response;
  if (modem.httpsGet(HTTPS_URL, response)) {
    Serial.println(F("HTTPS response:"));
    Serial.println(response);
  } else {
    Serial.print(F("HTTPS GET failed: "));
    Serial.println(modem.getLastErrorString());
  }

  modem.deactivatePDP(PDP_CONTEXT_ID);
  Serial.println(F("HTTPS demo complete."));
}

void loop() {}
