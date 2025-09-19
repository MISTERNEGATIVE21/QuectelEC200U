#include <QuectelEC200U.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  modem.begin();

  Serial.println("Connecting to MQTT broker...");
  if (modem.mqttConnect("broker.hivemq.com", 1883)) {
    Serial.println("MQTT connected");

    modem.mqttSubscribe("test/topic");
    modem.mqttPublish("test/topic", "Hello from EC200U + Arduino");
  } else {
    Serial.println("MQTT failed");
  }
}

void loop() {
  // In real usage, you’d parse URCs for incoming messages here
}
