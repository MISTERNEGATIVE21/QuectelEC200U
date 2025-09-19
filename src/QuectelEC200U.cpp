#include "QuectelEC200U.h"

QuectelEC200U::QuectelEC200U(HardwareSerial &serial, uint32_t baud) {
  _serial = &serial;
  _baud = baud;
}

void QuectelEC200U::begin() {
  _serial->begin(_baud);
  delay(500);
  sendCommand("AT");
}

bool QuectelEC200U::sendCommand(const String &cmd, const String &expect, uint32_t timeout) {
  if (cmd.length()) _serial->println(cmd);
  String resp = readResponse(timeout);
  return resp.indexOf(expect) != -1;
}

String QuectelEC200U::readResponse(uint32_t timeout) {
  String resp = "";
  uint32_t start = millis();
  while (millis() - start < timeout) {
    while (_serial->available()) {
      resp += (char)_serial->read();
    }
  }
  return resp;
}

// ===== Core =====
String QuectelEC200U::getIMEI() {
  _serial->println("AT+GSN");
  return readResponse(1000);
}
int QuectelEC200U::getSignalStrength() {
  _serial->println("AT+CSQ");
  String resp = readResponse(1000);
  int idx = resp.indexOf(":");
  return (idx != -1) ? resp.substring(idx+1).toInt() : -1;
}
bool QuectelEC200U::setAPN(const String &apn) {
  return sendCommand("AT+CGDCONT=1,\"IP\",\"" + apn + "\"");
}

// ===== SMS =====
bool QuectelEC200U::sendSMS(const String &number, const String &text) {
  if (!sendCommand("AT+CMGF=1")) return false;
  _serial->print("AT+CMGS=\""); _serial->print(number); _serial->println("\"");
  delay(100);
  _serial->print(text);
  _serial->write(26);
  return readResponse(5000).indexOf("OK") != -1;
}
String QuectelEC200U::readSMS(int index) {
  _serial->println("AT+CMGR=" + String(index));
  return readResponse(2000);
}

// ===== HTTP =====
bool QuectelEC200U::httpGet(const String &url, String &response) {
  if (!sendCommand("AT+QHTTPCFG=\"contextid\",1")) return false;
  if (!sendCommand("AT+QHTTPURL=" + String(url.length()), "CONNECT")) return false;
  _serial->print(url);
  if (!sendCommand("", "OK", 5000)) return false;
  if (!sendCommand("AT+QHTTPGET=80", "OK", 10000)) return false;
  _serial->println("AT+QHTTPREAD=80");
  response = readResponse(10000);
  return response.length() > 0;
}
bool QuectelEC200U::httpPost(const String &url, const String &data, String &response) {
  if (!httpGet(url, response)) return false; // ensure context
  String cmd = "AT+QHTTPPOST=" + String(data.length()) + ",80,80";
  if (!sendCommand(cmd, "CONNECT")) return false;
  _serial->print(data);
  if (!sendCommand("", "OK", 10000)) return false;
  _serial->println("AT+QHTTPREAD=80");
  response = readResponse(10000);
  return true;
}

// ===== HTTPS =====
bool QuectelEC200U::httpsGet(const String &url, String &response) {
  if (!sendCommand("AT+QHTTPCFG=\"contextid\",1")) return false;
  if (!sendCommand("AT+QHTTPCFG=\"sslctxid\",1")) return false;
  if (!sendCommand("AT+QHTTPURL=" + String(url.length()), "CONNECT")) return false;
  _serial->print(url);
  if (!sendCommand("", "OK", 5000)) return false;
  if (!sendCommand("AT+QHTTPGET=80", "OK", 10000)) return false;
  _serial->println("AT+QHTTPREAD=80");
  response = readResponse(10000);
  return response.length() > 0;
}

bool QuectelEC200U::httpsPost(const String &url, const String &data, String &response) {
  if (!sendCommand("AT+QHTTPCFG=\"contextid\",1")) return false;
  if (!sendCommand("AT+QHTTPCFG=\"sslctxid\",1")) return false;
  if (!sendCommand("AT+QHTTPURL=" + String(url.length()), "CONNECT")) return false;
  _serial->print(url);
  if (!sendCommand("", "OK", 5000)) return false;

  String cmd = "AT+QHTTPPOST=" + String(data.length()) + ",80,80";
  if (!sendCommand(cmd, "CONNECT")) return false;
  _serial->print(data);
  if (!sendCommand("", "OK", 10000)) return false;
  _serial->println("AT+QHTTPREAD=80");
  response = readResponse(10000);
  return true;
}

// ===== MQTT =====
bool QuectelEC200U::mqttConnect(const String &server, int port) {
  return sendCommand("AT+QMTOPEN=0,\"" + server + "\"," + String(port), "+QMTOPEN: 0,0", 5000);
}
bool QuectelEC200U::mqttPublish(const String &topic, const String &message) {
  String cmd = "AT+QMTPUB=0,0,0,0,\"" + topic + "\"";
  if (!sendCommand(cmd, ">", 2000)) return false;
  _serial->print(message);
  _serial->write(26);
  return readResponse(5000).indexOf("OK") != -1;
}
bool QuectelEC200U::mqttSubscribe(const String &topic) {
  return sendCommand("AT+QMTSUB=0,1,\"" + topic + "\",0", "+QMTSUB: 0,1,0", 5000);
}

// ===== GNSS =====
bool QuectelEC200U::startGNSS() {
  return sendCommand("AT+QGPS=1");
}
String QuectelEC200U::getGNSSLocation() {
  _serial->println("AT+QGPSLOC=2");
  return readResponse(2000);
}

// ===== TTS =====
bool QuectelEC200U::playTTS(const String &text) {
  return sendCommand("AT+QTTS=1,\"" + text + "\"");
}

// ===== FTP =====
bool QuectelEC200U::ftpLogin(const String &server, const String &user, const String &pass) {
  if (!sendCommand("AT+QFTPCFG=\"account\",\"" + user + "\",\"" + pass + "\"")) return false;
  return sendCommand("AT+QFTPOPEN=\"" + server + "\",21");
}
bool QuectelEC200U::ftpDownload(const String &filename, String &data) {
  if (!sendCommand("AT+QFTPGET=\"" + filename + "\"")) return false;
  data = readResponse(10000);
  return data.length() > 0;
}

// ===== PSM =====
bool QuectelEC200U::enablePSM(bool enable) {
  return sendCommand("AT+QINDCFG=\"psm\"," + String(enable ? 1 : 0));
}
