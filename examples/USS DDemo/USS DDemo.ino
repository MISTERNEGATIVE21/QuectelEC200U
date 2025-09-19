#include <QuectelEC200U_CN.h>

// Minimal USSD example (folder name kept to satisfy existing structure)

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("USSD demo (folder name contains a space per repo). For richer demo, see examples/advance/USSD_Demo.ino");

  modem.begin();
  String resp;
  if (modem.sendUSSD("*#06#", resp)) {
    Serial.println(resp);
  } else {
    Serial.println("USSD failed");
  }
}

void loop() {}
