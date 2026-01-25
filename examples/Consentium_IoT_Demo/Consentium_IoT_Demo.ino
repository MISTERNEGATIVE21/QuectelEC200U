#include <QuectelEC200U.h>
#include <ArduinoJson.h>
#include <pgmspace.h>

static const char consentiumiot_ca_cert[] PROGMEM = R"EOF(-----BEGIN CERTIFICATE-----
MIIDtzCCA1ygAwIBAgIRAI+LGiSVq2mrEwvoGRl4oJwwCgYIKoZIzj0EAwIwOzEL
MAkGA1UEBhMCVVMxHjAcBgNVBAoTFUdvb2dsZSBUcnVzdCBTZXJ2aWNlczEMMAoG
A1UEAxMDV0UxMB4XDTI1MTExMDIzNDIxMFoXDTI2MDIwOTAwMzk0OFowHDEaMBgG
A1UEAxMRY29uc2VudGl1bWlvdC5jb20wWTATBgcqhkjOPQIBBggqhkjOPQMBBwNC
AARi4qsxF4Kibh+cPlBF25WrkTB8gM85+aPLN4zC87BNq6Cbv0RJpqsMDT8034Lr
aOsS8MAsLQEeynDjvYUy8UUJo4ICXjCCAlowDgYDVR0PAQH/BAQDAgeAMBMGA1Ud
JQQMMAoGCCsGAQUFBwMBMAwGA1UdEwEB/wQCMAAwHQYDVR0OBBYEFLXG4ftzbPsq
um/EqZ18caniZ++vMB8GA1UdIwQYMBaAFJB3kjVnxP+ozKnme9mAeXvMk/k4MF4G
CCsGAQUFBwEBBFIwUDAnBggrBgEFBQcwAYYbaHR0cDovL28ucGtpLmdvb2cvcy93
ZTEvajRzMCUGCCsGAQUFBzAChhlodHRwOi8vaS5wa2kuZ29vZy93ZTEuY3J0MDEG
A1UdEQQqMCiCEWNvbnNlbnRpdW1pb3QuY29tghMqLmNvbnNlbnRpdW1pb3QuY29t
MBMGA1UdIAQMMAowCAYGZ4EMAQIBMDYGA1UdHwQvMC0wK6ApoCeGJWh0dHA6Ly9j
LnBraS5nb29nL3dlMS9raEU0el9nN0xHOC5jcmwwggEDBgorBgEEAdZ5AgQCBIH0
BIHxAO8AdQCWl2S/VViXrfdDh2g3CEJ36fA61fak8zZuRqQ/D8qpxgAAAZpwXPKU
AAAEAwBGMEQCIE4ItioUtrCr0IevZGUOFrIvJp6wlUWmEDm5iWmTDROWAiBq/BV4
dTZuz4VYj2uWE9oQerTYBJzD6EBEz3husjDHPQB2ABmG1Mcoqm/+ugNveCpNAZGq
zi1yMQ+uzl1wQS0lTMfUAAABmnBc8mYAAAQDAEcwRQIgZ1Zq6WplL83tfqDIOLni
3+16kvFxNnPY3KB/Bu1ug3cCIQD0Jshk/5jYwZA1k24oRP7yhM1UMp5fwTlP593N
JUxDrDAKBggqhkjOPQQDAgNJADBGAiEA3WUakYAt1OgKmik8ANqfh0Y4jBuIffS6
jgsgIVYrOxECIQCEri3dzlwRadQgtD7AzxLizE+rZbUwfBYOmIA+6kqtRw==
-----END CERTIFICATE-----
)EOF";

#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17
#define EC200U_PWRKEY_PIN 10
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial SerialAT(2);
  QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#elif defined(ARDUINO_ARCH_ZEPHYR)
  HardwareSerial& SerialAT = Serial1;
  QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
  #include <SoftwareSerial.h>
  SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
  QuectelEC200U modem(SerialAT);
#endif

// --- Consentium IoT Configuration ---
static const char SEND_KEY[] = "<YOUR-SEND-KEY>";
static const char RECEIVE_KEY[] = "<YOUR-RECEIVE-KEY>";
static const char BOARD_KEY[] = "<YOUR-BOARD-KEY>";
static const char APN[] = "your.apn";
static const char APN_USER[] = "";
static const char APN_PASS[] = "";
static const int PDP_CONTEXT_ID = 1;
static const int SSL_CTX_ID = 1;
static const unsigned long SEND_INTERVAL_MS = 60000;
static const char CERT_PATH[] = "consentium_ca.pem";
static const bool FETCH_RECENTS_AFTER_SEND = true;

struct SensorEntry {
  const char *info;
  const char *data;
};

static const SensorEntry SENSORS[] = {
  {"Temperature", "25.50"},
  {"Humidity", "60.00"},
  {"", ""},
  {"", ""},
  {"", ""}
};

static unsigned long lastSend = 0;

#if defined(ARDUINO_ARCH_ESP32)
static void powerOnModem() {
  pinMode(EC200U_PWRKEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);
  if (digitalRead(EC200U_STATUS_PIN) == LOW) {
    digitalWrite(EC200U_PWRKEY_PIN, LOW);
    delay(2000);
    digitalWrite(EC200U_PWRKEY_PIN, HIGH);
    delay(200);
  }
}
#else
static void powerOnModem() {}
#endif

static void haltForever(const __FlashStringHelper *reason) {
  Serial.println(reason);
  while (true) {
    delay(1000);
  }
}

static void requireStep(bool ok, const __FlashStringHelper *label) {
  if (ok) {
    Serial.print(F("[ OK ] "));
    Serial.println(label);
  } else {
    Serial.print(F("[FAIL] "));
    Serial.println(label);
    Serial.print(F("Reason: "));
    Serial.println(modem.getLastErrorString());
    haltForever(F("Stopping demo"));
  }
}

static String buildConsentiumUrl() {
  String url = F("https://api.consentiumiot.com/v2/updateData?sendKey=");
  url += SEND_KEY;
  url += F("&boardKey=");
  url += BOARD_KEY;
  return url;
}

static bool hasReceiveKey() {
  return RECEIVE_KEY[0] != '<' && strlen(RECEIVE_KEY) > 0;
}

static String buildConsentiumFetchUrl(bool recents = true, const String &fromIso = String(), const String &toIso = String()) {
  String url = F("https://api.consentiumiot.com/getData?");
  url += F("receiveKey=");
  url += RECEIVE_KEY;
  url += F("&boardKey=");
  url += BOARD_KEY;
  if (recents) {
    url += F("&recents=true");
  } else {
    url += F("&recents=false");
    if (fromIso.length()) {
      url += F("&from=");
      url += fromIso;
    }
    if (toIso.length()) {
      url += F("&to=");
      url += toIso;
    }
  }
  return url;
}

static void printFeedValues(JsonObject board, JsonObject feed) {
  Serial.println(F("Latest feed payload:"));
  Serial.print(F("Timestamp: "));
  Serial.println(feed["updated_at"].as<const char*>() ? feed["updated_at"].as<const char*>() : "--");
  for (int i = 1; i <= 10; ++i) {
    char infoKey[8];
    snprintf(infoKey, sizeof(infoKey), "info%d", i);
    const char *label = board[infoKey];
    if (!label || !strlen(label)) continue;
    char valueKey[8];
    snprintf(valueKey, sizeof(valueKey), "value%d", i);
    if (feed.containsKey(valueKey)) {
      Serial.print(F("  "));
      Serial.print(label);
      Serial.print(F(": "));
      Serial.println(feed[valueKey].as<String>());
    }
  }
}

static bool fetchConsentiumFeeds(bool recents = true, const String &fromIso = String(), const String &toIso = String()) {
  if (!hasReceiveKey()) {
    Serial.println(F("Receive key not configured; skipping fetch."));
    return false;
  }

  String url = buildConsentiumFetchUrl(recents, fromIso, toIso);
  Serial.print(F("Fetching data: "));
  Serial.println(url);

  String response;
  bool ok = modem.httpsGet(url, response);
  if (!ok) {
    Serial.println(F("❌ Failed to fetch data"));
    Serial.print(F("Error: "));
    Serial.println(modem.getLastErrorString());
    return false;
  }

  Serial.println(F("Server response:"));
  Serial.println(F("--------------------------------------"));
  Serial.println(response);
  Serial.println(F("--------------------------------------"));

  StaticJsonDocument<4096> doc;
  DeserializationError err = deserializeJson(doc, response);
  if (err) {
    Serial.print(F("JSON parse error: "));
    Serial.println(err.c_str());
    return true; // request succeeded even if parsing failed
  }

  JsonObject board = doc["board"].as<JsonObject>();
  JsonArray feeds = doc["feeds"].as<JsonArray>();
  if (!feeds.isNull() && feeds.size() > 0) {
    printFeedValues(board, feeds[0].as<JsonObject>());
  } else {
    Serial.println(F("No feed entries returned."));
  }
  return true;
}

static JsonObject addBoardInfo(StaticJsonDocument<512> &doc) {
  JsonObject boardInfo = doc["boardInfo"].to<JsonObject>();
  boardInfo["firmwareVersion"] = "1.0";
  boardInfo["architecture"] = "ESP32";
  boardInfo["statusOTA"] = false;

#if defined(ARDUINO_ARCH_ESP32)
  uint64_t chipId = ESP.getEfuseMac();
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           (uint8_t)(chipId >> 40), (uint8_t)(chipId >> 32),
           (uint8_t)(chipId >> 24), (uint8_t)(chipId >> 16),
           (uint8_t)(chipId >> 8), (uint8_t)chipId);
  boardInfo["deviceMAC"] = macStr;
#else
  boardInfo["deviceMAC"] = "AA:BB:CC:DD:EE:FF";
#endif

  int csq = modem.getSignalStrength();
  boardInfo["signalStrength"] = csq;
  boardInfo["timestamp"] = millis();
  return boardInfo;
}

static size_t addSensors(StaticJsonDocument<512> &doc) {
  JsonObject sensors = doc["sensors"].to<JsonObject>();
  JsonArray sensorData = sensors["sensorData"].to<JsonArray>();
  size_t count = 0;
  for (const auto &entry : SENSORS) {
    if (entry.info && strlen(entry.info) && entry.data && strlen(entry.data)) {
      JsonObject sensor = sensorData.add<JsonObject>();
      sensor["info"] = entry.info;
      sensor["data"] = entry.data;
      count++;
    }
  }
  sensors["sensorCount"] = count;
  return count;
}

static bool sendData() {
  StaticJsonDocument<512> doc;
  size_t sensorCount = addSensors(doc);
  addBoardInfo(doc);

  Serial.println();
  Serial.println(F("======================================"));
  Serial.println(F("Sending data to Consentium IoT"));
  Serial.println(F("======================================"));
  Serial.print(F("Sensors in payload: "));
  Serial.println(sensorCount);

  String url = buildConsentiumUrl();
  Serial.print(F("URL: "));
  Serial.println(url);

  String response;
  String headers[] = { String(F("Content-Type: application/json")) };
  bool ok = modem.httpsPost(url, doc, response, headers, 1);

  if (ok) {
    Serial.println(F("✓ Data sent successfully"));
    Serial.println(F("Response:"));
    Serial.println(F("--------------------------------------"));
    Serial.println(response);
    Serial.println(F("--------------------------------------"));
    if (FETCH_RECENTS_AFTER_SEND && hasReceiveKey()) {
      fetchConsentiumFeeds(true);
    }
  } else {
    Serial.println(F("❌ Failed to send data"));
    Serial.print(F("Error: "));
    Serial.println(modem.getLastErrorString());
  }
  return ok;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  Serial.println();
  Serial.println(F("======================================"));
  Serial.println(F("  Consentium IoT Demo"));
  Serial.println(F("======================================"));

#if defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(115200, SERIAL_8N1, EC200U_RX_PIN, EC200U_TX_PIN);
#elif defined(ARDUINO_ARCH_ZEPHYR)
  SerialAT.begin(115200);
#else
  SerialAT.begin(9600);
#endif

  powerOnModem();
  modem.enableDebug(Serial);

  requireStep(modem.begin(), F("Modem ready"));
  requireStep(modem.waitForNetwork(30000), F("Network registered"));
  requireStep(modem.attachData(APN, APN_USER, APN_PASS), F("Data attached"));
  requireStep(modem.activatePDP(PDP_CONTEXT_ID), F("PDP active"));
  requireStep(modem.sslUploadCert(consentiumiot_ca_cert, CERT_PATH), F("CA uploaded"));
  requireStep(modem.sslConfigure(SSL_CTX_ID, CERT_PATH), F("SSL context configured"));

  lastSend = millis() - SEND_INTERVAL_MS; // trigger immediate send
}

void loop() {
  unsigned long now = millis();
  if (now - lastSend >= SEND_INTERVAL_MS) {
    lastSend = now;
    sendData();
  }
  delay(200);
}