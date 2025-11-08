#include <QuectelEC200U.h>

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8);
QuectelEC200U modem(SerialAT);
#endif

static bool sendOk(const String &cmd, const String &expect = "OK", uint32_t to = 2000) {
  return modem.sendCommand(cmd, expect, to);
}

void setup() {
  Serial.begin(115200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  modem.begin();

  Serial.println("Bluetooth demo (experimental)");
  (void)sendOk("AT+QBTPWR=1");
  (void)sendOk("AT+QBTNAME=\"EC200U_BT\"");
  (void)sendOk("AT+QBTVIS=1,1");

  modem.sendCommand("AT+QBTADDR?", "", 1000);
  Serial.println(modem.readResponse(1000));

  if (sendOk("AT+QBTSCAN=10", "OK", 2000)) {
    String res = modem.readResponse(12000);
    Serial.println(res);
  }
}

void loop() {}