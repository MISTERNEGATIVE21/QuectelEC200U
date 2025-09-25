#include <QuectelEC200U.h>

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
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

#ifndef WIFI_SSID
#define WIFI_SSID "YourSSID"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "YourPassword"
#endif

static bool sendOk(const String &cmd, const String &expect = "OK", uint32_t to = 2000) {
  bool ok = modem.sendCommand(cmd, expect, to);
  Serial.print(F("-> ")); Serial.print(cmd); Serial.print(F("  => "));
  Serial.println(ok ? F("OK") : F("ERROR"));
  return ok;
}

static void bluetoothSetup() {
  Serial.println(F("Setting up Bluetooth (module AT)..."));
  (void)sendOk("AT+QBTPWR=1");
  (void)sendOk("AT+QBTNAME=\"EC200U_BT\"");
  (void)sendOk("AT+QBTVIS=1,1");
  modem.sendCommand("AT+QBTADDR?", "", 1000);
  Serial.println(modem.readResponse(1000));
  if (sendOk("AT+QBTSCAN=8", "OK", 2000)) {
    Serial.println(modem.readResponse(12000));
  }
}

static void moduleWiFiProbe() {
  Serial.println(F("Probing module WiFi (experimental)..."));
  modem.sendCommand("AT+QWIFI?", "", 1000);
  Serial.println(modem.readResponse(1000));
  (void)sendOk("AT+QWIFI=1");
  modem.sendCommand("AT+QWIFISCAN=8", "", 12000);
  Serial.println(modem.readResponse(12000));
}

static void esp32WiFiConnect(const char* ssid, const char* pass) {
#if defined(ARDUINO_ARCH_ESP32)
  Serial.println(F("ESP32 native WiFi connect..."));
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print(F("WiFi connected, IP: "));
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(F("ESP32 WiFi connect timeout."));
  }
#else
  (void)ssid; (void)pass; Serial.println(F("ESP32 WiFi not available on this board."));
#endif
}

void setup() {
  Serial.begin(115200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  modem.begin();

  Serial.println();
  Serial.println(F("Bluetooth + WiFi Demo"));

  bluetoothSetup();
  moduleWiFiProbe();
  esp32WiFiConnect(WIFI_SSID, WIFI_PASS);
}

void loop() {}
