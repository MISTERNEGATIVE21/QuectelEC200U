#include <QuectelEC200U.h>
#include "ca_cert.h"

#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17
#define EC200U_PW_KEY_PIN 10
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial SerialAT(1);
  QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#elif defined(ARDUINO_ARCH_ZEPHYR)
  HardwareSerial& SerialAT = Serial1;
  QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
  #include <SoftwareSerial.h>
  SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
  QuectelEC200U modem(SerialAT);
#endif

static const char* CERT_PATH = "https_ca.pem";

#if defined(ARDUINO_ARCH_ESP32)
static void powerOnModem() {
  pinMode(EC200U_PW_KEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);
  if (digitalRead(EC200U_STATUS_PIN) == LOW) {
    digitalWrite(EC200U_PW_KEY_PIN, LOW);
    delay(2000);
    digitalWrite(EC200U_PW_KEY_PIN, HIGH);
  }
}
#else
static void powerOnModem() {}
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
    haltForever(F("Stopping demo"));
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

#if defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(115200, SERIAL_8N1, EC200U_RX_PIN, EC200U_TX_PIN);
  powerOnModem();
#elif defined(ARDUINO_ARCH_ZEPHYR)
  SerialAT.begin(115200);
#else
  SerialAT.begin(9600);
#endif

  Serial.println(F("\n== EC200U HTTPS Demo =="));
  requireStep(modem.begin(), F("Modem ready"));
  requireStep(modem.waitForNetwork(), F("Network registered"));

  const char APN[] = "jionet";   // Replace with your APN
  const char USER[] = "";
  const char PASS[] = "";

  requireStep(modem.attachData(APN, USER, PASS), F("Data attached"));
  requireStep(modem.activatePDP(1), F("PDP context 1 active"));

  Serial.println(F("\nUploading TLS certificate (blocking)..."));
  requireStep(modem.sslUploadCert(cloudflare_ca_cert, CERT_PATH), F("CA stored"));
  requireStep(modem.sslConfigure(1, CERT_PATH), F("SSL ctx configured"));

  String response;
  String headers[] = {String("User-Agent: EC200U-HTTPS-Demo")};
  const size_t headerCount = sizeof(headers) / sizeof(headers[0]);

  Serial.println(F("\nIssuing HTTPS GET (blocking until response)..."));
  if (modem.httpsGet("https://www.cloudflare.com/", response, headers, headerCount)) {
    Serial.println(F("HTTPS GET Response:"));
    Serial.println(response);
  } else {
    Serial.print(F("HTTPS GET failed: "));
    Serial.println(modem.getLastErrorString());
  }

  modem.deactivatePDP(1);
  Serial.println(F("\nHTTPS demo finished."));
}

void loop() {}