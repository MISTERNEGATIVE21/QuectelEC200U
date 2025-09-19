#include <QuectelEC200U_CN.h>

// Advanced MQTT over SSL: map MQTT client to SSL ctx and connect to 8883

QuectelEC200U modem(Serial1, 115200);

const char* APN     = "your.apn";
const char* MQTT_HOST = "test.mosquitto.org"; // change as needed
const int   MQTT_PORT = 8883; // TLS port

void setup() {
  Serial.begin(115200);
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
