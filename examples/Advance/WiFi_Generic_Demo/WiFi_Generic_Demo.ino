#include <QuectelEC200U.h>

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

#if defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
HardwareSerial SerialAT(1);
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
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

static void tryRead(const String &cmd, uint32_t to = 1500) {
  modem.sendCommand(cmd, "", to);
  String r = modem.readResponse(to);
  Serial.print(F("-> ")); Serial.println(cmd);
  Serial.println(r);
}

// Best-effort probe for module WiFi capability (commands vary by module firmware).
static void tryModuleWiFiScan() {
  Serial.println(F("Probing module WiFi AT commands (experimental)..."));
  // Common guesses: these may or may not exist on your EC200U variant.
  // We only print results; failures are expected if unsupported.
  tryRead("AT+QWIFI?");            // status query (may not exist)
  (void)sendOk("AT+QWIFI=1");      // enable WiFi if supported
  tryRead("AT+QWIFISCAN=10", 12000); // scan (some firmwares)
  tryRead("AT+QWSCAN", 12000);       // alternate scan command
  tryRead("AT+QWIFIAP?");          // AP info if any
  tryRead("AT+QWLAN?", 1500);      // alternate namespace
}

static void tryModuleWiFiConnect(const char* ssid, const char* pass) {
  Serial.println(F("Trying module WiFi connect (if supported by firmware)..."));
  // A few vendor-style attempts; harmless if unsupported (will print ERROR).
  (void)sendOk(String("AT+QWIFICFG=\"ssid\",\"") + ssid + "\"");
  (void)sendOk(String("AT+QWIFICFG=\"password\",\"") + pass + "\"");
  (void)sendOk("AT+QWIFI=1", "OK", 5000);  // bring up
  tryRead("AT+QWIFI?", 2000);
}

static void tryESP32WiFi(const char* ssid, const char* pass) {
#if defined(ARDUINO_ARCH_ESP32)
  Serial.println(F("ESP32 native WiFi fallback..."));
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
  (void)ssid; (void)pass;
  Serial.println(F("ESP32 WiFi fallback not available on this board."));
#endif
}

void setup() {
  Serial.begin(115200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  modem.begin();

  Serial.println();
  Serial.println(F("WiFi Generic Demo (module probe + ESP32 fallback)"));
  Serial.println(F("Note: Module WiFi may not be present on all EC200U variants."));

  // Probe module WiFi support and attempt a scan
  tryModuleWiFiScan();

  // Try to connect using module WiFi if available
  tryModuleWiFiConnect(WIFI_SSID, WIFI_PASS);

  // Always offer ESP32 WiFi path for boards that support it
  tryESP32WiFi(WIFI_SSID, WIFI_PASS);
}

void loop() {
  // Idle
}
