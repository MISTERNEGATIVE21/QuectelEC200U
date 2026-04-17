/*
  QuectelPhoneUI - Android 16 Style Smartphone Interface

  A premium, Material You-inspired phone interface served as a web app
  from an ESP32 WiFi hotspot. Controls Quectel EC200U modem via AT commands.

  Features:
  - Home screen with live signal, battery, operator widgets
  - Phone dialer with T9 keypad and call controls
  - SMS inbox (auto-list with hex→UTF8 decode), compose & delete
  - Web browser via cellular HTTP
  - AT Command terminal
  - Settings: WiFi, APN, modem power, GPS

  Hardware:
  - ESP32 Board (16MB flash recommended)
  - Quectel EC200U Module connected via UART

  Dependencies:
  - ArduinoJson
  - QuectelEC200U
*/

#include "phone_ui.h"
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <QuectelEC200U.h>

// Forward declarations for Arduino auto-prototype compatibility
struct BatteryInfo;
struct ApnProfile;
struct ApnStored;
struct ApnSelection;

// --- Configuration ---
const char *ap_ssid = "QuectelPhone";
const char *ap_password = "12345678";

// Modem Pins (Adjust for your board)
#define RX_PIN 18
#define TX_PIN 17
#define POWER_PIN 10

#if defined(ESP32)
#include <Preferences.h>
#include <WebServer.h>
#include <WiFi.h>
WebServer server(80);
HardwareSerial modemSerial(1);
QuectelEC200U modem(modemSerial, 115200, RX_PIN, TX_PIN);

#elif defined(ESP8266)
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
ESP8266WebServer server(80);
SoftwareSerial modemSerial(RX_PIN, TX_PIN);
QuectelEC200U modem(modemSerial);

class Preferences {
public:
  void begin(const char *name, bool readOnly) {}
  void end() {}
  void clear() {}
  String getString(const char *key, String defaultVal) { return defaultVal; }
  int getInt(const char *key, int defaultVal) { return defaultVal; }
  void putString(const char *key, String val) {}
  void putInt(const char *key, int val) {}
};

#elif defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350)
#include <WebServer.h>
#include <WiFi.h>
WebServer server(80);
SerialUART &modemSerial = Serial1;
QuectelEC200U modem(modemSerial);

class Preferences {
public:
  void begin(const char *name, bool readOnly) {}
  void end() {}
  void clear() {}
  String getString(const char *key, String defaultVal) { return defaultVal; }
  int getInt(const char *key, int defaultVal) { return defaultVal; }
  void putString(const char *key, String val) {}
  void putInt(const char *key, int val) {}
};
#endif

Preferences preferences;
DNSServer dnsServer;

// --- Global State ---
int currentSocketId = -1;
int speakerVolumeLevel = 60;
int ringerVolumeLevel = 60;

// --- APN Profiles ---
struct ApnProfile {
  const char *keyword;
  const char *apn;
  const char *user;
  const char *pass;
  int auth;
};
static const ApnProfile APN_PROFILES[] = {
    {"jio", "jionet", "", "", 0},
    {"reliance", "jionet", "", "", 0},
    {"airtel", "airtelgprs.com", "", "", 0},
    {"vodafone", "www", "", "", 0},
    {"idea", "www", "", "", 0},
    {" vi", "www", "", "", 0},
    {"bsnl", "bsnlnet", "", "", 0},
    {"docomo", "tatadocomo.com", "", "", 0},
    {"mtnl", "mtnl.net", "", "", 0},
    {"telstra", "telstra.internet", "", "", 0},
    {"t-mobile", "fast.t-mobile.com", "", "", 0},
    {"att", "phone", "", "", 0},
    {"rogers", "internet.com", "", "", 0}};

struct ApnStored {
  String apn;
  String user;
  String pass;
  int auth;
  bool hasCustom;
};
ApnStored storedApn;

// --- APN helpers ---
void loadApnPreferences() {
  preferences.begin("cellular", true);
  storedApn.apn = preferences.getString("apn", "");
  storedApn.user = preferences.getString("user", "");
  storedApn.pass = preferences.getString("pass", "");
  storedApn.auth = preferences.getInt("auth", 0);
  preferences.end();
  storedApn.hasCustom = storedApn.apn.length() > 0;
}

void saveApnPreferences(const String &apn, const String &user,
                        const String &pass, int auth) {
  preferences.begin("cellular", false);
  preferences.putString("apn", apn);
  preferences.putString("user", user);
  preferences.putString("pass", pass);
  preferences.putInt("auth", auth);
  preferences.end();
  storedApn = {apn, user, pass, auth, apn.length() > 0};
}

void clearApnPreferences() {
  preferences.begin("cellular", false);
  preferences.clear();
  preferences.end();
  storedApn = {"", "", "", 0, false};
}

struct ApnSelection {
  String apn;
  String user;
  String pass;
  int auth;
  String source;
  String operatorName;
};

ApnSelection getApnSelection(const String &operatorHint = String()) {
  ApnSelection sel;
  String operatorName =
      operatorHint.length() > 0 ? operatorHint : modem.getOperator();
  sel.operatorName = operatorName;

  if (storedApn.hasCustom) {
    sel = {storedApn.apn,  storedApn.user, storedApn.pass,
           storedApn.auth, "custom",       operatorName};
    return sel;
  }

  String lower = operatorName;
  lower.toLowerCase();
  for (const auto &p : APN_PROFILES) {
    String kw(p.keyword);
    kw.toLowerCase();
    if (kw.length() > 0 && lower.indexOf(kw) != -1) {
      sel = {p.apn, p.user, p.pass, p.auth, "auto", operatorName};
      return sel;
    }
  }
  sel = {"internet", "", "", 0, "default", operatorName};
  return sel;
}

// --- Signal Helpers ---
static const uint8_t CSQ_PERCENT_LOOKUP[31] = {
    0,  3,  6,  9,  12, 15, 18, 21, 24, 28, 32, 36, 40, 44, 48, 52,
    56, 60, 64, 68, 72, 76, 80, 84, 88, 91, 94, 96, 98, 99, 100};

static int csqToPercent(int csq) {
  if (csq <= 0 || csq == 99)
    return 0;
  if (csq >= 31)
    return 100;
  return CSQ_PERCENT_LOOKUP[csq];
}

static String csqToText(int csq) {
  if (csq < 0 || csq == 99)
    return F("No signal");
  if (csq == 0)
    return F("< -113 dBm");
  if (csq == 1)
    return F("-111 dBm");
  if (csq == 31)
    return F("> -51 dBm");
  return String(-113 + (csq * 2)) + F(" dBm");
}

// --- Battery Parser ---
struct BatteryInfo {
  bool valid;
  int status;
  int percent;
  int millivolts;
};

static BatteryInfo parseBatteryInfo(const String &raw) {
  BatteryInfo info = {false, -1, -1, -1};
  int tag = raw.indexOf(F("+CBC:"));
  if (tag == -1)
    return info;
  int colon = raw.indexOf(':', tag);
  int lineStart = (colon == -1) ? tag + 5 : colon + 1;
  int lineEnd = raw.indexOf('\n', lineStart);
  if (lineEnd == -1)
    lineEnd = raw.length();
  String line = raw.substring(lineStart, lineEnd);
  line.replace("\r", "");
  line.trim();
  int c1 = line.indexOf(',');
  int c2 = line.indexOf(',', c1 + 1);
  if (c1 == -1 || c2 == -1)
    return info;
  info.status = line.substring(0, c1).toInt();
  info.percent = line.substring(c1 + 1, c2).toInt();
  String vs = line.substring(c2 + 1);
  vs.trim();
  info.millivolts = vs.toInt();
  info.valid = true;
  return info;
}

// --- UCS2 Hex Helpers (for SMS decoding) ---
static bool isHexStr(const String &str) {
  if (str.length() == 0 || str.length() % 2 != 0)
    return false;
  for (unsigned int i = 0; i < str.length(); i++) {
    if (!isxdigit(str.charAt(i)))
      return false;
  }
  return true;
}

static String ucs2HexToUtf8(const String &hexStr) {
  String utf8Str = "";
  for (unsigned int i = 0; i < hexStr.length(); i += 4) {
    String h = hexStr.substring(i, i + 4);
    uint16_t cp = (uint16_t)strtol(h.c_str(), NULL, 16);
    if (cp <= 0x7F) {
      utf8Str += (char)cp;
    } else if (cp <= 0x7FF) {
      utf8Str += (char)(0xC0 | ((cp >> 6) & 0x1F));
      utf8Str += (char)(0x80 | (cp & 0x3F));
    } else {
      utf8Str += (char)(0xE0 | ((cp >> 12) & 0x0F));
      utf8Str += (char)(0x80 | ((cp >> 6) & 0x3F));
      utf8Str += (char)(0x80 | (cp & 0x3F));
    }
  }
  return utf8Str;
}

// --- RAT Info Helpers ---
static String extractRatDetail(const String &qInfo) {
  int fq = qInfo.indexOf('"');
  if (fq == -1)
    return "";
  int sq = qInfo.indexOf('"', fq + 1);
  if (sq == -1)
    return "";
  String d = qInfo.substring(fq + 1, sq);
  d.trim();
  return d;
}

static String simplifyRatLabel(const String &detail) {
  if (detail.length() == 0)
    return "";
  String l = detail;
  l.toLowerCase();
  if (l.indexOf("nr") != -1)
    return "5G NR";
  if (l.indexOf("lte") != -1)
    return "4G LTE";
  if (l.indexOf("wcdma") != -1 || l.indexOf("umts") != -1 ||
      l.indexOf("hspa") != -1)
    return "3G";
  if (l.indexOf("cdma") != -1)
    return "3G CDMA";
  if (l.indexOf("edge") != -1 || l.indexOf("gprs") != -1 ||
      l.indexOf("gsm") != -1)
    return "2G";
  return detail;
}

// --- Call Info Helpers ---
static String callStateText(int s) {
  switch (s) {
  case 0:
    return "Active";
  case 1:
    return "Held";
  case 2:
    return "Dialing";
  case 3:
    return "Alerting";
  case 4:
    return "Incoming";
  case 5:
    return "Waiting";
  default:
    return "Unknown";
  }
}

static void appendCallEntries(const String &raw, JsonArray &entries) {
  const String tag = F("+CLCC:");
  int idx = 0;
  while ((idx = raw.indexOf(tag, idx)) != -1) {
    int end = raw.indexOf('\n', idx);
    if (end == -1)
      end = raw.length();
    String line = raw.substring(idx + tag.length(), end);
    line.replace("\r", "");
    line.trim();
    if (line.length() == 0) {
      idx = end;
      continue;
    }
    int vals[5] = {-1, -1, -1, -1, -1};
    int cur = 0;
    for (int f = 0; f < 5; f++) {
      int c = line.indexOf(',', cur);
      String tk;
      if (c == -1) {
        tk = line.substring(cur);
        cur = line.length();
      } else {
        tk = line.substring(cur, c);
        cur = c + 1;
      }
      tk.trim();
      vals[f] = tk.toInt();
    }
    String number = "";
    int qs = line.indexOf('"', cur);
    if (qs != -1) {
      int qe = line.indexOf('"', qs + 1);
      if (qe != -1)
        number = line.substring(qs + 1, qe);
    }
    JsonObject entry = entries.add<JsonObject>();
    entry["index"] = vals[0];
    entry["direction"] = vals[1] == 1 ? "Incoming" : "Outgoing";
    entry["state"] = callStateText(vals[2]);
    entry["number"] = number;
    idx = end;
  }
}

// =====================================================================
//                         HTTP HANDLERS
// =====================================================================

void sendCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void handleOptions() {
  sendCorsHeaders();
  server.send(204);
}

void handleRoot() { server.send_P(200, "text/html", phone_ui_html); }

void handleNotFound() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

// --- Status ---
void handleStatus() {
  sendCorsHeaders();
  JsonDocument doc;
  int csq = modem.getSignalStrength();
  int reg = modem.getRegistrationStatus();
  String operatorName = modem.getOperator();
  String networkInfo = modem.getNetworkInfo();
  String ratDetail = extractRatDetail(networkInfo);
  String ratLabel = simplifyRatLabel(ratDetail);
  String registration = (reg == 1 || reg == 5) ? "Registered" : "Searching";

  doc["signal"] = csqToPercent(csq);
  doc["signal_csq"] = csq;
  doc["signal_text"] = csqToText(csq);
  doc["operator"] = operatorName;
  doc["net_type"] = ratLabel.length() ? ratLabel : registration;
  doc["registration"] = registration;
  doc["sim_status"] = modem.getSIMStatus();
  doc["imei"] = modem.getIMEI();
  doc["model"] = modem.getModelIdentification();

  BatteryInfo batt = parseBatteryInfo(modem.getBatteryCharge());
  JsonObject b = doc["battery"].to<JsonObject>();
  b["valid"] = batt.valid;
  b["percent"] = batt.percent;
  b["status"] = batt.status;
  b["millivolts"] = batt.millivolts;
  b["voltage"] = batt.millivolts > 0 ? batt.millivolts / 1000.0f : 0.0f;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// --- Modem Info ---
void handleModemInfo() {
  sendCorsHeaders();
  JsonDocument doc;
  doc["imei"] = modem.getIMEI();
  doc["manufacturer"] = modem.getManufacturerIdentification();
  doc["model"] = modem.getModelIdentification();
  doc["firmware"] = modem.getFirmwareRevision();
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// --- SMS Send ---
void handleSmsSend() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  const char *number = doc["number"];
  const char *text = doc["text"];
  bool success = modem.sendSMS(number, text);
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- SMS List ---
void handleSmsList() {
  sendCorsHeaders();
  // Ensure text mode
  modem.sendAT(F("AT+CMGF=1"));
  String raw = modem.listMessages("ALL");

  JsonDocument doc;
  doc["success"] = true;
  JsonArray messages = doc["messages"].to<JsonArray>();

  const String tag = F("+CMGL: ");
  int idx = 0;
  while ((idx = raw.indexOf(tag, idx)) != -1) {
    int endLine = raw.indexOf('\n', idx);
    if (endLine == -1)
      endLine = raw.length();
    String header = raw.substring(idx + tag.length(), endLine);
    header.replace("\r", "");
    int fc = header.indexOf(',');
    int index = header.substring(0, fc).toInt();

    // Sender = 2nd quoted string
    int q1 = header.indexOf('"');
    int q2 = header.indexOf('"', q1 + 1);
    int q3 = header.indexOf('"', q2 + 1);
    int q4 = header.indexOf('"', q3 + 1);
    String sender = "";
    if (q3 != -1 && q4 > q3)
      sender = header.substring(q3 + 1, q4);

    // Date = last quoted string
    int lq1 = header.lastIndexOf('"');
    int lq2 = header.lastIndexOf('"', lq1 - 1);
    String date = "";
    if (lq2 != -1 && lq1 > lq2)
      date = header.substring(lq2 + 1, lq1);

    // Message body
    int nextTag = raw.indexOf(tag, endLine);
    int okTag = raw.indexOf(F("\r\nOK"), endLine);
    if (okTag != -1 && (nextTag == -1 || okTag < nextTag))
      nextTag = okTag;
    else if (nextTag == -1)
      nextTag = raw.length();
    String content = raw.substring(endLine + 1, nextTag);
    content.trim();

    if (content.length() > 0 && isHexStr(content))
      content = ucs2HexToUtf8(content);
    if (sender.length() > 0 && isHexStr(sender))
      sender = ucs2HexToUtf8(sender);

    JsonObject msg = messages.add<JsonObject>();
    msg["index"] = index;
    msg["sender"] = sender;
    msg["date"] = date;
    msg["text"] = content;
    idx = nextTag;
  }

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// --- SMS Delete ---
void handleSmsDelete() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int index = doc["index"] | -1;
  bool success = (index >= 0) && modem.deleteSMS(index);
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- Call Dial ---
void handleCallDial() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  const char *number = doc["number"];
  bool success = modem.dial(number);
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- Call Hangup ---
void handleCallHangup() {
  sendCorsHeaders();
  bool success = modem.hangup();
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- Call Answer ---
void handleCallAnswer() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  bool success = modem.answer();
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- Call Status ---
void handleCallStatus() {
  sendCorsHeaders();
  String raw = modem.getCallList();
  JsonDocument res;
  res["success"] = true;
  JsonArray entries = res["entries"].to<JsonArray>();
  appendCallEntries(raw, entries);
  res["active"] = entries.size() > 0;
  res["speaker_volume"] = speakerVolumeLevel;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- Call Volume ---
void handleCallVolume() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int delta = doc["delta"] | 0;
  speakerVolumeLevel = constrain(speakerVolumeLevel + delta, 0, 100);
  bool success = modem.setSpeakerVolume(speakerVolumeLevel);
  JsonDocument res;
  res["success"] = success;
  res["level"] = speakerVolumeLevel;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- AT Terminal ---
void handleAT() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  String cmd = doc["cmd"];
  modem.sendATRaw(cmd);
  String resp = modem.readResponse(5000);
  JsonDocument res;
  res["response"] = resp;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- Ensure cellular data is ready ---
bool dataReady = false;

bool ensureCellularData() {
  if (dataReady) return true;

  ApnSelection sel = getApnSelection();
  if (sel.apn.length() == 0) return false;

  // Step 1: GPRS attach + APN config
  if (!modem.attachData(sel.apn.c_str(), sel.user.c_str(),
                        sel.pass.c_str(), sel.auth)) {
    Serial.println("attachData failed");
    return false;
  }

  // Step 2: Configure PDP context
  modem.configureContext(1, 1, sel.apn.c_str(), sel.user.c_str(),
                         sel.pass.c_str(), sel.auth);

  // Step 3: Activate PDP
  if (!modem.activatePDP(1)) {
    // May already be active, that's OK
    Serial.println("PDP activate returned false (may already be active)");
  }

  // Step 4: SSL config for HTTPS
  modem.sendAT(F("AT+QSSLCFG=\"sslversion\",1,4"));  // TLS 1.2
  modem.sendAT(F("AT+QSSLCFG=\"ciphersuite\",1,0xFFFF"));
  modem.sendAT(F("AT+QSSLCFG=\"seclevel\",1,0"));  // No cert verify

  dataReady = true;
  return true;
}

// --- Data Status ---
void handleDataStatus() {
  sendCorsHeaders();
  ApnSelection sel = getApnSelection();
  JsonDocument res;
  res["success"] = true;
  res["data_ready"] = dataReady;
  res["apn"] = sel.apn;
  res["apn_source"] = sel.source;
  res["operator"] = sel.operatorName;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- Web Browse (cellular HTTP GET) ---
void handleBrowse() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  String url = doc["url"] | "";

  JsonDocument res;
  if (url.length() == 0) {
    res["success"] = false;
    res["error"] = "URL required";
  } else {
    // Auto-prefix http:// if no protocol specified
    if (!url.startsWith("http://") && !url.startsWith("https://"))
      url = "http://" + url;

    // Ensure cellular data is active
    if (!ensureCellularData()) {
      res["success"] = false;
      res["error"] = "Could not activate cellular data. Check SIM/signal.";
      String response;
      serializeJson(res, response);
      server.send(200, "application/json", response);
      return;
    }

    ApnSelection sel = getApnSelection();
    String httpResp;
    bool useHttps = url.startsWith("https");
    bool success = useHttps ? modem.httpsGet(url.c_str(), httpResp)
                            : modem.httpGet(url.c_str(), httpResp);

    // If HTTP failed, try HTTPS variant
    if (!success && !useHttps) {
      String httpsUrl = "https" + url.substring(4);
      success = modem.httpsGet(httpsUrl.c_str(), httpResp);
      if (success) url = httpsUrl;
    }

    // If still failed, possibly data session dropped
    if (!success) {
      dataReady = false;  // Force re-activation next time
    }

    res["success"] = success;
    res["apn_used"] = sel.apn;
    res["url"] = url;
    if (success) {
      res["content"] = httpResp;
    } else {
      res["error"] = modem.getLastErrorString();
    }
  }
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- GPS ---
void handleGpsLocation() {
  sendCorsHeaders();
  if (!modem.isGNSSOn())
    modem.startGNSS();
  QuectelEC200U::GNSSData data = modem.getGNSSData(5000);
  JsonDocument res;
  if (data.valid) {
    res["success"] = true;
    res["lat"] = data.lat;
    res["lon"] = data.lon;
    res["alt"] = data.altitude;
    res["time"] = data.utc_time;
    res["sats"] = data.nsat;
  } else {
    res["success"] = false;
  }
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- PDP Activate ---
void handlePdpActivate() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  int ctxId = doc["ctxId"] | 1;
  bool persist = doc["persist"] | false;
  ApnSelection sel = getApnSelection(doc["operator"] | "");
  String apn =
      !doc["apn"].isNull() ? String((const char *)doc["apn"]) : sel.apn;
  String user =
      !doc["user"].isNull() ? String((const char *)doc["user"]) : sel.user;
  String pass =
      !doc["pass"].isNull() ? String((const char *)doc["pass"]) : sel.pass;
  int auth = !doc["auth"].isNull() ? doc["auth"].as<int>() : sel.auth;
  apn.trim();
  if (apn.length() == 0)
    return server.send(400, "application/json",
                       "{\"success\":false,\"error\":\"APN required\"}");
  if (persist)
    saveApnPreferences(apn, user, pass, auth);
  bool ctxOk = modem.configureContext(ctxId, 1, apn, user, pass, auth);
  bool pdpOk = ctxOk && modem.activatePDP(ctxId);
  JsonDocument res;
  res["success"] = ctxOk && pdpOk;
  if (!ctxOk)
    res["error"] = "Context config failed";
  else if (!pdpOk)
    res["error"] = "PDP activation failed";
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- WiFi Save ---
void handleWifiSave() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  String ssid = doc["ssid"];
  String pass = doc["pass"];
  if (ssid.length() > 0) {
    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("pass", pass);
    preferences.end();
    server.send(200, "application/json",
                "{\"success\":true,\"message\":\"Saved. Rebooting...\"}");
    delay(1000);
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350)
    rp2040.reboot();
#else
    ESP.restart();
#endif
  } else {
    server.send(400, "application/json", "{\"success\":false}");
  }
}

// --- WiFi Forget ---
void handleWifiForget() {
  sendCorsHeaders();
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();
  server.send(200, "application/json",
              "{\"success\":true,\"message\":\"Forgot. Rebooting...\"}");
  delay(1000);
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350)
  rp2040.reboot();
#else
  ESP.restart();
#endif
}

// --- Modem Power ---
void handleModemPowerOn() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  modem.powerOn(POWER_PIN);
  bool success = modem.begin();
  JsonDocument res;
  res["success"] = success;
  res["message"] = success ? "Modem On" : "Init Failed";
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleModemPowerOff() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  bool success = modem.powerOff();
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleEspReboot() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST)
    return server.send(405, "text/plain", "Method Not Allowed");
  server.send(200, "application/json", "{\"success\":true}");
  delay(500);
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350)
  rp2040.reboot();
#else
  ESP.restart();
#endif
}

// --- WiFi Scan (ESP32 native) ---
void handleWifiScan() {
  sendCorsHeaders();
  int n = WiFi.scanNetworks(false, true);
  JsonDocument res;
  res["success"] = true;
  JsonArray nets = res["networks"].to<JsonArray>();
  for (int i = 0; i < n && i < 20; i++) {
    JsonObject net = nets.add<JsonObject>();
    net["ssid"] = WiFi.SSID(i);
    net["rssi"] = WiFi.RSSI(i);
    net["enc"] = WiFi.encryptionType(i) != 0; // 0 = open
    net["ch"] = WiFi.channel(i);
  }
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// --- ESP Info ---
void handleEspInfo() {
  sendCorsHeaders();
  JsonDocument res;
  res["success"] = true;
#if defined(ESP32)
  res["chip"] = ESP.getChipModel();
  res["cores"] = ESP.getChipCores();
  res["freq"] = ESP.getCpuFreqMHz();
  res["heap_free"] = ESP.getFreeHeap();
  res["flash_size"] = ESP.getFlashChipSize();
  res["sdk"] = ESP.getSdkVersion();
#elif defined(ESP8266)
  res["chip"] = "ESP8266";
  res["heap_free"] = ESP.getFreeHeap();
  res["flash_size"] = ESP.getFlashChipRealSize();
#else
  res["chip"] = "RP2040";
  res["heap_free"] = rp2040.getFreeHeap();
#endif
  res["wifi_mode"] = WiFi.getMode() == WIFI_AP ? "AP" : "STA";
  res["ap_ssid"] = ap_ssid;
  res["ap_clients"] = WiFi.softAPgetStationNum();
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// =====================================================================
//                           SETUP & LOOP
// =====================================================================

void setup() {
  Serial.begin(115200);
#if defined(ESP8266)
  modemSerial.begin(115200);
#elif defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_RP2350)
  modemSerial.setRX(RX_PIN);
  modemSerial.setTX(TX_PIN);
  modemSerial.begin(115200);
#endif
  loadApnPreferences();

  Serial.println("Initializing Modem...");

  bool modemOk = false;
  for (int attempt = 1; attempt <= 3 && !modemOk; attempt++) {
    Serial.print("Attempt ");
    Serial.print(attempt);
    Serial.println("/3...");

    if (attempt > 1) {
      Serial.println("Power-cycling modem...");
      modem.powerOn(POWER_PIN);
      // EC200U needs 5-12s to fully boot after power-on
      int warmup = (attempt == 2) ? 8000 : 12000;
      Serial.print("Waiting ");
      Serial.print(warmup / 1000);
      Serial.println("s for modem boot...");
      delay(warmup);
    }

    modemOk = modem.begin(attempt > 1);  // force reinit on retries

    if (!modemOk && attempt == 1) {
      // First attempt without power-on failed, try power-on
      Serial.println("Modem not responding, power cycling...");
      modem.powerOn(POWER_PIN);
      Serial.println("Waiting 5s for boot...");
      delay(5000);
      modemOk = modem.begin(true);
    }
  }

  if (modemOk) {
    Serial.println("Modem ready!");
    String sim = modem.getSIMStatus();
    Serial.print("SIM: ");
    Serial.println(sim);

    // Wait for network registration (up to 30 seconds)
    Serial.println("Waiting for network registration...");
    for (int i = 0; i < 15; i++) {
      int reg = modem.getRegistrationStatus();
      if (reg == 1 || reg == 5) {
        Serial.print("Registered to: ");
        Serial.println(modem.getOperator());
        break;
      }
      Serial.print(".");
      delay(2000);
    }
    Serial.println();

    // Auto-activate data with detected APN
    ApnSelection sel = getApnSelection();
    if (sel.apn.length() > 0) {
      Serial.print("Auto-attaching with APN: ");
      Serial.print(sel.apn);
      Serial.print(" (");
      Serial.print(sel.source);
      Serial.println(")");
      if (modem.attachData(sel.apn.c_str(), sel.user.c_str(),
                           sel.pass.c_str(), sel.auth)) {
        modem.configureContext(1, 1, sel.apn.c_str(), sel.user.c_str(),
                               sel.pass.c_str(), sel.auth);
        if (modem.activatePDP(1)) {
          dataReady = true;
          Serial.println("Cellular data ACTIVE!");
        } else {
          Serial.println("PDP activation returned false (may need manual)");
        }
      } else {
        Serial.println("Data attach failed");
      }
    } else if (storedApn.hasCustom) {
      Serial.println("Using stored APN...");
      modem.attachData(storedApn.apn, storedApn.user, storedApn.pass,
                       storedApn.auth);
    }

    // Enable SMS text mode
    modem.sendAT(F("AT+CMGF=1"));
  } else {
    Serial.println("Modem FAILED after 3 attempts. Use Settings > Modem to retry.");
  }

  // WiFi Setup
  preferences.begin("wifi", true);
  String ssid = preferences.getString("ssid", "");
  String pass = preferences.getString("pass", "");
  preferences.end();

  bool connected = false;
  if (ssid.length() > 0) {
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    for (int i = 0; i < 20; i++) {
      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        break;
      }
      delay(500);
      Serial.print(".");
    }
    Serial.println();
  }

  if (connected) {
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Starting WiFi Hotspot...");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
    dnsServer.start(53, "*", WiFi.softAPIP());
  }

  // Routes
  server.on("/", handleRoot);
  server.on("/api/status", handleStatus);
  server.on("/api/modem/info", handleModemInfo);
  server.on("/api/sms/send", handleSmsSend);
  server.on("/api/sms/list", handleSmsList);
  server.on("/api/sms/delete", handleSmsDelete);
  server.on("/api/call/dial", handleCallDial);
  server.on("/api/call/hangup", handleCallHangup);
  server.on("/api/call/answer", handleCallAnswer);
  server.on("/api/call/status", handleCallStatus);
  server.on("/api/call/volume", handleCallVolume);
  server.on("/api/at", handleAT);
  server.on("/api/browse", handleBrowse);
  server.on("/api/gps/location", handleGpsLocation);
  server.on("/api/pdp/activate", handlePdpActivate);
  server.on("/api/wifi/save", handleWifiSave);
  server.on("/api/wifi/forget", handleWifiForget);
  server.on("/api/modem/poweron", handleModemPowerOn);
  server.on("/api/modem/poweroff", handleModemPowerOff);
  server.on("/api/esp/reboot", handleEspReboot);
  server.on("/api/wifi/scan", handleWifiScan);
  server.on("/api/esp/info", handleEspInfo);
  server.on("/api/data/status", handleDataStatus);

  // CORS OPTIONS
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/modem/info", HTTP_OPTIONS, handleOptions);
  server.on("/api/sms/send", HTTP_OPTIONS, handleOptions);
  server.on("/api/sms/list", HTTP_OPTIONS, handleOptions);
  server.on("/api/sms/delete", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/dial", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/hangup", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/answer", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/volume", HTTP_OPTIONS, handleOptions);
  server.on("/api/at", HTTP_OPTIONS, handleOptions);
  server.on("/api/browse", HTTP_OPTIONS, handleOptions);
  server.on("/api/gps/location", HTTP_OPTIONS, handleOptions);
  server.on("/api/pdp/activate", HTTP_OPTIONS, handleOptions);
  server.on("/api/wifi/save", HTTP_OPTIONS, handleOptions);
  server.on("/api/wifi/forget", HTTP_OPTIONS, handleOptions);
  server.on("/api/modem/poweron", HTTP_OPTIONS, handleOptions);
  server.on("/api/modem/poweroff", HTTP_OPTIONS, handleOptions);
  server.on("/api/esp/reboot", HTTP_OPTIONS, handleOptions);

  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Phone UI server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
