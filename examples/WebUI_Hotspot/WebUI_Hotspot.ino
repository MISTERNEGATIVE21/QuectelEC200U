/*
  WebUI Hotspot Example for Quectel EC200U
  
  This example turns the ESP32 into a standalone Wi-Fi Hotspot with a beautiful Web UI
  to control the Quectel EC200U modem.
  Features:
  - Dashboard with Signal, Operator, and Network status
  - SMS Send/Receive
  - GPS Location
  - Voice Call Dialer
  - AT Command Terminal
  - TCP Test Tool
  
  Hardware:
  - ESP32 Board
  - Quectel EC200U Module connected via UART
  
  Dependencies:
  - ArduinoJson
  - QuectelEC200U
*/
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>
#include <ArduinoJson.h>
#include <QuectelEC200U.h>
#include "index_html.h"

// --- Configuration ---
// --- Configuration ---
const char* ap_ssid = "Quectel_Manager";
const char* ap_password = "password";

Preferences preferences;

// Modem Pins (Adjust for your board)
#define RX_PIN 18
#define TX_PIN 17
#define POWER_PIN 10 // Optional power pin

// Initialize Modem
HardwareSerial modemSerial(1);
QuectelEC200U modem(modemSerial, 115200, RX_PIN, TX_PIN);

// Web Server on port 80
WebServer server(80);
DNSServer dnsServer;

// Global variables
int currentSocketId = -1;

struct ApnProfile {
  const char* keyword;
  const char* apn;
  const char* user;
  const char* pass;
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
  {"rogers", "internet.com", "", "", 0}
};

struct ApnDetection {
  bool found;
  String apn;
  String user;
  String pass;
  int auth;
  String keyword;
};

struct ApnStored {
  String apn;
  String user;
  String pass;
  int auth;
  bool hasCustom;
};

struct ApnSelection {
  String apn;
  String user;
  String pass;
  int auth;
  String source;
  String operatorName;
  ApnDetection detected;
};

ApnStored storedApn;

bool mqttConnected = false;
String mqttServer = "";
int mqttPort = 1883;
int mqttCtxId = 1;
unsigned long mqttLastActivity = 0;
String mqttLastError = "";
String mqttLastTopic = "";

struct BatteryInfo {
  bool valid;
  int status;
  int percent;
  int millivolts;
  String raw;
};

// --- Helper Functions ---

void loadApnPreferences();
void saveApnPreferences(const String &apn, const String &user, const String &pass, int auth);
void clearApnPreferences();
ApnDetection detectApnProfile(const String &operatorName);
ApnSelection getApnSelection(const String &operatorHint = String());
bool configureContextWithApn(int ctxId, const String &apn, const String &user, const String &pass, int auth);
BatteryInfo parseBatteryInfo(const String &raw);
void appendCallEntries(const String &raw, JsonArray &entries);

int speakerVolumeLevel = 60;
int ringerVolumeLevel = 60;

void loadApnPreferences() {
  preferences.begin("cellular", true);
  storedApn.apn = preferences.getString("apn", "");
  storedApn.user = preferences.getString("user", "");
  storedApn.pass = preferences.getString("pass", "");
  storedApn.auth = preferences.getInt("auth", 0);
  preferences.end();
  storedApn.hasCustom = storedApn.apn.length() > 0;
}

void saveApnPreferences(const String &apn, const String &user, const String &pass, int auth) {
  preferences.begin("cellular", false);
  preferences.putString("apn", apn);
  preferences.putString("user", user);
  preferences.putString("pass", pass);
  preferences.putInt("auth", auth);
  preferences.end();
  storedApn.apn = apn;
  storedApn.user = user;
  storedApn.pass = pass;
  storedApn.auth = auth;
  storedApn.hasCustom = apn.length() > 0;
}

void clearApnPreferences() {
  preferences.begin("cellular", false);
  preferences.clear();
  preferences.end();
  storedApn.apn = "";
  storedApn.user = "";
  storedApn.pass = "";
  storedApn.auth = 0;
  storedApn.hasCustom = false;
}

ApnDetection detectApnProfile(const String &operatorName) {
  ApnDetection detection;
  detection.found = false;
  detection.apn = "";
  detection.user = "";
  detection.pass = "";
  detection.auth = 0;
  detection.keyword = "";

  String lower = operatorName;
  lower.toLowerCase();
  for (const auto &profile : APN_PROFILES) {
    String keyword(profile.keyword);
    keyword.toLowerCase();
    if (keyword.length() == 0) continue;
    if (lower.indexOf(keyword) != -1) {
      detection.found = true;
      detection.apn = profile.apn;
      detection.user = profile.user;
      detection.pass = profile.pass;
      detection.auth = profile.auth;
      detection.keyword = keyword;
      break;
    }
  }

  return detection;
}

ApnSelection getApnSelection(const String &operatorHint) {
  ApnSelection selection;
  String operatorName = operatorHint;
  if (operatorName.length() == 0) {
    operatorName = modem.getOperator();
  }
  selection.operatorName = operatorName;
  selection.detected = detectApnProfile(operatorName);

  if (storedApn.hasCustom) {
    selection.apn = storedApn.apn;
    selection.user = storedApn.user;
    selection.pass = storedApn.pass;
    selection.auth = storedApn.auth;
    selection.source = "custom";
  } else if (selection.detected.found) {
    selection.apn = selection.detected.apn;
    selection.user = selection.detected.user;
    selection.pass = selection.detected.pass;
    selection.auth = selection.detected.auth;
    selection.source = "auto";
  } else {
    selection.apn = "internet";
    selection.user = "";
    selection.pass = "";
    selection.auth = 0;
    selection.source = "default";
  }

  return selection;
}

bool configureContextWithApn(int ctxId, const String &apn, const String &user, const String &pass, int auth) {
  bool ctxReady = modem.configureContext(ctxId, 1, apn, user, pass, auth);
  bool pdpReady = ctxReady && modem.activatePDP(ctxId);
  return pdpReady;
}

BatteryInfo parseBatteryInfo(const String &raw) {
  BatteryInfo info;
  info.valid = false;
  info.status = -1;
  info.percent = -1;
  info.millivolts = -1;
  info.raw = raw;

  int tag = raw.indexOf(F("+CBC:"));
  if (tag == -1) {
    return info;
  }

  int colon = raw.indexOf(':', tag);
  int lineStart = (colon == -1) ? tag + 5 : colon + 1;
  int lineEnd = raw.indexOf('\n', lineStart);
  if (lineEnd == -1) lineEnd = raw.length();

  String line = raw.substring(lineStart, lineEnd);
  line.replace("\r", "");
  line.trim();

  int firstComma = line.indexOf(',');
  int secondComma = line.indexOf(',', firstComma + 1);
  if (firstComma == -1 || secondComma == -1) {
    return info;
  }

  info.status = line.substring(0, firstComma).toInt();
  info.percent = line.substring(firstComma + 1, secondComma).toInt();
  String voltageStr = line.substring(secondComma + 1);
  voltageStr.trim();
  info.millivolts = voltageStr.toInt();
  info.valid = true;
  return info;
}

void sendCorsHeaders() {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

void handleOptions() {
  sendCorsHeaders();
  server.send(204);
}

void handleRoot() {
  Serial.println(F("HTTP GET / (serving UI)"));
  // Serve large HTML directly from PROGMEM to avoid heap fragmentation
  server.send_P(200, "text/html", index_html);
}

void handleNotFound() {
  Serial.print(F("NotFound URI: ")); Serial.println(server.uri());
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

static const uint8_t CSQ_PERCENT_LOOKUP[31] = {
  0, 3, 6, 9, 12, 15, 18, 21,
  24, 28, 32, 36, 40, 44, 48, 52,
  56, 60, 64, 68, 72, 76, 80, 84,
  88, 91, 94, 96, 98, 99, 100
};

static int csqToPercent(int csq) {
  if (csq <= 0 || csq == 99) return 0;
  if (csq >= 31) return 100;
  return CSQ_PERCENT_LOOKUP[csq];
}

static String csqToText(int csq) {
  if (csq < 0 || csq == 99) return F("No signal");
  if (csq == 0) return F("< -113 dBm");
  if (csq == 1) return F("-111 dBm");
  if (csq == 31) return F("> -51 dBm");
  return String(-113 + (csq * 2)) + F(" dBm");
}

static String extractRatDetail(const String &qInfo) {
  int firstQuote = qInfo.indexOf('"');
  if (firstQuote == -1) return "";
  int secondQuote = qInfo.indexOf('"', firstQuote + 1);
  if (secondQuote == -1) return "";
  String detail = qInfo.substring(firstQuote + 1, secondQuote);
  detail.trim();
  return detail;
}

static String simplifyRatLabel(const String &detail) {
  if (detail.length() == 0) return "";
  String lower = detail;
  lower.toLowerCase();
  if (lower.indexOf("nr") != -1) return F("5G NR");
  if (lower.indexOf("lte") != -1) return F("4G LTE");
  if (lower.indexOf("wcdma") != -1 || lower.indexOf("umts") != -1 || lower.indexOf("hspa") != -1) return F("3G");
  if (lower.indexOf("cdma") != -1 || lower.indexOf("td-scdma") != -1) return F("3G CDMA");
  if (lower.indexOf("edge") != -1 || lower.indexOf("gprs") != -1 || lower.indexOf("gsm") != -1) return F("2G");
  return detail;
}

static void appendWifiEntries(const String &raw, JsonArray &entries) {
  const String tag = F("+QWIFISCAN:");
  int idx = 0;
  while ((idx = raw.indexOf(tag, idx)) != -1) {
    int end = raw.indexOf('\n', idx);
    if (end == -1) end = raw.length();
    String line = raw.substring(idx + tag.length(), end);
    line.replace("\r", "");
    line.trim();
    if (line.length() == 0) {
      idx = end;
      continue;
    }

    JsonObject entry = entries.createNestedObject();
    entry["raw"] = line;

    int firstQuote = line.indexOf('"');
    int secondQuote = line.indexOf('"', firstQuote + 1);
    if (firstQuote != -1 && secondQuote != -1 && secondQuote > firstQuote) {
      entry["ssid"] = line.substring(firstQuote + 1, secondQuote);
    }

    int thirdQuote = line.indexOf('"', secondQuote + 1);
    int fourthQuote = line.indexOf('"', thirdQuote + 1);
    if (thirdQuote != -1 && fourthQuote != -1 && fourthQuote > thirdQuote) {
      entry["bssid"] = line.substring(thirdQuote + 1, fourthQuote);
    }

    int cursor = fourthQuote != -1 ? fourthQuote + 1 : (secondQuote != -1 ? secondQuote + 1 : 0);
    while (cursor < (int)line.length() && (line[cursor] == ',' || line[cursor] == ' ')) {
      cursor++;
    }

    String remainder = line.substring(cursor);
    remainder.trim();
    int comma = remainder.indexOf(',');
    if (comma != -1) {
      entry["channel"] = remainder.substring(0, comma).toInt();
      remainder = remainder.substring(comma + 1);
      remainder.trim();
      comma = remainder.indexOf(',');
      if (comma != -1) {
        entry["rssi"] = remainder.substring(0, comma).toInt();
        remainder = remainder.substring(comma + 1);
        remainder.trim();
        if (remainder.length() > 0) {
          entry["auth"] = remainder;
        }
      } else if (remainder.length() > 0) {
        entry["rssi"] = remainder.toInt();
      }
    }

    idx = end;
  }
}

static void appendBluetoothEntries(const String &raw, JsonArray &entries) {
  const String tag = F("+QBTSCAN:");
  int idx = 0;
  while ((idx = raw.indexOf(tag, idx)) != -1) {
    int end = raw.indexOf('\n', idx);
    if (end == -1) end = raw.length();
    String line = raw.substring(idx + tag.length(), end);
    line.replace("\r", "");
    line.trim();
    if (line.length() == 0) {
      idx = end;
      continue;
    }

    JsonObject entry = entries.createNestedObject();
    entry["raw"] = line;

    int nameEnd = line.lastIndexOf('"');
    int nameStart = line.lastIndexOf('"', nameEnd - 1);
    if (nameStart != -1 && nameEnd != -1 && nameEnd > nameStart) {
      entry["name"] = line.substring(nameStart + 1, nameEnd);
    }

    int macStart = line.indexOf('"');
    int macEnd = line.indexOf('"', macStart + 1);
    if (macStart != -1 && macEnd != -1 && macEnd > macStart) {
      entry["mac"] = line.substring(macStart + 1, macEnd);
    } else {
      int colonIdx = line.indexOf(':');
      if (colonIdx > 1) {
        int start = colonIdx - 2;
        while (start > 0 && line[start - 1] != ',' && line[start - 1] != ' ') start--;
        int endIdx = colonIdx + 1;
        while (endIdx < (int)line.length() && line[endIdx] != ',' && line[endIdx] != ' ') endIdx++;
        entry["mac"] = line.substring(start, endIdx);
      }
    }

    idx = end;
  }
}

static String callStateToText(int state) {
  switch (state) {
    case 0: return F("Active");
    case 1: return F("Held");
    case 2: return F("Dialing");
    case 3: return F("Alerting");
    case 4: return F("Incoming");
    case 5: return F("Waiting");
    default: return F("Unknown");
  }
}

static String callDirectionToText(int dir) {
  return (dir == 1) ? F("Incoming") : F("Outgoing");
}

static String callModeToText(int mode) {
  switch (mode) {
    case 0: return F("Voice");
    case 1: return F("Data");
    case 2: return F("Fax");
    case 3: return F("Voice (Alt)");
    case 4: return F("Video");
    default: return F("Unknown");
  }
}

void appendCallEntries(const String &raw, JsonArray &entries) {
  const String tag = F("+CLCC:");
  int idx = 0;
  while ((idx = raw.indexOf(tag, idx)) != -1) {
    int end = raw.indexOf('\n', idx);
    if (end == -1) end = raw.length();
    String line = raw.substring(idx + tag.length(), end);
    line.replace("\r", "");
    line.trim();
    if (line.length() == 0) {
      idx = end;
      continue;
    }

    int values[5] = {-1, -1, -1, -1, -1};
    int cursor = 0;
    for (int field = 0; field < 5; field++) {
      int comma = line.indexOf(',', cursor);
      String token;
      if (comma == -1) {
        token = line.substring(cursor);
        cursor = line.length();
      } else {
        token = line.substring(cursor, comma);
        cursor = comma + 1;
      }
      token.trim();
      values[field] = token.toInt();
    }

    String number = "";
    int type = -1;
    int quoteStart = line.indexOf('"', cursor);
    if (quoteStart != -1) {
      int quoteEnd = line.indexOf('"', quoteStart + 1);
      if (quoteEnd != -1) {
        number = line.substring(quoteStart + 1, quoteEnd);
        int typeComma = line.indexOf(',', quoteEnd + 1);
        if (typeComma != -1) {
          String typeToken = line.substring(typeComma + 1);
          typeToken.trim();
          type = typeToken.toInt();
        }
      }
    }

    JsonObject entry = entries.createNestedObject();
    entry["index"] = values[0];
    entry["direction_code"] = values[1];
    entry["state_code"] = values[2];
    entry["mode_code"] = values[3];
    entry["multiparty"] = values[4] == 1;
    entry["direction"] = callDirectionToText(values[1]);
    entry["state"] = callStateToText(values[2]);
    entry["mode"] = callModeToText(values[3]);
    entry["number"] = number;
    entry["type"] = type;
    entry["raw"] = line;

    idx = end;
  }
}

void handleStatus() {
  sendCorsHeaders();
  JsonDocument doc;

  int csq = modem.getSignalStrength();
  int reg = modem.getRegistrationStatus();
  String operatorName = modem.getOperator();
  ApnSelection apnSelection = getApnSelection(operatorName);
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
  doc["network_info"] = networkInfo;
  doc["rat_detail"] = ratDetail;

  JsonObject apnJson = doc["apn"].to<JsonObject>();
  apnJson["apn"] = apnSelection.apn;
  apnJson["user"] = apnSelection.user;
  apnJson["pass"] = apnSelection.pass;
  apnJson["auth"] = apnSelection.auth;
  apnJson["source"] = apnSelection.source;
  apnJson["operator"] = apnSelection.operatorName;
  apnJson["detected"] = apnSelection.detected.found;
  apnJson["keyword"] = apnSelection.detected.keyword;

  JsonObject mqttJson = doc["mqtt"].to<JsonObject>();
  mqttJson["connected"] = mqttConnected;
  mqttJson["server"] = mqttServer;
  mqttJson["port"] = mqttPort;
  mqttJson["last_topic"] = mqttLastTopic;

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleModemInfo() {
  sendCorsHeaders();
  JsonDocument doc;
  doc["imei"] = modem.getIMEI();
  doc["manufacturer"] = modem.getManufacturerIdentification();
  doc["model"] = modem.getModelIdentification();
  doc["firmware"] = modem.getFirmwareRevision();
  doc["version"] = modem.getModuleVersion();

  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleSmsSend() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  
  const char* number = doc["number"];
  const char* text = doc["text"];
  
  bool success = modem.sendSMS(number, text);
  
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleSmsRead() {
  sendCorsHeaders();
  int index = server.arg("index").toInt();
  
  String msg = modem.readSMS(index);
  
  JsonDocument res;
  res["success"] = msg.length() > 0;
  res["message"] = msg;
  
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleGpsLocation() {
  sendCorsHeaders();
  
  // Ensure GNSS is on
  if (!modem.isGNSSOn()) {
    modem.startGNSS();
  }
  
  String loc = modem.getGNSSLocation();
  // Format: <latitude>,<longitude>,...
  // We need to parse this simple string or just return it
  // For better JSON, let's try to parse if possible, or just send raw
  
  JsonDocument res;
  if (loc.length() > 0) {
    res["success"] = true;
    int firstComma = loc.indexOf(',');
    int secondComma = loc.indexOf(',', firstComma + 1);
    if (firstComma > 0) {
        res["lat"] = loc.substring(0, firstComma);
        res["lon"] = loc.substring(firstComma + 1, secondComma > 0 ? secondComma : loc.length());
    } else {
        res["lat"] = loc;
        res["lon"] = "";
    }
  } else {
    res["success"] = false;
  }
  
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleCallDial() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  const char* number = doc["number"];
  
  bool success = modem.dial(number);
  
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleCallHangup() {
  sendCorsHeaders();
  bool success = modem.hangup();
  
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleCallAnswer() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");

  bool success = modem.answer();
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleCallStatus() {
  sendCorsHeaders();
  String raw = modem.getCallList();

  JsonDocument res;
  res["success"] = true;
  res["raw"] = raw;
  JsonArray entries = res.createNestedArray("entries");
  appendCallEntries(raw, entries);
  res["active"] = entries.size() > 0;
  res["speaker_volume"] = speakerVolumeLevel;
  res["ringer_volume"] = ringerVolumeLevel;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleCallVolume() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
  }

  String target = doc["target"] | "speaker";
  target.toLowerCase();
  bool isRinger = target == "ringer";
  int delta = doc["delta"] | 0;
  bool hasLevel = doc.containsKey("level");
  int requestedLevel = doc["level"] | (isRinger ? ringerVolumeLevel : speakerVolumeLevel);

  int *store = isRinger ? &ringerVolumeLevel : &speakerVolumeLevel;
  if (hasLevel) {
    requestedLevel = constrain(requestedLevel, 0, 100);
    *store = requestedLevel;
  } else {
    *store = constrain(*store + delta, 0, 100);
  }

  bool success = isRinger ? modem.setRingerVolume(*store) : modem.setSpeakerVolume(*store);
  JsonDocument res;
  res["success"] = success;
  res["target"] = isRinger ? "ringer" : "speaker";
  res["level"] = *store;
  if (!success) {
    res["error"] = "Volume command failed";
  }

  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleAT() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  String cmd = doc["cmd"];
  
  modem.sendATRaw(cmd);
  String resp = modem.readResponse(5000); // Wait up to 5s
  
  JsonDocument res;
  res["response"] = resp;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleTcpOpen() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
  }

  String host = doc["host"] | "";
  int port = doc["port"] | 0;
  int ctxId = doc["ctxId"] | 1;
  int socketId = doc["socketId"] | 0;
  String operatorHint = doc["operator"] | "";
  ApnSelection selection = getApnSelection(operatorHint);

  String apn = doc.containsKey("apn") ? String((const char*)doc["apn"]) : selection.apn;
  String user = doc.containsKey("user") ? String((const char*)doc["user"]) : selection.user;
  String pass = doc.containsKey("pass") ? String((const char*)doc["pass"]) : selection.pass;
  int auth = doc.containsKey("auth") ? doc["auth"].as<int>() : selection.auth;

  JsonDocument res;

  if (host.length() == 0 || port <= 0) {
    res["success"] = false;
    res["error"] = "Host/port required";
  } else if (apn.length() == 0) {
    res["success"] = false;
    res["error"] = "APN missing";
  } else {
    bool ctxReady = modem.configureContext(ctxId, 1, apn, user, pass, auth);
    bool pdpReady = ctxReady && modem.activatePDP(ctxId);
    if (!ctxReady) {
      res["success"] = false;
      res["error"] = "Context config failed";
    } else if (!pdpReady) {
      res["success"] = false;
      res["error"] = "PDP activation failed";
    } else {
      int openedId = modem.tcpOpen(host, port, ctxId, socketId);
      currentSocketId = openedId;
      res["success"] = (openedId != -1);
      res["socketId"] = openedId;
      res["ctxId"] = ctxId;
      if (openedId == -1) {
        res["error"] = "Socket open failed";
      }
    }
  }
  JsonObject apnJson = res["apn"].to<JsonObject>();
  apnJson["apn"] = apn;
  apnJson["user"] = user;
  apnJson["pass"] = pass;
  apnJson["auth"] = auth;
  apnJson["source"] = doc.containsKey("apn") ? "request" : selection.source;
  apnJson["operator"] = selection.operatorName;
  
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleTcpClose() {
  sendCorsHeaders();
  bool success = false;
  bool hadSocket = currentSocketId != -1;
  if (hadSocket) {
    success = modem.tcpClose(currentSocketId);
    currentSocketId = -1;
  }
  
  JsonDocument res;
  res["success"] = success;
  if (!success) {
    res["error"] = hadSocket ? "Close failed" : "No open socket";
  }
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleTcpSend() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
  }
  String data = doc["data"] | "";
  
  bool success = false;
  String tcpResponse = "";
  
  if (currentSocketId != -1) {
    success = modem.tcpSend(currentSocketId, data);
    if (success) {
        // Try to read response
        modem.tcpRecv(currentSocketId, tcpResponse, 1024, 3000);
    }
  }
  
  JsonDocument res;
  res["success"] = success;
  if (!success) {
    res["error"] = "No open socket or send failed";
  }
  String formatted = tcpResponse;
  formatted.replace("\r", "\n");
  res["response"] = formatted;
  res["raw"] = tcpResponse;
  
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleWifiSave() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  String ssid = doc["ssid"];
  String pass = doc["pass"];
  
  if (ssid.length() > 0) {
    preferences.begin("wifi", false);
    preferences.putString("ssid", ssid);
    preferences.putString("pass", pass);
    preferences.end();
    
    server.send(200, "application/json", "{\"success\":true, \"message\":\"Saved. Rebooting...\"}");
    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "application/json", "{\"success\":false, \"message\":\"Invalid SSID\"}");
  }
}

void handleWifiForget() {
  sendCorsHeaders();
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();
  
  server.send(200, "application/json", "{\"success\":true, \"message\":\"Forgot. Rebooting...\"}");
  delay(1000);
  ESP.restart();
}

void handleModemPowerOn() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  modem.powerOn(POWER_PIN);
  bool success = modem.begin();
  
  JsonDocument res;
  res["success"] = success;
  res["message"] = success ? "Modem Powered On" : "Initialization Failed";
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleModemPowerOff() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  bool success = modem.powerOff();
  
  JsonDocument res;
  res["success"] = success;
  res["message"] = success ? "Modem Powered Off" : "Power Off Failed";
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleEspReboot() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  server.send(200, "application/json", "{\"success\":true,\"message\":\"Rebooting ESP32...\"}");
  delay(500);
  ESP.restart();
}

void handleQuectelWifiScan() {
  sendCorsHeaders();
  String scanResult = modem.getWifiScan();

  JsonDocument res;
  bool success = scanResult.length() > 0 && scanResult.indexOf(F("ERROR")) == -1;
  String formatted = scanResult;
  formatted.replace("\r", "\n");
  res["success"] = success;
  res["raw"] = formatted;
  if (success) {
    JsonArray entries = res.createNestedArray("entries");
    appendWifiEntries(formatted, entries);
  }
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleBluetoothScan() {
  sendCorsHeaders();
  String scanResult = modem.scanBluetooth();

  JsonDocument res;
  bool success = scanResult.length() > 0 && scanResult.indexOf(F("ERROR")) == -1;
  String formatted = scanResult;
  formatted.replace("\r", "\n");
  res["success"] = success;
  res["raw"] = formatted;
  if (success) {
    JsonArray entries = res.createNestedArray("entries");
    appendBluetoothEntries(formatted, entries);
  }
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleDeviceSensors() {
  sendCorsHeaders();
  BatteryInfo battery = parseBatteryInfo(modem.getBatteryCharge());
  int adcValue = modem.readADC();

  JsonDocument res;
  JsonObject batt = res["battery"].to<JsonObject>();
  batt["valid"] = battery.valid;
  batt["percent"] = battery.percent;
  batt["status"] = battery.status;
  batt["millivolts"] = battery.millivolts;
  batt["voltage"] = battery.millivolts > 0 ? battery.millivolts / 1000.0f : 0.0f;
  batt["raw"] = battery.raw;

  JsonObject sensors = res["sensors"].to<JsonObject>();
  sensors["adc_value"] = adcValue;

  res["timestamp"] = millis();

  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handlePdpStatus() {
  sendCorsHeaders();
  int ctxId = server.hasArg("ctx") ? server.arg("ctx").toInt() : 1;
  if (ctxId <= 0) ctxId = 1;

  QuectelEC200U::PDPContext ctx = modem.getPDPContext(ctxId);
  ApnSelection selection = getApnSelection();

  JsonDocument res;
  res["ctxId"] = ctxId;
  bool active = ctx.cid == ctxId && ctx.p_addr.length() > 0 && ctx.p_addr != "0.0.0.0";
  res["active"] = active;
  res["ip"] = ctx.p_addr;
  res["apn"] = ctx.apn;
  res["type"] = ctx.pdp_type;
  res["dns_primary"] = ctx.dns_p;
  res["dns_secondary"] = ctx.dns_s;

  JsonObject sel = res["selection"].to<JsonObject>();
  sel["apn"] = selection.apn;
  sel["user"] = selection.user;
  sel["pass"] = selection.pass;
  sel["auth"] = selection.auth;
  sel["source"] = selection.source;
  sel["operator"] = selection.operatorName;
  sel["detected"] = selection.detected.found;
  sel["keyword"] = selection.detected.keyword;

  JsonObject stored = res["stored"].to<JsonObject>();
  stored["hasCustom"] = storedApn.hasCustom;
  stored["apn"] = storedApn.apn;
  stored["user"] = storedApn.user;
  stored["pass"] = storedApn.pass;
  stored["auth"] = storedApn.auth;

  res["registration"] = modem.getRegistrationStatus();

  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handlePdpActivate() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
  }

  int ctxId = doc["ctxId"] | 1;
  if (ctxId <= 0) ctxId = 1;
  String operatorHint = doc["operator"] | "";
  bool clearCustomFlag = doc["clearCustom"] | false;
  bool persist = doc["persist"] | false;

  if (clearCustomFlag) {
    clearApnPreferences();
  }

  ApnSelection selection = getApnSelection(operatorHint);
  String apn = doc.containsKey("apn") ? String((const char*)doc["apn"]) : selection.apn;
  String user = doc.containsKey("user") ? String((const char*)doc["user"]) : selection.user;
  String pass = doc.containsKey("pass") ? String((const char*)doc["pass"]) : selection.pass;
  int auth = doc.containsKey("auth") ? doc["auth"].as<int>() : selection.auth;

  apn.trim();
  if (apn.length() == 0) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"APN is required\"}");
  }

  if (persist) {
    saveApnPreferences(apn, user, pass, auth);
  }

  bool ctxReady = modem.configureContext(ctxId, 1, apn, user, pass, auth);
  bool pdpReady = ctxReady && modem.activatePDP(ctxId);

  JsonDocument res;
  res["success"] = ctxReady && pdpReady;
  res["ctx_ready"] = ctxReady;
  res["pdp_ready"] = pdpReady;
  res["ctxId"] = ctxId;
  JsonObject apnJson = res["apn"].to<JsonObject>();
  apnJson["apn"] = apn;
  apnJson["user"] = user;
  apnJson["pass"] = pass;
  apnJson["auth"] = auth;

  if (!ctxReady) {
    res["error"] = "Context configuration failed";
  } else if (!pdpReady) {
    res["error"] = "PDP activation failed";
  }

  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handlePdpDeactivate() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
  }

  int ctxId = doc["ctxId"] | 1;
  if (ctxId <= 0) ctxId = 1;
  bool success = modem.deactivatePDP(ctxId);

  JsonDocument res;
  res["success"] = success;
  res["ctxId"] = ctxId;
  if (!success) {
    res["error"] = "Deactivate failed";
  }

  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handlePdpClear() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  clearApnPreferences();
  JsonDocument res;
  res["success"] = true;
  res["message"] = "Custom APN cleared";
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleMqttStatus() {
  sendCorsHeaders();
  JsonDocument res;
  res["connected"] = mqttConnected;
  res["server"] = mqttServer;
  res["port"] = mqttPort;
  res["ctxId"] = mqttCtxId;
  res["last_topic"] = mqttLastTopic;
  res["last_activity"] = mqttLastActivity;
  res["last_error"] = mqttLastError;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

static void _updateMqttState(bool connected, const String &host, int port, int ctxId, const String &error) {
  mqttConnected = connected;
  mqttServer = connected ? host : String();
  mqttPort = connected ? port : 0;
  mqttCtxId = ctxId;
  mqttLastError = error;
  if (connected) {
    mqttLastActivity = millis();
  }
}

void handleMqttConnect() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
  }

  String host = doc["host"] | "";
  int port = doc["port"] | 1883;
  int ctxId = doc["ctxId"] | 1;
  if (ctxId <= 0) ctxId = 1;
  String operatorHint = doc["operator"] | "";

  JsonDocument res;

  if (host.length() == 0 || port <= 0) {
    res["success"] = false;
    res["error"] = "Host and port required";
  } else {
    ApnSelection selection = getApnSelection(operatorHint);

    String apn = doc.containsKey("apn") ? String((const char*)doc["apn"]) : selection.apn;
    String user = doc.containsKey("user") ? String((const char*)doc["user"]) : selection.user;
    String pass = doc.containsKey("pass") ? String((const char*)doc["pass"]) : selection.pass;
    int auth = doc.containsKey("auth") ? doc["auth"].as<int>() : selection.auth;

    apn.trim();
    if (apn.length() == 0) {
      res["success"] = false;
      res["error"] = "APN required";
    } else {
      bool ctxReady = configureContextWithApn(ctxId, apn, user, pass, auth);
      bool connected = false;
      if (ctxReady) {
        if (mqttConnected) {
          modem.mqttDisconnect();
          mqttConnected = false;
        }
        connected = modem.mqttConnect(host, port);
        _updateMqttState(connected, host, port, ctxId, connected ? "" : modem.getLastErrorString());
      }

      res["success"] = ctxReady && connected;
      res["ctx_ready"] = ctxReady;
      res["connected"] = connected;
      JsonObject apnJson = res["apn"].to<JsonObject>();
      apnJson["apn"] = apn;
      apnJson["user"] = user;
      apnJson["pass"] = pass;
      apnJson["auth"] = auth;
      apnJson["source"] = doc.containsKey("apn") ? "request" : selection.source;
      apnJson["operator"] = selection.operatorName;

      if (!ctxReady) {
        res["error"] = "Context failed";
      } else if (!connected) {
        res["error"] = mqttLastError.length() ? mqttLastError : "MQTT connect failed";
      }
    }
  }

  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleMqttDisconnect() {
  sendCorsHeaders();
  bool success = true;
  if (mqttConnected) {
    success = modem.mqttDisconnect();
  }
  _updateMqttState(false, String(), 0, mqttCtxId, success ? "" : "Disconnect failed");
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleMqttPublish() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
  }

  String topic = doc["topic"] | "";
  String payload = doc["payload"] | "";

  JsonDocument res;
  if (!mqttConnected) {
    res["success"] = false;
    res["error"] = "MQTT not connected";
  } else if (topic.length() == 0) {
    res["success"] = false;
    res["error"] = "Topic required";
  } else {
    bool success = modem.mqttPublish(topic, payload);
    res["success"] = success;
    if (!success) {
      res["error"] = modem.getLastErrorString();
      mqttLastError = res["error"].as<String>();
    } else {
      mqttLastTopic = topic;
      mqttLastActivity = millis();
      mqttLastError = "";
    }
  }

  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleMqttSubscribe() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
  }

  String topic = doc["topic"] | "";
  JsonDocument res;
  if (!mqttConnected) {
    res["success"] = false;
    res["error"] = "MQTT not connected";
  } else if (topic.length() == 0) {
    res["success"] = false;
    res["error"] = "Topic required";
  } else {
    bool success = modem.mqttSubscribe(topic);
    res["success"] = success;
    if (!success) {
      res["error"] = modem.getLastErrorString();
      mqttLastError = res["error"].as<String>();
    } else {
      mqttLastActivity = millis();
      mqttLastError = "";
    }
  }

  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handlePing() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, server.arg("plain"));
  if (err) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Invalid JSON\"}");
  }

  String host = doc["host"] | "";
  int contextId = doc["context"] | 1;
  int timeout = doc["timeout"] | 4;
  int count = doc["count"] | 4;

  if (host.length() == 0) {
    return server.send(400, "application/json", "{\"success\":false,\"error\":\"Host required\"}");
  }

  String report;
  bool success = modem.ping(host, report, contextId, timeout, count);

  JsonDocument res;
  res["success"] = success;
  res["report"] = report;
  if (!success) {
    res["error"] = "Ping failed";
  }

  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

// Advanced Features Handlers
void handleSimSwitch() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  bool success = modem.switchSimCard();
  server.send(200, "application/json", success ? "{\"success\":true}" : "{\"success\":false}");
}

void handleToggleISIM() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  bool enable = server.arg("enable") == "1";
  bool success = modem.toggleISIM(enable);
  server.send(200, "application/json", success ? "{\"success\":true}" : "{\"success\":false}");
}

void handleSetDSDS() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  bool dsds = server.arg("dsds") == "1";
  bool success = modem.setDSDSMode(dsds);
  server.send(200, "application/json", success ? "{\"success\":true}" : "{\"success\":false}");
}

void handleBlockCalls() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  bool enable = server.arg("enable") == "1";
  bool success = modem.blockIncomingCalls(enable);
  server.send(200, "application/json", success ? "{\"success\":true}" : "{\"success\":false}");
}

void handleSetUSBMode() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  bool success = modem.setUSBModeCDC();
  server.send(200, "application/json", success ? "{\"success\":true}" : "{\"success\":false}");
}

void setup() {
  Serial.begin(115200);
  loadApnPreferences();
  
  // Init Modem - Try to power on but don't block indefinitely
  Serial.println("Initializing Modem...");
  // modem.powerOn(POWER_PIN); // Moved to manual control or non-blocking check
  // Check if modem is responsive, if not, try power on
  if (!modem.begin()) {
    Serial.println("Modem not responding, attempting power on...");
    modem.powerOn(POWER_PIN);
    if (modem.begin()) {
        Serial.println("Modem initialized.");
    } else {
        Serial.println("Modem failed to initialize. Use Web UI to retry.");
    }
  } else {
    Serial.println("Modem already ready.");
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
    
    // Wait up to 10s
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
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
    
    // Start DNS Server for Captive Portal (only in AP mode)
    dnsServer.start(53, "*", IP);
  }
  
  // Setup Web Server Routes
  server.on("/", handleRoot);
  server.on("/health", [](){ server.send(200, "text/plain", "OK"); });
  server.on("/api/status", handleStatus);
  server.on("/api/modem/info", handleModemInfo);
  server.on("/api/sms/send", handleSmsSend);
  server.on("/api/sms/read", handleSmsRead);
  server.on("/api/gps/location", handleGpsLocation);
  server.on("/api/call/dial", handleCallDial);
  server.on("/api/call/hangup", handleCallHangup);
  server.on("/api/call/answer", handleCallAnswer);
  server.on("/api/call/status", handleCallStatus);
  server.on("/api/call/volume", handleCallVolume);
  server.on("/api/at", handleAT);
  server.on("/api/tcp/open", handleTcpOpen);
  server.on("/api/tcp/close", handleTcpClose);
  server.on("/api/tcp/send", handleTcpSend);
  server.on("/api/ping", handlePing);
  server.on("/api/device/sensors", handleDeviceSensors);
  server.on("/api/pdp/status", handlePdpStatus);
  server.on("/api/pdp/activate", handlePdpActivate);
  server.on("/api/pdp/deactivate", handlePdpDeactivate);
  server.on("/api/pdp/clear", handlePdpClear);
  server.on("/api/mqtt/status", handleMqttStatus);
  server.on("/api/mqtt/connect", handleMqttConnect);
  server.on("/api/mqtt/disconnect", handleMqttDisconnect);
  server.on("/api/mqtt/publish", handleMqttPublish);
  server.on("/api/mqtt/subscribe", handleMqttSubscribe);
  server.on("/api/wifi/save", handleWifiSave);
  server.on("/api/wifi/forget", handleWifiForget);
  server.on("/api/modem/poweron", handleModemPowerOn);
  server.on("/api/modem/poweroff", handleModemPowerOff);
  server.on("/api/esp/reboot", handleEspReboot);
  server.on("/api/quectel/wifi/scan", handleQuectelWifiScan);
  server.on("/api/quectel/bt/scan", handleBluetoothScan);
  
  // Advanced Features
  server.on("/api/sim/switch", handleSimSwitch);
  server.on("/api/sim/isim", handleToggleISIM);
  server.on("/api/sim/dsds", handleSetDSDS);
  server.on("/api/call/block", handleBlockCalls);
  server.on("/api/system/usb", handleSetUSBMode);
  
  // Handle OPTIONS for CORS
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/modem/info", HTTP_OPTIONS, handleOptions);
  server.on("/api/sms/send", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/dial", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/answer", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/at", HTTP_OPTIONS, handleOptions);
  server.on("/api/tcp/open", HTTP_OPTIONS, handleOptions);
  server.on("/api/tcp/close", HTTP_OPTIONS, handleOptions);
  server.on("/api/tcp/send", HTTP_OPTIONS, handleOptions);
  server.on("/api/ping", HTTP_OPTIONS, handleOptions);
  server.on("/api/device/sensors", HTTP_OPTIONS, handleOptions);
  server.on("/api/pdp/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/pdp/activate", HTTP_OPTIONS, handleOptions);
  server.on("/api/pdp/deactivate", HTTP_OPTIONS, handleOptions);
  server.on("/api/pdp/clear", HTTP_OPTIONS, handleOptions);
  server.on("/api/mqtt/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/mqtt/connect", HTTP_OPTIONS, handleOptions);
  server.on("/api/mqtt/disconnect", HTTP_OPTIONS, handleOptions);
  server.on("/api/mqtt/publish", HTTP_OPTIONS, handleOptions);
  server.on("/api/mqtt/subscribe", HTTP_OPTIONS, handleOptions);
  server.on("/api/wifi/save", HTTP_OPTIONS, handleOptions);
  server.on("/api/wifi/forget", HTTP_OPTIONS, handleOptions);
  server.on("/api/modem/poweron", HTTP_OPTIONS, handleOptions);
  server.on("/api/modem/poweroff", HTTP_OPTIONS, handleOptions);
  server.on("/api/esp/reboot", HTTP_OPTIONS, handleOptions);
  server.on("/api/quectel/wifi/scan", HTTP_OPTIONS, handleOptions);
  server.on("/api/quectel/bt/scan", HTTP_OPTIONS, handleOptions);
  server.on("/api/sim/switch", HTTP_OPTIONS, handleOptions);
  server.on("/api/sim/isim", HTTP_OPTIONS, handleOptions);
  server.on("/api/sim/dsds", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/block", HTTP_OPTIONS, handleOptions);
  server.on("/api/system/usb", HTTP_OPTIONS, handleOptions);
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Web Server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  // Add any non-blocking modem maintenance here if needed
}
