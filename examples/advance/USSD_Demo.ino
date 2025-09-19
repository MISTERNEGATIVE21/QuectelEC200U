#include <QuectelEC200U.h>

// USSD demo: send a USSD code and print response

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("USSD demo");

  modem.begin();

  String resp;
  if (modem.sendUSSD("*#06#", resp)) {
    Serial.println("USSD OK");
    Serial.println(resp);
  } else {
    Serial.println("USSD failed");
  }
}

void loop() {}
