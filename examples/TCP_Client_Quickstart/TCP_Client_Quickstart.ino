#include <QuectelEC200U.h>

// Adjust these pins for your board
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17
#define EC200U_PWRKEY_PIN 10
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

static const char APN[] = "your.apn.here";
static const char APN_USER[] = "";
static const char APN_PASS[] = "";
static const int PDP_CONTEXT_ID = 1;

static const char TCP_HOST[] = "postman-echo.com";
static const int TCP_PORT = 80;
static const char TCP_PATH[] = "/post";

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
#elif defined(ARDUINO_ARCH_ZEPHYR)
  SerialAT.begin(115200);
#else
  SerialAT.begin(9600);
#endif

  Serial.println(F("\n== EC200U TCP Client Quickstart =="));
  requireStep(modem.begin(), F("Modem ready"));
  requireStep(modem.waitForNetwork(), F("Network registered"));
  requireStep(modem.attachData(APN, APN_USER, APN_PASS), F("Data attached"));
  requireStep(modem.activatePDP(PDP_CONTEXT_ID), F("PDP active"));

  Serial.println(F("\nOpening TCP socket..."));
  int socketId = modem.tcpOpen(TCP_HOST, TCP_PORT, PDP_CONTEXT_ID, 0);
  if (socketId < 0) {
    Serial.print(F("tcpOpen failed: "));
    Serial.println(modem.getLastErrorString());
    haltForever(F("Unable to open socket"));
  }

  String payload = F("POST ");
  payload += TCP_PATH;
  payload += F(" HTTP/1.1\r\nHost: ");
  payload += TCP_HOST;
  payload += F("\r\nContent-Type: application/json\r\nContent-Length: 18\r\nConnection: close\r\n\r\n{\"demo\":true}\n");

  Serial.println(F("Sending HTTP request over raw TCP..."));
  if (!modem.tcpSend(socketId, payload)) {
    Serial.println(F("tcpSend failed"));
    modem.tcpClose(socketId);
    haltForever(F("Send error"));
  }

  Serial.println(F("Waiting for response (up to 8 KB)..."));
  String response;
  if (modem.tcpRecv(socketId, response, 8192, 8000)) {
    Serial.println(F("--- SERVER RESPONSE ---"));
    Serial.println(response);
    Serial.println(F("------------------------"));
  } else {
    Serial.print(F("tcpRecv failed: "));
    Serial.println(modem.getLastErrorString());
  }

  modem.tcpClose(socketId);
  modem.deactivatePDP(PDP_CONTEXT_ID);
  Serial.println(F("TCP demo complete."));
}

void loop() {}
