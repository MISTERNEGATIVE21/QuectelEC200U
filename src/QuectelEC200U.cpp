/*
  QuectelEC200U_CN - Arduino library for Quectel EC200U (CN-AA)
  Author: misternegative21
  Maintainer: MisterNegative21 <misternegative21@gmail.com>
  Repository: https://github.com/MISTERNEGATIVE21/QuectelEC200U
  License: MIT (see LICENSE)

  Quectel and EC200U are trademarks of Quectel Wireless Solutions Co., Ltd.
  This library is an independent, unofficial project and is not affiliated with or endorsed by Quectel.
*/

#include "QuectelEC200U.h"

QuectelEC200U::QuectelEC200U(HardwareSerial &serial, uint32_t baud, int8_t rxPin, int8_t txPin) {
  _serial = &serial;
  _hwSerial = &serial;
  _baud = baud;
  _rxPin = rxPin;
  _txPin = txPin;
}

QuectelEC200U::QuectelEC200U(Stream &stream) {
  _serial = &stream;
  _hwSerial = nullptr;
  _baud = 0;
  _rxPin = -1;
  _txPin = -1;
}

void QuectelEC200U::begin() {
#if defined(ARDUINO_ARCH_ESP32)
  if (_hwSerial) {
    if (_rxPin >= 0 && _txPin >= 0) {
      _hwSerial->begin(_baud, SERIAL_8N1, _rxPin, _txPin);
    } else {
      _hwSerial->begin(_baud);
    }
  }
#else
  if (_hwSerial) {
    _hwSerial->begin(_baud);
  }
#endif
  delay(200);
  flushInput();
  // Try to sync and disable echo
  for (int i = 0; i < 5; i++) {
    _serial->println("AT");
    String resp = readResponse(500);
    if (resp.indexOf("OK") != -1) break;
    delay(100);
  }
  sendCommand("ATE0"); // disable echo
}

bool QuectelEC200U::sendCommand(const String &cmd, const String &expect, uint32_t timeout) {
  if (cmd.length()) _serial->println(cmd);
  String resp = readResponse(timeout);
  if (resp.indexOf("ERROR") != -1) return false;
  return expect.length() == 0 ? true : resp.indexOf(expect) != -1;
}

String QuectelEC200U::readResponse(uint32_t timeout) {
  String resp = "";
  uint32_t start = millis();
  uint32_t lastByte = start;
  while (millis() - start < timeout) {
    while (_serial->available()) {
      resp += (char)_serial->read();
      lastByte = millis();
    }
    // If we received something and input went idle >100ms, stop early
    if (resp.length() && (millis() - lastByte > 100)) break;
    delay(1);
  }
  return resp;
}

void QuectelEC200U::flushInput() {
  while (_serial->available()) (void)_serial->read();
}

// ===== helpers =====
bool QuectelEC200U::expectURC(const String &tag, uint32_t timeout) {
  String r = readResponse(timeout);
  return r.indexOf(tag) != -1;
}

// ===== Core =====
String QuectelEC200U::getIMEI() {
  _serial->println("AT+GSN");
  return readResponse(1000);
}

int QuectelEC200U::getSignalStrength() {
  _serial->println("AT+CSQ");
  String resp = readResponse(1000);
  int c1 = resp.indexOf(':');
  int c2 = resp.indexOf(',', c1 + 1);
  if (c1 == -1) return -1;
  String rssiStr = (c2 != -1) ? resp.substring(c1 + 1, c2) : resp.substring(c1 + 1);
  rssiStr.trim();
  return rssiStr.toInt();
}

bool QuectelEC200U::setAPN(const String &apn) {
  return sendCommand("AT+CGDCONT=1,\"IP\",\"" + apn + "\"");
}

// ===== Network + PDP =====
bool QuectelEC200U::waitForNetwork(uint32_t timeoutMs) {
  uint32_t start = millis();
  while (millis() - start < timeoutMs) {
    _serial->println("AT+CEREG?");
    String r = readResponse(500);
    // +CEREG: <n>,<stat> ... stat 1=registered(home),5=registered(roaming)
    if (r.indexOf("+CEREG:") != -1 && (r.indexOf(",1") != -1 || r.indexOf(",5") != -1)) return true;
    delay(1000);
  }
  return false;
}

bool QuectelEC200U::attachData(const String &apn, const String &user, const String &pass, int auth) {
  // PDP context + optional auth
  if (!sendCommand("AT+CGATT=1")) return false;
  if (!sendCommand("AT+CGDCONT=1,\"IP\",\"" + apn + "\"")) return false;
  // Optional auth for context 1
  if (!sendCommand("AT+QICSGP=1,1,\"" + apn + "\",\"" + user + "\",\"" + pass + "\"," + String(auth))) return false;
  return true;
}

bool QuectelEC200U::activatePDP(int ctxId) {
  return sendCommand("AT+QIACT=" + String(ctxId), "OK", 15000) && expectURC("+QIACT:", 15000);
}

bool QuectelEC200U::deactivatePDP(int ctxId) {
  return sendCommand("AT+QIDEACT=" + String(ctxId), "OK", 15000);
}

int QuectelEC200U::getRegistrationStatus(bool eps) {
  _serial->println(eps ? "AT+CEREG?" : "AT+CREG?");
  String r = readResponse(1000);
  int c = r.lastIndexOf(',');
  if (c == -1) return -1;
  String s = r.substring(c + 1);
  s.trim();
  return s.toInt();
}

bool QuectelEC200U::isSimReady() {
  _serial->println("AT+CPIN?");
  String r = readResponse(1000);
  return r.indexOf("READY") != -1;
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
  if (!sendCommand("AT+QHTTPURL=" + String(url.length()) + ",80", "CONNECT")) return false;
  _serial->print(url);
  if (!sendCommand("", "OK", 5000)) return false;
  if (!sendCommand("AT+QHTTPGET=80", "OK", 15000)) return false;
  _serial->println("AT+QHTTPREAD=80");
  response = readResponse(15000);
  return response.length() > 0 && response.indexOf("ERROR") == -1;
}

bool QuectelEC200U::httpPost(const String &url, const String &data, String &response) {
  if (!httpGet(url, response)) return false; // ensure context
  String cmd = "AT+QHTTPPOST=" + String(data.length()) + ",80,80";
  if (!sendCommand(cmd, "CONNECT")) return false;
  _serial->print(data);
  if (!sendCommand("", "OK", 10000)) return false;
  _serial->println("AT+QHTTPREAD=80");
  response = readResponse(10000);
  return response.indexOf("ERROR") == -1;
}

// ===== HTTPS =====
bool QuectelEC200U::httpsGet(const String &url, String &response) {
  if (!sendCommand("AT+QHTTPCFG=\"contextid\",1")) return false;
  if (!sendCommand("AT+QHTTPCFG=\"sslctxid\",1")) return false;
  if (!sendCommand("AT+QHTTPURL=" + String(url.length()) + ",80", "CONNECT")) return false;
  _serial->print(url);
  if (!sendCommand("", "OK", 5000)) return false;
  if (!sendCommand("AT+QHTTPGET=80", "OK", 20000)) return false;
  _serial->println("AT+QHTTPREAD=80");
  response = readResponse(20000);
  return response.length() > 0 && response.indexOf("ERROR") == -1;
}

bool QuectelEC200U::httpsPost(const String &url, const String &data, String &response) {
  if (!sendCommand("AT+QHTTPCFG=\"contextid\",1")) return false;
  if (!sendCommand("AT+QHTTPCFG=\"sslctxid\",1")) return false;
  if (!sendCommand("AT+QHTTPURL=" + String(url.length()) + ",80", "CONNECT")) return false;
  _serial->print(url);
  if (!sendCommand("", "OK", 5000)) return false;

  String cmd = "AT+QHTTPPOST=" + String(data.length()) + ",80,80";
  if (!sendCommand(cmd, "CONNECT")) return false;
  _serial->print(data);
  if (!sendCommand("", "OK", 10000)) return false;
  _serial->println("AT+QHTTPREAD=80");
  response = readResponse(10000);
  return response.indexOf("ERROR") == -1;
}

// ===== TCP sockets =====
int QuectelEC200U::tcpOpen(const String &host, int port, int ctxId, int socketId) {
  String cmd = "AT+QIOPEN=" + String(ctxId) + "," + String(socketId) + ",\"TCP\",\"" + host + "\"," + String(port) + ",0,1";
  if (!sendCommand(cmd, "OK", 5000)) return -1;
  if (!expectURC("+QIOPEN: " + String(socketId) + ",0", 15000)) return -1;
  return socketId;
}

bool QuectelEC200U::tcpSend(int socketId, const String &data) {
  String cmd = "AT+QISEND=" + String(socketId) + "," + String(data.length());
  if (!sendCommand(cmd, ">", 2000)) return false;
  _serial->print(data);
  return readResponse(5000).indexOf("SEND OK") != -1;
}

bool QuectelEC200U::tcpRecv(int socketId, String &out, size_t bytes, uint32_t timeout) {
  String cmd = "AT+QIRD=" + String(socketId) + "," + String(bytes);
  _serial->println(cmd);
  out = readResponse(timeout);
  return out.length() > 0 && out.indexOf("ERROR") == -1;
}

bool QuectelEC200U::tcpClose(int socketId) {
  return sendCommand("AT+QICLOSE=" + String(socketId), "OK", 5000);
}

// ===== USSD =====
bool QuectelEC200U::sendUSSD(const String &code, String &response) {
  if (!sendCommand("AT+CUSD=1,\"" + code + "\",15")) return false;
  response = readResponse(10000);
  return response.indexOf("+CUSD:") != -1;
}

// ===== NTP / Clock =====
bool QuectelEC200U::ntpSync(const String &server, int timezone) {
  if (!sendCommand("AT+QNTP=1,\"" + server + "\"", "OK", 1000)) return false;
  // wait for URC +QNTP: 0 or similar
  return expectURC("+QNTP: 0", 20000);
}

String QuectelEC200U::getClock() {
  _serial->println("AT+CCLK?");
  return readResponse(1000);
}

bool QuectelEC200U::setClock(const String &datetime) {
  return sendCommand("AT+CCLK=\"" + datetime + "\"");
}

// ===== GNSS =====
bool QuectelEC200U::startGNSS() {
  return sendCommand("AT+QGPS=1");
}

bool QuectelEC200U::stopGNSS() {
  return sendCommand("AT+QGPSEND");
}

bool QuectelEC200U::isGNSSOn() {
  _serial->println("AT+QGPS?");
  String r = readResponse(1000);
  return r.indexOf("+QGPS: 1") != -1;
}

bool QuectelEC200U::setGNSSConfig(const String &item, const String &value) {
  // Example: AT+QGPSCFG="gnssconfig",1 or AT+QGPSCFG="nmeasrc",1
  return sendCommand(String("AT+QGPSCFG=\"") + item + "\"," + value);
}

String QuectelEC200U::getNMEASentence(const String &type) {
  // Valid types typically: RMC,GGA,GSV,GSA,VTG
  _serial->println(String("AT+QGPSGNMEA=") + type);
  return readResponse(1500);
}

String QuectelEC200U::getGNSSLocation() {
  _serial->println("AT+QGPSLOC=2");
  return readResponse(2000);
}

String QuectelEC200U::getGNSSLocation(uint32_t fixWaitMs) {
  uint32_t start = millis();
  while (millis() - start < fixWaitMs) {
    _serial->println("AT+QGPSLOC=2");
    String r = readResponse(1500);
    if (r.indexOf("+QGPSLOC:") != -1 && r.indexOf("ERROR") == -1) return r;
    delay(1000);
  }
  return String();
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

// ===== Filesystem =====
bool QuectelEC200U::fsList(String &out) {
  _serial->println("AT+QFLST");
  out = readResponse(2000);
  return out.indexOf("+QFLST:") != -1;
}

bool QuectelEC200U::fsUpload(const String &path, const String &content) {
  String cmd = "AT+QFUPL=\"" + path + "\"," + String(content.length()) + ",100";
  if (!sendCommand(cmd, "CONNECT", 3000)) return false;
  _serial->print(content);
  return readResponse(5000).indexOf("OK") != -1;
}

bool QuectelEC200U::fsRead(const String &path, String &out, size_t length) {
  if (!sendCommand("AT+QFOPEN=\"" + path + "\",0")) return false;
  String resp = readResponse(1000);
  // Optional: parse file handle, but EC200U allows direct read by name as well:
  String cmd = "AT+QFREAD=\"" + path + "\"," + String(length ? length : 1024);
  _serial->println(cmd);
  out = readResponse(5000);
  (void)sendCommand("AT+QFCLOSE"); // best-effort
  return out.length() > 0 && out.indexOf("ERROR") == -1;
}

bool QuectelEC200U::fsDelete(const String &path) {
  return sendCommand("AT+QFDEL=\"" + path + "\"");
}

// ===== SSL/TLS =====
bool QuectelEC200U::sslConfigure(int ctxId, const String &caPath, bool verify) {
  if (!sendCommand("AT+QSSLCFG=\"cacert\"," + String(ctxId) + ",\"" + caPath + "\"")) return false;
  return sendCommand("AT+QSSLCFG=\"seclevel\"," + String(ctxId) + "," + String(verify ? 2 : 0));
}

// ===== PSM =====
bool QuectelEC200U::enablePSM(bool enable) {
  // Use CPSMS: enable with default requested params
  return sendCommand(String("AT+CPSMS=") + (enable ? "1" : "0"));
}

// ===== MQTT =====
bool QuectelEC200U::mqttConnect(const String &server, int port) {
  if (!sendCommand("AT+QMTOPEN=0,\"" + server + "\"," + String(port), "+QMTOPEN: 0,0", 15000)) return false;
  return sendCommand("AT+QMTCONN=0,\"ec200u\"", "+QMTCONN: 0,0", 10000);
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

// ===== Voice Call =====
bool QuectelEC200U::dial(const String &number) {
  // Dial voice call (trailing ';' selects voice)
  return sendCommand(String("ATD") + number + ";");
}

bool QuectelEC200U::hangup() {
  // Hang up call
  return sendCommand("ATH");
}

bool QuectelEC200U::answer() {
  // Answer incoming call
  return sendCommand("ATA");
}

String QuectelEC200U::getCallList() {
  _serial->println("AT+CLCC");
  return readResponse(2000);
}

bool QuectelEC200U::enableCallerId(bool enable) {
  return sendCommand(String("AT+CLIP=") + (enable ? "1" : "0"));
}

// ===== Audio (speaker/microphone) =====
bool QuectelEC200U::setSpeakerVolume(int level) {
  level = constrain(level, 0, 100);
  return sendCommand(String("AT+CLVL=") + level);
}

bool QuectelEC200U::setRingerVolume(int level) {
  level = constrain(level, 0, 100);
  return sendCommand(String("AT+CRSL=") + level);
}

bool QuectelEC200U::setMicMute(bool mute) {
  return sendCommand(String("AT+CMUT=") + (mute ? 1 : 0));
}

bool QuectelEC200U::setMicGain(int channel, int level) {
  // Channel 0: main mic, 1: headset mic, ranges per module docs
  level = constrain(level, 0, 15);
  return sendCommand(String("AT+QMIC=") + channel + "," + level);
}

bool QuectelEC200U::setSidetone(bool enable, int level) {
  level = constrain(level, 0, 15);
  return sendCommand(String("AT+QSIDET=") + (enable ? 1 : 0) + "," + level);
}

bool QuectelEC200U::setAudioChannel(int channel) {
  // 0: handset, 1: headset, 2: loudspeaker, etc. per Quectel AT manual
  return sendCommand(String("AT+QAUDCH=") + channel);
}

bool QuectelEC200U::setAudioInterface(const String &params) {
  // e.g., "1,0,0,1" per hardware routing needs
  return sendCommand(String("AT+QDAI=") + params);
}

bool QuectelEC200U::audioLoopback(bool enable) {
  return sendCommand(String("AT+QAUDLOOP=") + (enable ? 1 : 0));
}
