#include <QuectelEC200U.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  modem.begin();

  String resp;
  if (modem.sendUSSD("*#06#", resp)) {
    Serial.println(resp);
  } else {
    Serial.println("USSD failed");
  }
}
void loop() {}
