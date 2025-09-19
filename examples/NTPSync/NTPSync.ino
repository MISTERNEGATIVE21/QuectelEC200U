#include <QuectelEC200U.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  modem.begin();

  if (modem.ntpSync("pool.ntp.org", 0)) {
    Serial.println("NTP OK");
    Serial.println(modem.getClock());
  } else {
    Serial.println("NTP failed");
  }
}

void loop() {}