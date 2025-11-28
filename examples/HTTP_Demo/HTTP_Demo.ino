#include <QuectelEC200U.h>

#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17
#define EC200U_PW_KEY_PIN 10
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
QuectelEC200U modem(SerialAT);
#endif

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
#else
  SerialAT.begin(9600);
#endif

  Serial.println(F("\n== EC200U HTTP Demo =="));
  requireStep(modem.begin(), F("Modem ready"));
  requireStep(modem.waitForNetwork(), F("Network registered"));

  const char APN[] = "jionet";   // Replace with your APN
  const char USER[] = "";
  const char PASS[] = "";

  requireStep(modem.attachData(APN, USER, PASS), F("Data attached"));
  requireStep(modem.activatePDP(1), F("PDP context 1 active"));

  String response;
  String headers[] = {String("User-Agent: EC200U-HTTP-Demo")};
  const size_t headerCount = sizeof(headers) / sizeof(headers[0]);

  Serial.println(F("\nIssuing HTTP GET (blocking until response)..."));
  if (modem.httpGet("http://httpbin.org/get", response, headers, headerCount)) {
    Serial.println(F("HTTP GET Response:"));
    Serial.println(response);
  } else {
    Serial.print(F("HTTP GET failed: "));
    Serial.println(modem.getLastErrorString());
  }

  delay(1000);
  Serial.println(F("\nIssuing HTTP POST (blocking until response)..."));
  String payload = F("{\"demo\":\"ec200u\"}");
  if (modem.httpPost("http://httpbin.org/post", payload, response, headers, headerCount)) {
    Serial.println(F("HTTP POST Response:"));
    Serial.println(response);
  } else {
    Serial.print(F("HTTP POST failed: "));
    Serial.println(modem.getLastErrorString());
  }

  modem.deactivatePDP(1);
  Serial.println(F("\nHTTP demo finished."));
}

void loop() {}
