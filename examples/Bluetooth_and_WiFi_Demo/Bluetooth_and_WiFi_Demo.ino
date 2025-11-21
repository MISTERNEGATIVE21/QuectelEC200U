#include <QuectelEC200U.h>

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

// Set the EC200U modem power key pin
#define EC200U_PW_KEY_PIN 10

// Set the EC200U modem power status pin
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>


#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial& SerialAT = Serial1;
#else
  #include <SoftwareSerial.h>
  SoftwareSerial SerialAT(10, 11);
#endif
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);

void EC200U_powerOn() {
  pinMode(EC200U_PW_KEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);

  // Check if the modem is already on
  if (digitalRead(EC200U_STATUS_PIN) == LOW) {
    // Power on the modem
    digitalWrite(EC200U_PW_KEY_PIN, LOW);
    delay(500);
    digitalWrite(EC200U_PW_KEY_PIN, HIGH);
    delay(3000);
  }
}
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
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
#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#else
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
