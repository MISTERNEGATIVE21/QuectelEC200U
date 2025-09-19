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

const char* APN = "your.apn";

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("PSM/eDRX demo");

#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  modem.begin();
  modem.attachData(APN);
  modem.activatePDP(1);

  // PSM: enable with network-defined settings
  modem.sendCommand("AT+CPSMS=1");
  // Example request values (may be rejected by network): TAU="00100101" Active="00000101"
  // modem.sendCommand("AT+CPSMS=1,,,\"00100101\",\"00000101\"");
  Serial.println(modem.readResponse(1000));

  // eDRX: enable for LTE-M (mode=5) with eDRX cycle index 5 (0101)
  modem.sendCommand("AT+CEDRXS=1,5,\"0101\"");
  Serial.println(modem.readResponse(1000));

  // Query settings
  modem.sendCommand("AT+CPSMS?");
  Serial.println(modem.readResponse(1000));
  modem.sendCommand("AT+CEDRXS?");
  Serial.println(modem.readResponse(1000));
}

void loop() {
  // Sleep or perform low duty tasks
}
