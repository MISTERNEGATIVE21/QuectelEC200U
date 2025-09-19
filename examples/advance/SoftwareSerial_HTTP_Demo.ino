#include <SoftwareSerial.h>
#include <QuectelEC200U_CN.h>

// Demo: Use SoftwareSerial on boards without extra UARTs

SoftwareSerial ss(7, 8); // RX, TX (adjust pins for your board)
QuectelEC200U modem(ss);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("SoftwareSerial HTTP demo");

  ss.begin(9600); // many shields default to 9600; change as needed
  modem.begin();

  modem.attachData("your.apn");
  modem.activatePDP(1);

  String resp;
  if (modem.httpGet("http://example.com", resp)) {
    Serial.println("HTTP OK");
    Serial.println(resp);
  } else {
    Serial.println("HTTP failed");
  }
}

void loop() {}
