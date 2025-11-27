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

// --- Helper Functions ---

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
  server.send(200, "text/html", index_html);
}

void handleNotFound() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
}

void handleStatus() {
  sendCorsHeaders();
  JsonDocument doc;
  
  doc["signal"] = modem.getSignalStrength();
  doc["operator"] = modem.getOperator();
  doc["net_type"] = modem.getRegistrationStatus() == 1 || modem.getRegistrationStatus() == 5 ? "Registered" : "Searching";
  doc["sim_status"] = modem.getSIMStatus();
  doc["imei"] = modem.getIMEI();
  doc["model"] = modem.getModelIdentification();
  
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
  deserializeJson(doc, server.arg("plain"));
  String host = doc["host"];
  int port = doc["port"];
  
  // Ensure context is active
  modem.configureContext(1, 1, "internet", "", "", 0);
  modem.activatePDP(1);
  
  int socketId = modem.tcpOpen(host, port);
  currentSocketId = socketId;
  
  JsonDocument res;
  res["success"] = (socketId != -1);
  res["socketId"] = socketId;
  
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleTcpClose() {
  sendCorsHeaders();
  bool success = false;
  if (currentSocketId != -1) {
    success = modem.tcpClose(currentSocketId);
    currentSocketId = -1;
  }
  
  JsonDocument res;
  res["success"] = success;
  String response;
  serializeJson(res, response);
  server.send(200, "application/json", response);
}

void handleTcpSend() {
  sendCorsHeaders();
  if (server.method() != HTTP_POST) return server.send(405, "text/plain", "Method Not Allowed");
  
  JsonDocument doc;
  deserializeJson(doc, server.arg("plain"));
  String data = doc["data"];
  
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
  res["response"] = tcpResponse;
  
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

void setup() {
  Serial.begin(115200);
  
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
  server.on("/api/status", handleStatus);
  server.on("/api/sms/send", handleSmsSend);
  server.on("/api/sms/read", handleSmsRead);
  server.on("/api/gps/location", handleGpsLocation);
  server.on("/api/call/dial", handleCallDial);
  server.on("/api/call/hangup", handleCallHangup);
  server.on("/api/at", handleAT);
  server.on("/api/tcp/open", handleTcpOpen);
  server.on("/api/tcp/close", handleTcpClose);
  server.on("/api/tcp/send", handleTcpSend);
  server.on("/api/wifi/save", handleWifiSave);
  server.on("/api/wifi/forget", handleWifiForget);
  server.on("/api/modem/poweron", handleModemPowerOn);
  
  // Handle OPTIONS for CORS
  server.on("/api/status", HTTP_OPTIONS, handleOptions);
  server.on("/api/sms/send", HTTP_OPTIONS, handleOptions);
  server.on("/api/call/dial", HTTP_OPTIONS, handleOptions);
  server.on("/api/at", HTTP_OPTIONS, handleOptions);
  server.on("/api/tcp/open", HTTP_OPTIONS, handleOptions);
  server.on("/api/tcp/send", HTTP_OPTIONS, handleOptions);
  server.on("/api/wifi/save", HTTP_OPTIONS, handleOptions);
  server.on("/api/wifi/forget", HTTP_OPTIONS, handleOptions);
  server.on("/api/modem/poweron", HTTP_OPTIONS, handleOptions);
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Web Server started");
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
  // Add any non-blocking modem maintenance here if needed
}
