#include <QuectelEC200U.h>

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
#ifndef AT_RX_PIN
#define AT_RX_PIN 16
#endif
#ifndef AT_TX_PIN
#define AT_TX_PIN 17
#endif
QuectelEC200U modem(SerialAT, 115200, AT_RX_PIN, AT_TX_PIN);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8);
QuectelEC200U modem(SerialAT);
#endif

const char* APN     = "your.apn";
const char* MQTT_HOST = "test.mosquitto.org"; // change as needed
const int   MQTT_PORT = 8883; // TLS port

void setup() {
  Serial.begin(115200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  delay(500);
  Serial.println("MQTT SSL demo");

  modem.begin();
  modem.attachData(APN);
  modem.activatePDP(1);

  // Optional: if you uploaded a CA to UFS and configured ctx 1, enable SSL for MQTT client 0
  // Some firmware uses: AT+QMTCFG="SSL",<client>,<ctxid>
  modem.sendCommand("AT+QMTCFG=\"SSL\",0,1");

  // Open transport and connect (client id auto)
  if (!modem.mqttConnect(MQTT_HOST, MQTT_PORT)) {
    Serial.println("MQTT connect failed");
    return;
  }
  Serial.println("MQTT connected");

  // Subscribe
  if (modem.mqttSubscribe("/ec200u/demo")) {
    Serial.println("Subscribed");
  } else {
    Serial.println("Subscribe failed");
  }

  // Publish
  if (modem.mqttPublish("/ec200u/demo", "Hello over TLS")) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish failed");
  }
}

void loop() {
  // In a real app, read URCs for incoming messages and connection state
}
