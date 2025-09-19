#include <QuectelEC200U_CN.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  modem.begin();
  if (modem.sendSMS("+911234567890", "Hello from EC200U!"))
    Serial.println("SMS sent");
  else
    Serial.println("SMS failed");
}
void loop() {}
