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

// NTP/Time demo: sync time and read RTC

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("NTP/time demo");

#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif

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
