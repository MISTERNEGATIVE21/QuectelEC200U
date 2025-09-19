#include <QuectelEC200U_CN.h>

// NTP/Time demo: sync time and read RTC

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("NTP/time demo");

  modem.begin();

  // Optional: automatic time zone update
  modem.sendCommand("AT+CTZU=1");

  if (modem.ntpSync("pool.ntp.org", 0)) {
    Serial.println("NTP synced");
  } else {
    Serial.println("NTP sync failed");
  }

  Serial.println(modem.getClock());
}

void loop() {}
