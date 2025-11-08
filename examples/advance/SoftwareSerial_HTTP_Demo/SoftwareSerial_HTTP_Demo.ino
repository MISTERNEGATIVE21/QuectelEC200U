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

  modem.attachData("your.apn");
  modem.activatePDP(1);

  String resp;
  Serial.println("HTTP GET...");
  if (modem.httpGet("http://example.com", resp)) {
    Serial.println("OK");
    Serial.println(resp);
  } else {
    Serial.println("HTTP failed");
  }
}

void loop() {}