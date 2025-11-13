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
  _debugSerial = nullptr;
  _baud = baud;
  _rxPin = rxPin;
  _txPin = txPin;
  _state = MODEM_UNINITIALIZED;
  _initialized = false;
  _echoDisabled = false;
  _simChecked = false;
  _networkRegistered = false;
  _historyCount = 0;
  _historyIndex = 0;
}

QuectelEC200U::QuectelEC200U(Stream &stream) {
  _serial = &stream;
  _hwSerial = nullptr;
  _debugSerial = nullptr;
  _baud = 0;
  _rxPin = -1;
  _txPin = -1;
  _state = MODEM_UNINITIALIZED;
  _initialized = false;
  _echoDisabled = false;
  _simChecked = false;
  _networkRegistered = false;
  _historyCount = 0;
  _historyIndex = 0;
}

void QuectelEC200U::enableDebug(Stream &debugStream) {
  _debugSerial = &debugStream;
}

void QuectelEC200U::logDebug(const String &msg) {
  if (_debugSerial) {
    _debugSerial->print(F("[DEBUG] "));
    _debugSerial->println(msg);
  }
}

void QuectelEC200U::logError(const String &msg) {
  if (_debugSerial) {
    _debugSerial->print(F("[ERROR] "));
    _debugSerial->println(msg);
  }
}

bool QuectelEC200U::begin(bool forceReinit) {
  // Skip initialization if already done and not forced
  if (_initialized && !forceReinit) {
    logDebug(F("Modem already initialized"));
    return true;
  }

  _state = MODEM_INITIALIZING;
  
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

  delay(1000);
  flushInput();

  if (!initializeModem()) {
    _state = MODEM_ERROR;
    logError(F("Modem initialization failed"));
    return false;
  }

  _initialized = true;
  _state = MODEM_READY;
  logDebug(F("Modem initialized successfully"));
  return true;
}

bool QuectelEC200U::initializeModem() {
  logDebug(F("Testing modem responsiveness..."));

  // Test modem responsiveness
  for (int i = 0; i < 5; i++) {
    if (sendAT(F("AT"), F("OK"), 500)) {
      logDebug(F("Modem is responsive"));
      break;
    }
    if (i == 4) {
      logError(F("Modem not responding"));
      return false;
    }
    delay(500);
  }

  // Disable echo (only once)
  if (!_echoDisabled) {
    if (sendAT(F("ATE0"), F("OK"), 1000)) {
      _echoDisabled = true;
      logDebug(F("Echo disabled"));
    }
  }

  // Enable verbose errors
  sendAT(F("AT+CMEE=2"), F("OK"), 1000);

  // Check SIM (only once per session)
  if (!_simChecked) {
    for (int i = 0; i < 3; i++) {
      if (isSimReady()) {
        _simChecked = true;
        logDebug(F("SIM card is ready"));
        break;
      }
      if (i == 2) {
        logError(F("SIM card not ready"));
        return false;
      }
      delay(1000);
    }
  }

  updateNetworkStatus();
  return true;
}

void QuectelEC200U::updateNetworkStatus() {
  int status = getRegistrationStatus();
  _networkRegistered = (status == 1 || status == 5);
  if (_networkRegistered) {
    _state = MODEM_NETWORK_CONNECTED;
  }
}

// Inspired by simple AT command approach - clean and efficient
bool QuectelEC200U::sendAT(const String &cmd, const String &expect, uint32_t timeout) {
  if (_debugSerial) {
    _debugSerial->print(F("CMD: "));
    _debugSerial->println(cmd);
  }
  
  _serial->println(cmd);
  uint32_t start = millis();
  String response = "";
  
  while (millis() - start < timeout) {
    while (_serial->available()) {
      char c = (char)_serial->read();
      response += c;
    }
    if (response.indexOf(expect) != -1) {
      if (_debugSerial) {
        _debugSerial->print(F("RESP: "));
        _debugSerial->println(response);
      }
      return true;
    }
  }
  
  if (_debugSerial) {
    _debugSerial->print(F("RESP: "));
    _debugSerial->println(response);
  }
  return false;
}



String QuectelEC200U::readResponse(uint32_t timeout) {
  String resp = "";
  uint32_t start = millis();
  
  while (millis() - start < timeout) {
    while (_serial->available()) {
      char c = (char)_serial->read();
      resp += c;
      
      if (_debugSerial) {
        _debugSerial->print(c);
      }
    }
    
    // Exit early if we have a complete response
    if (resp.endsWith(F("\r\nOK\r\n")) || 
        resp.endsWith(F("\r\nERROR\r\n")) || 
        resp.endsWith(F("\r\n> ")) || 
        resp.indexOf(F("+CME ERROR:")) != -1) {
      break;
    }
    
    // Small delay to allow buffer to fill
    if (!_serial->available()) {
      delay(10);
    }
  }
  
  return resp;
}

bool QuectelEC200U::waitForResponse(const String &expect, uint32_t timeout) {
  String resp = readResponse(timeout);
  return resp.indexOf(expect) != -1;
}

void QuectelEC200U::flushInput() {
  while (_serial->available()) (void)_serial->read();
}

bool QuectelEC200U::expectURC(const String &tag, uint32_t timeout) {
  String r = readResponse(timeout);
  return r.indexOf(tag) != -1;
}

// Command history implementation
void QuectelEC200U::addToHistory(const String &cmd) {
  if (cmd.length() == 0) return;
  
  // Avoid duplicates
  if (_historyCount > 0 && _cmdHistory[_historyCount - 1] == cmd) {
    return;
  }
  
  // Shift history if full
  if (_historyCount >= MAX_HISTORY) {
    for (int i = 0; i < MAX_HISTORY - 1; i++) {
      _cmdHistory[i] = _cmdHistory[i + 1];
    }
    _historyCount = MAX_HISTORY - 1;
  }
  
  _cmdHistory[_historyCount++] = cmd;
  _historyIndex = _historyCount;
}

String QuectelEC200U::getFromHistory(int index) {
  if (index >= 0 && index < _historyCount) {
    return _cmdHistory[index];
  }
  return "";
}

String QuectelEC200U::getPreviousCommand() {
  if (_historyCount == 0) return "";
  
  if (_historyIndex > 0) {
    _historyIndex--;
  }
  
  return _cmdHistory[_historyIndex];
}

String QuectelEC200U::getNextCommand() {
  if (_historyCount == 0) return "";
  
  if (_historyIndex < _historyCount - 1) {
    _historyIndex++;
    return _cmdHistory[_historyIndex];
  }
  
  _historyIndex = _historyCount;
  return "";
}

void QuectelEC200U::clearHistory() {
  _historyCount = 0;
  _historyIndex = 0;
}

// Utility helper functions
String QuectelEC200U::extractQuotedString(const String &response, const String &tag) {
  int tagIdx = response.indexOf(tag);
  if (tagIdx == -1) return "";
  
  int start = response.indexOf('"', tagIdx);
  if (start == -1) return "";
  
  int end = response.indexOf('"', start + 1);
  if (end == -1) return "";
  
  return response.substring(start + 1, end);
}

int QuectelEC200U::extractInteger(const String &response, const String &tag) {
  int tagIdx = response.indexOf(tag);
  if (tagIdx == -1) return -1;
  
  int start = tagIdx + tag.length();
  while (start < response.length() && !isDigit(response.charAt(start)) && response.charAt(start) != '-') {
    start++;
  }
  
  if (start >= response.length()) return -1;
  
  String numStr = "";
  while (start < response.length() && (isDigit(response.charAt(start)) || response.charAt(start) == '-')) {
    numStr += response.charAt(start++);
  }
  
  return numStr.toInt();
}

// Modem info functions with better formatting
String QuectelEC200U::getModemInfo() {
  String info = "";
  info += F("=== Modem Information ===\n");
  
  _serial->println(F("ATI"));
  String model = readResponse(1000);
  int crIdx = model.indexOf('\r');
  if (crIdx > 0) {
    model = model.substring(0, crIdx);
    info += F("Model: ");
    info += model;
    info += F("\n");
  }
  
  String imei = getIMEI();
  if (imei.length() > 0) {
    info += F("IMEI: ");
    info += imei;
    info += F("\n");
  }
  
  int signal = getSignalStrength();
  if (signal >= 0) {
    info += F("Signal: ");
    info += String(signal);
    info += F(" (");
    if (signal == 0) info += F("< -113 dBm");
    else if (signal == 1) info += F("-111 dBm");
    else if (signal >= 2 && signal <= 30) {
      info += String(-109 + (signal - 2) * 2);
      info += F(" dBm");
    }
    else if (signal == 31) info += F("> -51 dBm");
    else info += F("Unknown");
    info += F(")\n");
  }
  
  String oper = getOperator();
  if (oper.length() > 0) {
    info += F("Operator: ");
    info += oper;
    info += F("\n");
  }
  
  int regStatus = getRegistrationStatus();
  info += F("Registration: ");
  switch (regStatus) {
    case 0: info += F("Not registered\n"); break;
    case 1: info += F("Registered (home)\n"); break;
    case 2: info += F("Searching...\n"); break;
    case 3: info += F("Registration denied\n"); break;
    case 5: info += F("Registered (roaming)\n"); break;
    default: info += F("Unknown\n"); break;
  }
  
  info += F("========================");
  return info;
}

String QuectelEC200U::getOperator() {
  _serial->println(F("AT+COPS?"));
  String resp = readResponse(1000);
  return extractQuotedString(resp, F("+COPS:"));
}

bool QuectelEC200U::factoryReset() {
  logDebug(F("Performing factory reset..."));
  bool result = sendAT(F("AT&F"), F("OK"), 5000);
  if (result) {
    _initialized = false;
    _echoDisabled = false;
    _simChecked = false;
    _networkRegistered = false;
    _state = MODEM_UNINITIALIZED;
  }
  return result;
}

bool QuectelEC200U::powerOff() {
  logDebug(F("Powering off modem..."));
  return sendAT(F("AT+QPOWD=1"), F("OK"), 5000);
}

bool QuectelEC200U::reboot() {
  logDebug(F("Rebooting modem..."));
  bool result = sendAT(F("AT+CFUN=1,1"), F("OK"), 5000);
  if (result) {
    _initialized = false;
    _echoDisabled = false;
    _simChecked = false;
    _networkRegistered = false;
    _state = MODEM_UNINITIALIZED;
    delay(5000); // Wait for reboot
  }
  return result;
}

// SMS utilities
int QuectelEC200U::getSMSCount() {
  _serial->println(F("AT+CPMS?"));
  String resp = readResponse(1000);
  
  int start = resp.indexOf(":") + 1;
  int end = resp.indexOf(",", start);
  
  if (start > 0 && end > start) {
    String countStr = resp.substring(start, end);
    countStr.trim();
    return countStr.toInt();
  }
  return -1;
}

bool QuectelEC200U::deleteSMS(int index) {
  return sendAT("AT+CMGD=" + String(index), "OK");
}

// FTP utilities
bool QuectelEC200U::ftpLogout() {
  return sendAT("AT+QFTPCLOSE", "OK", 10000);
}

// Filesystem utilities
bool QuectelEC200U::fsExists(const String &path) {
  _serial->println("AT+QFLST=\"" + path + "\"");
  String resp = readResponse(1000);
  return resp.indexOf(F("+QFLST:")) != -1;
}

// MQTT utilities
bool QuectelEC200U::mqttDisconnect() {
  return sendAT("AT+QMTDISC=0", "OK", 5000);
}

// ===== Core =====
String QuectelEC200U::getIMEI() {
  _serial->println(F("AT+GSN"));
  String resp = readResponse(1000);
  // The response is typically in the format:
  // <IMEI>
  //
  // OK
  // We need to extract the first line.
  int first_line_end = resp.indexOf('\r');
  if (first_line_end != -1) {
    String imei = resp.substring(0, first_line_end);
    imei.trim();
    // check if the imei is a valid number
    for (int i = 0; i < imei.length(); i++) {
      if (!isDigit(imei.charAt(i))) {
        return ""; // Not a valid IMEI
      }
    }
    return imei;
  }
  return "";
}

int QuectelEC200U::getSignalStrength() {
  _serial->println(F("AT+CSQ"));
  String resp = readResponse(1000);
  // Response is typically: +CSQ: <rssi>,<ber>
  int csq_index = resp.indexOf(F("+CSQ:"));
  if (csq_index == -1) return -1;

  int c1 = resp.indexOf(':', csq_index);
  if (c1 == -1) return -1;
  int c2 = resp.indexOf(',', c1 + 1);
  if (c2 == -1) return -1;
  
  String rssiStr = resp.substring(c1 + 1, c2);
  rssiStr.trim();
  return rssiStr.toInt();
}

bool QuectelEC200U::setAPN(const String &apn) {
  // First check if PDP contexts are active and deactivate them
  flushInput();
  _serial->println(F("AT+QIACT?"));
  String actResp = readResponse(2000);
  
  // If any context is active, deactivate all
  if (actResp.indexOf(F("+QIACT:")) != -1) {
    logDebug(F("PDP contexts are active, deactivating..."));
    sendAT(F("AT+QIDEACT=1"), F("OK"), 40000);
    delay(2000);
  }
  
  // Now set the APN
  flushInput();
  _serial->print(F("AT+CGDCONT=1,\"IP\",\""));
  _serial->print(apn);
  _serial->print(F("\"\r\n"));
  _serial->flush();
  
  String resp = readResponse(2000);
  
  // Check if successful or if it's already set
  if (resp.indexOf(F("OK")) != -1) {
    logDebug(F("APN set successfully"));
    return true;
  }
  
  // If operation not allowed, the APN might already be set correctly
  if (resp.indexOf(F("+CME ERROR: Operation not allowed")) != -1) {
    logDebug(F("APN operation not allowed - checking if already configured..."));
    
    // Query current APN settings
    flushInput();
    _serial->println(F("AT+CGDCONT?"));
    String queryResp = readResponse(2000);
    
    // If our APN is already set, that's fine
    if (queryResp.indexOf(apn) != -1) {
      logDebug(F("APN already configured correctly"));
      return true;
    }
    
    logError(F("APN configuration mismatch"));
    return false;
  }
  
  return false;
}

// ===== Network + PDP =====
bool QuectelEC200U::waitForNetwork(uint32_t timeoutMs) {
  uint32_t start = millis();
  while (millis() - start < timeoutMs) {
    int status = getRegistrationStatus();
    if (status == 1 || status == 5) {
      logDebug(F("Network registered"));
      return true;
    }
    delay(2000);
  }
  logError(F("Network registration timeout"));
  return false;
}

bool QuectelEC200U::attachData(const String &apn, const String &user, const String &pass, int auth) {
  logDebug(F("Attaching to data network..."));
  
  // Check current GPRS attach status
  flushInput();
  _serial->println(F("AT+CGATT?"));
  String attachResp = readResponse(2000);
  
  // If not attached, attach now
  if (attachResp.indexOf(F("+CGATT: 0")) != -1) {
    logDebug(F("GPRS not attached, attaching..."));
    if (!sendAT(F("AT+CGATT=1"), F("OK"), 10000)) {
      logError(F("GPRS attach failed"));
      return false;
    }
    delay(2000);
  } else if (attachResp.indexOf(F("+CGATT: 1")) != -1) {
    logDebug(F("GPRS already attached"));
  }
  
  // Set APN (will handle if already set)
  if (!setAPN(apn)) {
    logError(F("APN configuration failed"));
    return false;
  }
  
  // Configure authentication if provided
  if (user.length() > 0) {
    logDebug(F("Configuring PDP authentication..."));
    String authCmd = F("AT+QICSGP=1,1,\"") + apn + F("\",\"") + user + F("\",\"") + pass + F("\",") + String(auth);
    
    flushInput();
    _serial->println(authCmd);
    String authResp = readResponse(2000);
    
    if (authResp.indexOf(F("OK")) == -1 && authResp.indexOf(F("Operation not allowed")) == -1) {
      logError(F("Authentication configuration failed"));
      return false;
    }
  }
  
  logDebug(F("Data attach completed successfully"));
  return true;
}

bool QuectelEC200U::activatePDP(int ctxId) {
  return sendAT("AT+QIACT=" + String(ctxId), "OK", 15000);
}

bool QuectelEC200U::deactivatePDP(int ctxId) {
  return sendAT("AT+QIDEACT=" + String(ctxId), "OK", 15000);
}

int QuectelEC200U::getRegistrationStatus(bool eps) {
  _serial->println(eps ? "AT+CEREG?" : "AT+CREG?");
  String resp = readResponse(1000);
  // Response is typically: +CEREG: <n>,<stat>
  String tag = eps ? "+CEREG: " : "+CREG: ";
  int tag_index = resp.indexOf(tag);
  if (tag_index == -1) return -1;

  int c1 = resp.indexOf(',', tag_index);
  if (c1 == -1) return -1;
  
  String statStr = resp.substring(c1 + 1);
  // The status is the first integer we can parse from the rest of the string
  return atoi(statStr.c_str());
}

bool QuectelEC200U::isSimReady() {
  return sendAT("AT+CPIN?", "READY");
}

// ===== SMS =====
bool QuectelEC200U::sendSMS(const String &number, const String &text) {
  if (!sendAT("AT+CMGF=1")) return false;
  if (!sendAT("AT+CMGS=\"" + number + "\"", ">", 2000)) return false;
  _serial->print(text);
  _serial->write(26);
  
  // Wait for "OK"
  String resp = readResponse(10000);
  return resp.indexOf("OK") != -1;
}

String QuectelEC200U::readSMS(int index) {
  _serial->println("AT+CMGR=" + String(index));
  String resp = readResponse(2000);
  // Response is typically: +CMGR: <stat>,<oa>,<alpha>,<scts><CR><LF><data>
  // OK
  String tag = F("+CMGR: ");
  int tag_index = resp.indexOf(tag);
  if (tag_index == -1) return "";

  int sms_start = resp.indexOf('\n', tag_index);
  if (sms_start == -1) return "";

  int sms_end = resp.indexOf(F("\r\nOK\r\n"), sms_start);
  if (sms_end == -1) return "";

  return resp.substring(sms_start + 1, sms_end);
}

// ===== HTTP =====
bool QuectelEC200U::httpGet(const String &url, String &response) {
  if (!sendAT(F("AT+QHTTPCFG=\"contextid\",1"))) return false;
  if (!sendAT("AT+QHTTPURL=" + String(url.length()), F("CONNECT"))) return false;
  _serial->print(url);
  if (!expectURC(F("OK"), 5000)) return false;

  if (!sendAT(F("AT+QHTTPGET=60"), F("OK"), 15000)) return false;
  if (!expectURC(F("+QHTTPGET:"), 20000)) return false;
  sendAT(F("AT+QHTTPREAD"));
  response = readResponse(15000);
  return response.length() > 0 && response.indexOf(F("ERROR")) == -1;
}

bool QuectelEC200U::httpPost(const String &url, const String &data, String &response) {
  if (!sendAT(F("AT+QHTTPCFG=\"contextid\",1"))) return false;
  if (!sendAT("AT+QHTTPURL=" + String(url.length()), F("CONNECT"))) return false;
  _serial->print(url);
  if (!expectURC(F("OK"), 5000)) return false;

  String cmd = "AT+QHTTPPOST=" + String(data.length()) + ",60,60";
  if (!sendAT(cmd, F("CONNECT"))) return false;
  _serial->print(data);
  if (!expectURC(F("OK"), 10000)) return false;
  if (!expectURC(F("+QHTTPPOST:"), 20000)) return false;
  sendAT(F("AT+QHTTPREAD"));
  response = readResponse(10000);
  return response.indexOf(F("ERROR")) == -1;
}

// ===== HTTPS =====
bool QuectelEC200U::httpsGet(const String &url, String &response) {
  if (!sendAT(F("AT+QHTTPCFG=\"contextid\",1"))) return false;
  if (!sendAT(F("AT+QHTTPCFG=\"sslctxid\",1"))) return false;
  // You may need to configure the SSL context first using sslConfigure()
  if (!sendAT("AT+QHTTPURL=" + String(url.length()), F("CONNECT"))) return false;
  _serial->print(url);
  if (!expectURC(F("OK"), 5000)) return false;

  if (!sendAT(F("AT+QHTTPGET=60"), F("OK"), 20000)) return false;
  if (!expectURC(F("+QHTTPGET:"), 20000)) return false;
  sendAT(F("AT+QHTTPREAD"));
  response = readResponse(20000);
  return response.length() > 0 && response.indexOf(F("ERROR")) == -1;
}

bool QuectelEC200U::httpsPost(const String &url, const String &data, String &response) {
  if (!sendAT(F("AT+QHTTPCFG=\"contextid\",1"))) return false;
  if (!sendAT(F("AT+QHTTPCFG=\"sslctxid\",1"))) return false;
  // You may need to configure the SSL context first using sslConfigure()
  if (!sendAT("AT+QHTTPURL=" + String(url.length()), F("CONNECT"))) return false;
  _serial->print(url);
  if (!expectURC(F("OK"), 5000)) return false;

  String cmd = "AT+QHTTPPOST=" + String(data.length()) + ",60,60";
  if (!sendAT(cmd, F("CONNECT"))) return false;
  _serial->print(data);
  if (!expectURC(F("OK"), 10000)) return false;
  if (!expectURC(F("+QHTTPPOST:"), 20000)) return false;
  sendAT(F("AT+QHTTPREAD"));
  response = readResponse(10000);
  return response.indexOf(F("ERROR")) == -1;
}

// ===== TCP sockets =====
int QuectelEC200U::tcpOpen(const String &host, int port, int ctxId, int socketId) {
  String cmd = "AT+QIOPEN=" + String(ctxId) + "," + String(socketId) + ",\"TCP\",\"" + host + "\"," + String(port) + ",0,1";
  if (!sendAT(cmd, "OK", 5000)) return -1;
  if (!expectURC("+QIOPEN: " + String(socketId) + ",0", 15000)) return -1;
  return socketId;
}

bool QuectelEC200U::tcpSend(int socketId, const String &data) {
  String cmd = "AT+QISEND=" + String(socketId) + "," + String(data.length());
  if (!sendAT(cmd, F(">"), 2000)) return false;
  _serial->print(data);
  
  // Wait for "SEND OK"
  String resp = readResponse(5000);
  return resp.indexOf(F("SEND OK")) != -1;
}

bool QuectelEC200U::tcpRecv(int socketId, String &out, size_t bytes, uint32_t timeout) {
  _serial->println("AT+QIRD=" + String(socketId) + "," + String(bytes));
  String resp = readResponse(timeout);
  
  // Response is typically: +QIRD: <len>\r\n<data>
  String tag = F("+QIRD: ");
  int tag_index = resp.indexOf(tag);
  if (tag_index == -1) {
    return false;
  }

  int len_start = tag_index + tag.length();
  int len_end = resp.indexOf('\r', len_start);
  if (len_end == -1) {
    return false;
  }
  String len_str = resp.substring(len_start, len_end);
  len_str.trim();
  int len = len_str.toInt();

  if (len > 0) {
    int data_start = len_end + 2; // Skip \r\n
    out = resp.substring(data_start, data_start + len);
    return true;
  }
  
  return false;
}

bool QuectelEC200U::tcpClose(int socketId) {
  return sendAT("AT+QICLOSE=" + String(socketId), "OK", 5000);
}

// ===== USSD =====
bool QuectelEC200U::sendUSSD(const String &code, String &response) {
  _serial->println("AT+CUSD=1,\"" + code + "\",15");
  String resp = readResponse(15000); // Increased timeout
  
  if (resp.indexOf(F("OK")) != -1 && resp.indexOf(F("+CUSD:")) != -1) {
    int urc_start = resp.indexOf(F("+CUSD:"));
    response = resp.substring(urc_start);
    return true;
  }
  
  return false;
}

// ===== NTP / Clock =====
bool QuectelEC200U::ntpSync(const String &server, int timezone) {
  if (!sendAT("AT+QNTP=1,\"" + server + "\"", F("OK"), 1000)) return false;
  return expectURC(F("+QNTP: 0"), 20000);
}

String QuectelEC200U::getClock() {
  _serial->println(F("AT+CCLK?"));
  String resp = readResponse(1000);
  // Response is typically: +CCLK: "yy/MM/dd,HH:mm:ss±zz"
  // OK
  String tag = F("+CCLK: ");
  int tag_index = resp.indexOf(tag);
  if (tag_index == -1) return "";

  int time_start = tag_index + tag.length();
  int time_end = resp.indexOf('\r', time_start);
  if (time_end == -1) return "";

  return resp.substring(time_start, time_end);
}

bool QuectelEC200U::setClock(const String &datetime) {
  return sendAT("AT+CCLK=\"" + datetime + "\"");
}

// ===== GNSS =====
bool QuectelEC200U::startGNSS() {
  return sendAT("AT+QGPS=1");
}

bool QuectelEC200U::stopGNSS() {
  return sendAT("AT+QGPSEND");
}

bool QuectelEC200U::isGNSSOn() {
  return sendAT("AT+QGPS?", "+QGPS: 1");
}

bool QuectelEC200U::setGNSSConfig(const String &item, const String &value) {
  return sendAT(String("AT+QGPSCFG=\"") + item + "\"," + value);
}

String QuectelEC200U::getNMEASentence(const String &type) {
  _serial->println(String("AT+QGPSGNMEA=") + type);
  String resp = readResponse(1500);
  // Response is typically: +QGPSGNMEA: <nmea_sentence>
  // OK
  String tag = F("+QGPSGNMEA: ");
  int tag_index = resp.indexOf(tag);
  if (tag_index == -1) return "";

  int sentence_start = tag_index + tag.length();
  int sentence_end = resp.indexOf('\r', sentence_start);
  if (sentence_end == -1) return "";

  return resp.substring(sentence_start, sentence_end);
}

String QuectelEC200U::getGNSSLocation() {
  _serial->println(F("AT+QGPSLOC=2"));
  String resp = readResponse(2000);
  // Response is typically: +QGPSLOC: <latitude>,<longitude>,...
  // OK
  String tag = F("+QGPSLOC: ");
  int tag_index = resp.indexOf(tag);
  if (tag_index == -1) return "";

  int sentence_start = tag_index + tag.length();
  int sentence_end = resp.indexOf('\r', sentence_start);
  if (sentence_end == -1) return "";

  return resp.substring(sentence_start, sentence_end);
}

String QuectelEC200U::getGNSSLocation(uint32_t fixWaitMs) {
  uint32_t start = millis();
  while (millis() - start < fixWaitMs) {
    String loc = getGNSSLocation();
    if (loc.length() > 0) {
      return loc;
    }
    delay(1000);
  }
  return String();
}

// ===== TTS =====
bool QuectelEC200U::playTTS(const String &text) {
  return sendAT("AT+QTTS=1,\"" + text + "\"");
}

// ===== FTP =====
bool QuectelEC200U::ftpLogin(const String &server, const String &user, const String &pass) {
  if (!sendAT("AT+QFTPCFG=\"account\",\"" + user + "\",\"" + pass + "\"")) return false;
  return sendAT("AT+QFTPOPEN=\"" + server + "\",21", F("+QFTP"), 15000);
}

bool QuectelEC200U::ftpDownload(const String &filename, String &data) {
  _serial->println("AT+QFTPGET=\"" + filename + "\"");
  String resp = readResponse(10000);
  
  // Response is typically:
  // +QFTPGET: 1,0
  // +QFTPGET: 2,77
  // ... (77 bytes of data) ...
  // +QFTPGET: 0,0
  
  if (resp.indexOf(F("+QFTPGET: 1,0")) == -1) {
    return false;
  }

  String size_tag = F("+QFTPGET: 2,");
  int size_tag_index = resp.indexOf(size_tag);
  if (size_tag_index == -1) {
    return false;
  }

  int size_start = size_tag_index + size_tag.length();
  int size_end = resp.indexOf('\r', size_start);
  if (size_end == -1) {
    return false;
  }
  String size_str = resp.substring(size_start, size_end);
  size_str.trim();
  int size = size_str.toInt();

  if (size > 0) {
    int data_start = size_end + 2; // Skip \r\n
    data = resp.substring(data_start, data_start + size);
    return true;
  }

  return false;
}

// ===== Filesystem =====
bool QuectelEC200U::fsList(String &out) {
  _serial->println(F("AT+QFLST"));
  String resp = readResponse(2000);
  // Response is typically: +QFLST: ...
  // OK
  int list_start = resp.indexOf(F("+QFLST:"));
  if (list_start != -1) {
    int list_end = resp.indexOf(F("\r\nOK\r\n"), list_start);
    if (list_end != -1) {
      out = resp.substring(list_start, list_end);
      return true;
    }
  }
  return false;
}

bool QuectelEC200U::fsUpload(const String &path, const String &content) {
  String cmd = "AT+QFUPL=\"" + path + "\"," + String(content.length()) + ",100";
  if (!sendAT(cmd, F("CONNECT"), 3000)) return false;
  _serial->print(content);
  
  // Wait for "OK"
  String resp = readResponse(5000);
  return resp.indexOf(F("OK")) != -1;
}

bool QuectelEC200U::fsRead(const String &path, String &out, size_t length) {
  // Open file
  _serial->println("AT+QFOPEN=\"" + path + "\",0");
  String resp = readResponse(1000);
  if (resp.indexOf(F("+QFOPEN:")) == -1) {
    return false;
  }
  int handle_start = resp.indexOf(":") + 1;
  int handle_end = resp.indexOf('\r', handle_start);
  if (handle_end == -1) {
    return false;
  }
  String handle_str = resp.substring(handle_start, handle_end);
  handle_str.trim();
  int handle = handle_str.toInt();

  // Read file
  _serial->println("AT+QFREAD=" + String(handle) + "," + String(length ? length : 1024));
  String read_resp = readResponse(5000);
  
  // Close file
  sendAT("AT+QFCLOSE=" + String(handle));

  int content_start = read_resp.indexOf(F("CONNECT\r\n"));
  if (content_start == -1) {
    return false;
  }
  content_start += 9; // length of "CONNECT\r\n"

  int content_end = read_resp.indexOf(F("\r\nOK\r\n"), content_start);
  if (content_end == -1) {
    return false;
  }

  out = read_resp.substring(content_start, content_end);
  return true;
}

bool QuectelEC200U::fsDelete(const String &path) {
  return sendAT("AT+QFDEL=\"" + path + "\"");
}

// ===== SSL/TLS =====
bool QuectelEC200U::sslConfigure(int ctxId, const String &caPath, bool verify) {
  if (!sendAT("AT+QSSLCFG=\"cacert\"," + String(ctxId) + ",\"" + caPath + "\"")) return false;
  return sendAT("AT+QSSLCFG=\"seclevel\"," + String(ctxId) + "," + String(verify ? 2 : 0));
}

// ===== PSM =====
bool QuectelEC200U::enablePSM(bool enable) {
  return sendAT(String("AT+CPSMS=") + (enable ? "1" : "0"));
}

// ===== MQTT =====
bool QuectelEC200U::mqttConnect(const String &server, int port) {
  if (!sendAT("AT+QMTOPEN=0,\"" + server + "\"," + String(port), "+QMTOPEN: 0,0", 15000)) return false;
  return sendAT("AT+QMTCONN=0,\"ec200u\"", "+QMTCONN: 0,0", 10000);
}

bool QuectelEC200U::mqttPublish(const String &topic, const String &message) {
  String cmd = "AT+QMTPUB=0,0,0,0,\"" + topic + "\"";
  if (!sendAT(cmd, F(">"), 2000)) return false;
  _serial->print(message);
  _serial->write(26);
  
  // Wait for "OK"
  String resp = readResponse(5000);
  return resp.indexOf(F("OK")) != -1;
}

bool QuectelEC200U::mqttSubscribe(const String &topic) {
  return sendAT("AT+QMTSUB=0,1,\"" + topic + "\",0", F("+QMTSUB: 0,1,0"), 5000);
}

// ===== Voice Call =====
bool QuectelEC200U::dial(const String &number) {
  return sendAT(String("ATD") + number + ";");
}

bool QuectelEC200U::hangup() {
  return sendAT("ATH");
}

bool QuectelEC200U::answer() {
  return sendAT("ATA");
}

String QuectelEC200U::getCallList() {
  _serial->println(F("AT+CLCC"));
  String resp = readResponse(2000);
  // Response is typically: +CLCC: ...
  // OK
  String tag = F("+CLCC: ");
  int tag_index = resp.indexOf(tag);
  if (tag_index == -1) {
    // If no "+CLCC: " tag, it might be an empty list, which just returns "OK"
    if (resp.indexOf(F("OK")) != -1) {
      return "";
    }
    return ""; // Should not happen
  }

  int list_start = tag_index;
  int list_end = resp.indexOf(F("\r\nOK\r\n"), list_start);
  if (list_end == -1) return "";

  return resp.substring(list_start, list_end);
}

bool QuectelEC200U::enableCallerId(bool enable) {
  return sendAT(String("AT+CLIP=") + (enable ? "1" : "0"));
}

// ===== Audio (speaker/microphone) =====
bool QuectelEC200U::setSpeakerVolume(int level) {
  level = constrain(level, 0, 100);
  return sendAT(String("AT+CLVL=") + level);
}

bool QuectelEC200U::setRingerVolume(int level) {
  level = constrain(level, 0, 100);
  return sendAT(String("AT+CRSL=") + level);
}

bool QuectelEC200U::setMicMute(bool mute) {
  return sendAT(String("AT+CMUT=") + (mute ? 1 : 0));
}

bool QuectelEC200U::setMicGain(int channel, int level) {
  level = constrain(level, 0, 15);
  return sendAT(String("AT+QMIC=") + channel + "," + level);
}

bool QuectelEC200U::setSidetone(bool enable, int level) {
  level = constrain(level, 0, 15);
  return sendAT(String("AT+QSIDET=") + (enable ? 1 : 0) + "," + level);
}

bool QuectelEC200U::setAudioChannel(int channel) {
  return sendAT(String("AT+QAUDCH=") + channel);
}

bool QuectelEC200U::setAudioInterface(const String &params) {
  return sendAT(String("AT+QDAI=") + params);
}

bool QuectelEC200U::audioLoopback(bool enable) {
  return sendAT(String("AT+QAUDLOOP=") + (enable ? 1 : 0));
}
