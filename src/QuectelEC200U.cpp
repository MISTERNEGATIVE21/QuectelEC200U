/*
  QuectelEC200U- Arduino library for Quectel EC200U (CN-AA)
  Author: misternegative21
  Maintainer: MisterNegative21 <misternegative21@gmail.com>
  Repository: https://github.com/MISTERNEGATIVE21/QuectelEC200U
  License: MIT (see LICENSE)

  Quectel and EC200U are trademarks of Quectel Wireless Solutions Co., Ltd.
  This library is an independent, unofficial project and is not affiliated with or endorsed by Quectel.
*/

#include "QuectelEC200U.h"
#include <ArduinoJson.h>

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

// ... (rest of the file) ...

bool QuectelEC200U::parseJson(const String &jsonString, JsonDocument &doc) {
  DeserializationError error = deserializeJson(doc, jsonString);
  if (error) {
    logError(F("JSON deserialization failed: "));
    logError(error.c_str());
    return false;
  }
  return true;
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
  logDebug(F("Starting AT SYNC..."));

  // 1. Start AT SYNC: Send AT every 500ms, up to 10 times
  bool syncSuccess = false;
  for (int i = 0; i < 10; i++) {
    if (sendAT(F("AT"), F("OK"), 500)) {
      syncSuccess = true;
      logDebug(F("SYNC success"));
      break;
    }
    delay(500);
  }

  if (!syncSuccess) {
    logError(F("SYNC fail"));
    _lastError = ErrorCode::MODEM_NOT_RESPONDING;
    return false;
  }

  // 2. ATI - Module Info
  sendAT(F("ATI"));

  // 3. ATV1 - Verbose response format
  sendAT(F("ATV1"), F("OK"));

  // 4. ATE0 - Disable Echo (Library requires this for parsing, despite user log showing ATE1)
  if (!_echoDisabled) {
    if (sendAT(F("ATE0"), F("OK"), 1000)) {
      _echoDisabled = true;
      logDebug(F("Echo disabled"));
    }
  }

  // 5. AT+CMEE=2 - Verbose errors
  sendAT(F("AT+CMEE=2"), F("OK"));

  // 6. AT+IPR? - Baudrate
  sendAT(F("AT+IPR?"));

  // 7. AT+GSN - IMEI
  sendAT(F("AT+GSN"));

  // 8. AT+CPIN? - SIM Status
  sendAT(F("AT+CPIN?"));

  // 9. AT+CIMI - IMSI
  sendAT(F("AT+CIMI"));

  // 10. AT+QCCID - ICCID
  sendAT(F("AT+QCCID"));

  // 11. AT+CSQ - Signal Quality
  sendAT(F("AT+CSQ"));

  // 12. Network Registration Status
  sendAT(F("AT+CREG?"));
  sendAT(F("AT+CGREG?"));
  sendAT(F("AT+COPS?"));
  sendAT(F("AT+CEREG?"));

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
        _lastError = ErrorCode::SIM_NOT_READY;
        return false;
      }
      delay(1000);
    }
  }

  // Check signal quality
  if (getSignalStrength() < 10) {
    logError(F("Signal quality is too low"));
    _lastError = ErrorCode::SIGNAL_QUALITY_LOW;
  }

  // Check GPRS attachment
  if (!sendAT(F("AT+CGATT?"), F("+CGATT: 1"))) {
    logError(F("GPRS not attached"));
    _lastError = ErrorCode::GPRS_NOT_ATTACHED;
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

// Send AT command without waiting for response (for manual handling)
void QuectelEC200U::sendATRaw(const String &cmd) {
  if (_debugSerial) {
    _debugSerial->print(F("CMD (Raw): "));
    _debugSerial->println(cmd);
  }
  _serial->println(cmd);
}

// Inspired by simple AT command approach - clean and efficient
bool QuectelEC200U::sendAT(const String &cmd, const String &expect, uint32_t timeout) {
  if (_debugSerial) {
    _debugSerial->print(F("CMD: "));
    _debugSerial->println(cmd);
  }

  _serial->println(cmd);

  char buffer[256];
  readResponse(buffer, sizeof(buffer), timeout);

  if (_debugSerial) {
    _debugSerial->print(F("RESP: "));
    _debugSerial->println(buffer);
  }

  if (strstr(buffer, expect.c_str()) != NULL) {
    _lastError = ErrorCode::NONE;
    return true;
  }

  if (strstr(buffer, "+CME ERROR:") != NULL) {
    _lastError = (ErrorCode)extractInteger(buffer, F("+CME ERROR:"));
    return false;
  }

  if (strstr(buffer, "+CMS ERROR:") != NULL) {
    _lastError = (ErrorCode)extractInteger(buffer, F("+CMS ERROR:"));
    return false;
  }

  if (strstr(buffer, "ERROR") != NULL) {
    _lastError = ErrorCode::UNKNOWN;
    return false;
  }

  _lastError = ErrorCode::UNKNOWN;
  return false;
}

bool QuectelEC200U::sendAT(const String &cmd) {
  return sendAT(cmd, F("OK"), 1000);
}



[[deprecated("Use readResponse(char*, size_t, uint32_t) instead")]] String QuectelEC200U::readResponse(uint32_t timeout) {
  char buffer[256];
  readResponse(buffer, sizeof(buffer), timeout);
  return String(buffer);
}

int QuectelEC200U::readResponse(char* buffer, size_t length, uint32_t timeout) {
  size_t bytesRead = 0;
  uint32_t start = millis();

  while (millis() - start < timeout && bytesRead < length - 1) {
    while (_serial->available() && bytesRead < length - 1) {
      char c = (char)_serial->read();
      buffer[bytesRead++] = c;
      if (_debugSerial) {
        _debugSerial->print(c);
      }
    }
    buffer[bytesRead] = '\0';

    // Exit early if we have a complete response
    if (strstr(buffer, "\r\nOK\r\n") != NULL ||
        strstr(buffer, "\r\nERROR\r\n") != NULL ||
        strstr(buffer, "\r\n> ") != NULL ||
        strstr(buffer, "+CME ERROR:") != NULL) {
      break;
    }

    // Small delay to allow buffer to fill
    if (!_serial->available()) {
      delay(10);
    }
  }

  return bytesRead;
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

// Utility functions
String QuectelEC200U::extractQuotedString(const char* response, const String &tag) {
  const char* tag_c = tag.c_str();
  const char* tagIdx = strstr(response, tag_c);
  if (tagIdx == NULL) return "";

  const char* start = strchr(tagIdx, '"');
  if (start == NULL) return "";

  const char* end = strchr(start + 1, '"');
  if (end == NULL) return "";

  int length = end - (start + 1);
  char buffer[length + 1];
  strncpy(buffer, start + 1, length);
  buffer[length] = '\0';

  return String(buffer);
}

int QuectelEC200U::extractInteger(const char* response, const String &tag) {
  const char* tag_c = tag.c_str();
  const char* tagIdx = strstr(response, tag_c);
  if (tagIdx == NULL) return -1;

  const char* start = tagIdx + strlen(tag_c);
  while (*start != '\0' && !isDigit(*start) && *start != '-') {
    start++;
  }

  if (*start == '\0') return -1;

  return atoi(start);
}

void QuectelEC200U::_sendHttpHeaders(String headers[], size_t header_size) {
  if (headers == nullptr || header_size == 0) {
    return;
  }

  logDebug(F("Sending custom HTTP headers..."));
  if (!sendAT(F("AT+QHTTPCFG=\"requestheader\",1"))) {
    logError(F("Failed to enable custom request headers."));
    return;
  }

  for (size_t i = 0; i < header_size; i++) {
    String headerLine = headers[i];
    headerLine.trim();
    if (headerLine.length() > 0) {
      String cmd = "AT+QHTTPCFG=\"header\",\"" + headerLine + "\\r\\n\"";
      if (!sendAT(cmd)) {
        logError(String(F("Failed to send header: ")) + headerLine);
      }
    }
  }
}

// Modem info functions with better formatting
String QuectelEC200U::getModemInfo() {
  String info;
  info.reserve(256); // Pre-allocate memory

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
  info += F("Signal: ");
  info += String(signal);
  info += F(" (");
  info += _getSignalStrengthString(signal);
  info += F(")\n");

  String oper = getOperator();
  if (oper.length() > 0) {
    info += F("Operator: ");
    info += oper;
    info += F("\n");
  }

  int regStatus = getRegistrationStatus();
  info += F("Registration: ");
  info += _getRegistrationStatusString(regStatus);
  info += F("\n");

  info += F("========================");
  return info;
}

String QuectelEC200U::getOperator() {
  _serial->println(F("AT+COPS?"));
  String resp = readResponse(1000);
  return extractQuotedString(resp.c_str(), F("+COPS:"));
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

[[deprecated("Use begin() instead")]] bool QuectelEC200U::modem_init() {
  return begin();
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
  return _parseCsvInt(resp, F("+CSQ: "), 0);
}

bool QuectelEC200U::setAPN(const char* apn) {
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

bool QuectelEC200U::attachData(const char* apn, const char* user, const char* pass, int auth) {
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
      _lastError = ErrorCode::GPRS_NOT_ATTACHED;
      return false;
    }
    delay(2000);
  } else if (attachResp.indexOf(F("+CGATT: 1")) != -1) {
    logDebug(F("GPRS already attached"));
  }
  
  // Set APN (will handle if already set)
  if (!setAPN(apn)) {
    logError(F("APN configuration failed"));
    _lastError = ErrorCode::APN_CONFIG_FAILED;
    return false;
  }
  
  // Configure authentication if provided
  if (strlen(user) > 0) {
    logDebug(F("Configuring PDP authentication..."));
    String authCmd = String("AT+QICSGP=1,1,\"") + apn + "\",\"" + user + "\",\"" + pass + "\"," + String(auth);
    
    flushInput();
    _serial->println(authCmd);
    String authResp = readResponse(2000);
    
    if (authResp.indexOf(F("OK")) == -1 && authResp.indexOf(F("Operation not allowed")) == -1) {
      logError(F("Authentication configuration failed"));
      _lastError = ErrorCode::AUTH_CONFIG_FAILED;
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
  _serial->println(eps ? F("AT+CEREG?") : F("AT+CREG?"));
  String resp = readResponse(1000);
  String tag = eps ? F("+CEREG: ") : F("+CREG: ");
  return _parseCsvInt(resp, tag, 1);
}

bool QuectelEC200U::isSimReady() {
  return sendAT("AT+CPIN?", "READY");
}

// ===== SMS =====
bool QuectelEC200U::sendSMS(const char* number, const char* text) {
  if (!sendAT("AT+CMGF=1")) return false;
  if (!sendAT(String("AT+CMGS=\"") + number + "\"", ">", 2000)) return false;
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
bool QuectelEC200U::httpGet(const String &url, String &response, String headers[], size_t header_size) {
  return _sendHttpRequest(url, "", response, headers, header_size, false, false);
}

bool QuectelEC200U::httpPost(const String &url, const String &data, String &response, String headers[], size_t header_size) {
  return _sendHttpRequest(url, data, response, headers, header_size, false, true);
}

bool QuectelEC200U::httpPost(const String &url, const JsonDocument &json, String &response, String headers[], size_t header_size) {
  String data;
  serializeJson(json, data);
  return httpPost(url, data, response, headers, header_size);
}

// ===== HTTPS =====
bool QuectelEC200U::httpsGet(const String &url, String &response, String headers[], size_t header_size) {
  return _sendHttpRequest(url, "", response, headers, header_size, true, false);
}

bool QuectelEC200U::httpsPost(const String &url, const String &data, String &response, String headers[], size_t header_size) {
  return _sendHttpRequest(url, data, response, headers, header_size, true, true);
}

bool QuectelEC200U::httpsPost(const String &url, const JsonDocument &json, String &response, String headers[], size_t header_size) {
  String data;
  serializeJson(json, data);
  return httpsPost(url, data, response, headers, header_size);
}

ErrorCode QuectelEC200U::getLastError() {
  return _lastError;
}

String QuectelEC200U::getLastErrorString() {
  switch (_lastError) {
    case ErrorCode::NONE: return "No error";
    case ErrorCode::UNKNOWN: return "Unknown error";
    case ErrorCode::MODEM_NOT_RESPONDING: return "Modem not responding";
    case ErrorCode::SIM_NOT_READY: return "SIM not ready";
    case ErrorCode::SIGNAL_QUALITY_LOW: return "Signal quality too low";
    case ErrorCode::GPRS_NOT_ATTACHED: return "GPRS not attached";
    case ErrorCode::APN_CONFIG_FAILED: return "APN configuration failed";
    case ErrorCode::AUTH_CONFIG_FAILED: return "Authentication configuration failed";
    case ErrorCode::PDP_ACTIVATION_FAILED: return "PDP activation failed";
    case ErrorCode::HTTP_ERROR: return "HTTP error";
    case ErrorCode::HTTP_CONTEXT_ID_FAILED: return "HTTP context ID failed";
    case ErrorCode::HTTP_SSL_CONTEXT_ID_FAILED: return "HTTP SSL context ID failed";
    case ErrorCode::HTTP_URL_FAILED: return "HTTP URL failed";
    case ErrorCode::HTTP_URL_WRITE_FAILED: return "HTTP URL write failed";
    case ErrorCode::HTTP_POST_FAILED: return "HTTP POST failed";
    case ErrorCode::HTTP_POST_DATA_WRITE_FAILED: return "HTTP POST data write failed";
    case ErrorCode::HTTP_POST_URC_FAILED: return "HTTP POST URC failed";
    case ErrorCode::HTTP_GET_FAILED: return "HTTP GET failed";
    case ErrorCode::HTTP_GET_URC_FAILED: return "HTTP GET URC failed";
    case ErrorCode::HTTP_READ_FAILED: return "HTTP read failed";
    case ErrorCode::FTP_ERROR: return "FTP error";
    case ErrorCode::MQTT_ERROR: return "MQTT error";
    case ErrorCode::TCP_ERROR: return "TCP error";
    case ErrorCode::SSL_ERROR: return "SSL error";
    case ErrorCode::FS_ERROR: return "Filesystem error";
    default: return "Unknown error code";
  }
}

bool QuectelEC200U::_sendHttpRequest(const String &url, const String &data, String &response, String headers[], size_t header_size, bool ssl, bool isPost) {
  if (!sendAT(F("AT+QHTTPCFG=\"contextid\",1"))) {
    _lastError = ErrorCode::HTTP_CONTEXT_ID_FAILED;
    return false;
  }
  if (ssl) {
    if (!sendAT(F("AT+QHTTPCFG=\"sslctxid\",1"))) {
      _lastError = ErrorCode::HTTP_SSL_CONTEXT_ID_FAILED;
      return false;
    }
  }

  _sendHttpHeaders(headers, header_size);

  // Use a 10-second timeout for the URL
  if (!sendAT("AT+QHTTPURL=" + String(url.length()) + ",10", F("CONNECT"))) {
    sendAT(F("AT+QHTTPCFG=\"requestheader\",0"));
    _lastError = ErrorCode::HTTP_URL_FAILED;
    return false;
  }
  
  // Send URL in chunks
  int urlLength = url.length();
  for (int i = 0; i < urlLength; i += HTTP_URL_CHUNK_SIZE) {
    String chunk = url.substring(i, i + HTTP_URL_CHUNK_SIZE);
    _serial->print(chunk);
    // Small delay to allow module to process the chunk
    delay(10); 
  }

  if (!expectURC(F("OK"), 5000)) {
    sendAT(F("AT+QHTTPCFG=\"requestheader\",0"));
    _lastError = ErrorCode::HTTP_URL_WRITE_FAILED;
    return false;
  }

  if (isPost) {
    String cmd = "AT+QHTTPPOST=" + String(data.length()) + ",60,60";
    if (!sendAT(cmd, F("CONNECT"))) {
      sendAT(F("AT+QHTTPCFG=\"requestheader\",0"));
      _lastError = ErrorCode::HTTP_POST_FAILED;
      return false;
    }
    _serial->print(data);
    if (!expectURC(F("OK"), 10000)) {
      sendAT(F("AT+QHTTPCFG=\"requestheader\",0"));
      _lastError = ErrorCode::HTTP_POST_DATA_WRITE_FAILED;
      return false;
    }
    if (!expectURC(F("+QHTTPPOST:"), 20000)) {
      sendAT(F("AT+QHTTPCFG=\"requestheader\",0"));
      _lastError = ErrorCode::HTTP_POST_URC_FAILED;
      return false;
    }
  } else {
    if (!sendAT(F("AT+QHTTPGET=60"), F("OK"), 15000)) {
      sendAT(F("AT+QHTTPCFG=\"requestheader\",0"));
      _lastError = ErrorCode::HTTP_GET_FAILED;
      return false;
    }
    if (!expectURC(F("+QHTTPGET:"), 20000)) {
      sendAT(F("AT+QHTTPCFG=\"requestheader\",0"));
      _lastError = ErrorCode::HTTP_GET_URC_FAILED;
      return false;
    }
  }

  sendAT(F("AT+QHTTPREAD"));
  response = readResponse(15000);

  sendAT(F("AT+QHTTPCFG=\"requestheader\",0"));
  if (response.indexOf(F("ERROR")) != -1) {
    _lastError = ErrorCode::HTTP_READ_FAILED;
    return false;
  }
  return response.length() > 0;
}

// ===== TCP sockets =====
int QuectelEC200U::tcpOpen(const String &host, int port, int ctxId, int socketId) {
  String cmd = "AT+QIOPEN=" + String(ctxId) + "," + String(socketId) + "\"TCP\"\"" + host + "\"," + String(port) + ",0,1";
  if (!sendAT(cmd, "OK", 5000)) return -1;
  if (!expectURC("+QIOPEN: " + String(socketId) + ",0", 15000)) return -1;
  return socketId;
}

bool QuectelEC200U::tcpSend(int socketId, const String &data) {
  String cmd = "AT+QISEND=" + String(socketId) + "," + String(data.length());
  if (!sendAT(cmd, F("> "), 2000)) return false;
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
bool QuectelEC200U::ntpSync(const String &server, int timezone, int contextID, int port) {
    if (server.length() == 0) {
        return false;
    }
    if (timezone < -48 || timezone > 56) {
        return false;
    }
    String cmd = "AT+QNTP=" + String(contextID) + ",\"" + server + "\"," + String(port) + "," + String(timezone);
    if (!sendAT(cmd, F("OK"), 1000)) return false;
    return expectURC(F("+QNTP: 0"), 125000);
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
bool QuectelEC200U::playTTS(const char* text) {
  return sendAT(String("AT+QTTS=1,\"") + text + "\"");
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

bool QuectelEC200U::sslUploadCert(const String &cert, const String &path) {
  return fsUpload(path, cert);
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
  if (!sendAT(cmd, F("> "), 2000)) return false;
  _serial->print(message);
  _serial->write(26);
  
  // Wait for "OK"
  String resp = readResponse(5000);
  return resp.indexOf(F("OK")) != -1;
}

bool QuectelEC200U::mqttSubscribe(const String &topic) {
  return sendAT("AT+QMTSUB=0,1,\"" + topic + "\",0", F("+QMTSUB: 0,1,0"), 5000);
}

String QuectelEC200U::_getSignalStrengthString(int signal) {
  if (signal < 0) return "Unknown";
  if (signal == 0) return "< -113 dBm";
  if (signal == 1) return "-111 dBm";
  if (signal >= 2 && signal <= 30) {
    return String(-109 + (signal - 2) * 2) + " dBm";
  }
  if (signal == 31) return "> -51 dBm";
  return "Unknown";
}

String QuectelEC200U::_getRegistrationStatusString(int regStatus) {
  switch (regStatus) {
    case 0: return "Not registered";
    case 1: return "Registered (home)";
    case 2: return "Searching...";
    case 3: return "Registration denied";
    case 5: return "Registered (roaming)";
    default: return "Unknown";
  }
}

int QuectelEC200U::_parseCsvInt(const String& response, const String& tag, int index) {
  int tagIdx = response.indexOf(tag);
  if (tagIdx == -1) return -1;

  int currentIdx = tagIdx + tag.length();
  for (int i = 0; i < index; i++) {
    currentIdx = response.indexOf(',', currentIdx);
    if (currentIdx == -1) return -1;
    currentIdx++;
  }

  int endIdx = response.indexOf(',', currentIdx);
  if (endIdx == -1) {
    endIdx = response.indexOf('\r', currentIdx);
    if (endIdx == -1) {
        endIdx = response.length();
    }
  }

  String valStr = response.substring(currentIdx, endIdx);
  valStr.trim();
  return valStr.toInt();
}

// ===== Voice Call =====
bool QuectelEC200U::dial(const char* number) {
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

// ===== Ping =====
bool QuectelEC200U::ping(const String &host, int contextID, int timeout, int pingnum) {
  String cmd = "AT+QPING=" + String(contextID) + ",\"" + host + "\"," + String(timeout) + "," + String(pingnum);
  return sendAT(cmd, F("+QPING:"), timeout * 1000 * pingnum);
}



// ===== DNS =====
bool QuectelEC200U::setDNS(const String &primary, const String &secondary, int contextID) {
    String cmd = "AT+QIDNSCFG=" + String(contextID);
    if (!primary.isEmpty()) {
        cmd += ",\"" + primary + "\"";
        if (!secondary.isEmpty()) {
            cmd += ",\"" + secondary + "\"";
        }
    }
    return sendAT(cmd);
}

String QuectelEC200U::getIpByHostName(const String &hostname, int contextID) {
    String cmd = "AT+QIDNSGIP=" + String(contextID) + ",\"" + hostname + "\"";
    if (!sendAT(cmd, F("OK"), 1000)) return "";
    String resp = readResponse(60000);
    int urcIndex = resp.indexOf(F("+QIURC: \"dnsgip\""));
    if (urcIndex != -1) {
        int first_comma = resp.indexOf(',', urcIndex);
        int second_comma = resp.indexOf(',', first_comma + 1);
        int third_comma = resp.indexOf(',', second_comma + 1);
        int fourth_comma = resp.indexOf(',', third_comma + 1);

        if (first_comma != -1 && second_comma != -1 && third_comma != -1 && fourth_comma != -1) {
            int err = resp.substring(first_comma + 1, second_comma).toInt();
            if (err == 0) {
                int ip_count = resp.substring(second_comma + 1, third_comma).toInt();
                if (ip_count > 0) {
                    int ip_end_index = resp.indexOf('\r', fourth_comma + 1);
                    if (ip_end_index != -1) {
                        return resp.substring(fourth_comma + 1, ip_end_index);
                    }
                }
            }
        }
    }
    return "";
}

// ===== ADC =====
int QuectelEC200U::readADC() {
    _serial->println(F("AT+QADC=0"));
    String resp = readResponse(1000);
    return _parseCsvInt(resp, F("+QADC: "), 1);
}

// ===== Packet Domain =====
String QuectelEC200U::getPacketDataCounter() {
    _serial->println(F("AT+QGDCNT?"));
    return readResponse(1000);
}

String QuectelEC200U::readDynamicPDNParameters(int cid) {
    _serial->println("AT+CGCONTRDP=" + String(cid));
    return readResponse(1000);
}

QuectelEC200U::PDPContext QuectelEC200U::getPDPContext(int cid) {
    PDPContext ctx;
    ctx.cid = -1; // Indicate invalid context initially

    _serial->println(F("AT+CGDCONT?"));
    String resp = readResponse(1000); // Read the full response

    // Look for a line starting with "+CGDCONT: <cid>"
    String searchTag = "+CGDCONT: " + String(cid) + ",";
    int startIndex = resp.indexOf(searchTag);

    if (startIndex != -1) {
        startIndex += searchTag.length(); // Move past the tag
        int endIndex = resp.indexOf('\r', startIndex); // Find end of line
        if (endIndex == -1) { // If no CR, take till end of string
            endIndex = resp.length();
        }
        String line = resp.substring(startIndex, endIndex);

        // Parse the line: "IP","JIONET","0.0.0.0",0,0
        // Use a temporary char array and strtok for parsing, or careful indexOf/substring
        // For simplicity and avoiding strtok modifying String, we'll do careful indexOf
        
        int currentPos = 0;
        int quoteStart, quoteEnd;

        // 1. PDP_type (e.g., "IP")
        quoteStart = line.indexOf('"', currentPos);
        if (quoteStart != -1) {
            quoteEnd = line.indexOf('"', quoteStart + 1);
            if (quoteEnd != -1) {
                ctx.pdp_type = line.substring(quoteStart + 1, quoteEnd);
                currentPos = quoteEnd + 1; // Move past the closing quote
            }
        }

        // Move past comma if present
        if (line.charAt(currentPos) == ',') {
            currentPos++;
        }

        // 2. APN (e.g., "JIONET")
        quoteStart = line.indexOf('"', currentPos);
        if (quoteStart != -1) {
            quoteEnd = line.indexOf('"', quoteStart + 1);
            if (quoteEnd != -1) {
                ctx.apn = line.substring(quoteStart + 1, quoteEnd);
                currentPos = quoteEnd + 1;
            }
        }
        
        // Move past comma if present
        if (line.charAt(currentPos) == ',') {
            currentPos++;
        }

        // 3. P_ADDR (e.g., "0.0.0.0") - can be quoted or not
        quoteStart = line.indexOf('"', currentPos);
        if (quoteStart != -1) { // It's quoted
            quoteEnd = line.indexOf('"', quoteStart + 1);
            if (quoteEnd != -1) {
                ctx.p_addr = line.substring(quoteStart + 1, quoteEnd);
                currentPos = quoteEnd + 1;
            }
        } else { // It's not quoted, read until next comma or end
            int nextComma = line.indexOf(',', currentPos);
            if (nextComma != -1) {
                ctx.p_addr = line.substring(currentPos, nextComma);
                currentPos = nextComma + 1;
            } else {
                ctx.p_addr = line.substring(currentPos); // To end of line
                currentPos = line.length();
            }
        }
        
        ctx.cid = cid; // Mark as valid
    }
    return ctx;
}



// ===== Hardware =====
String QuectelEC200U::getBatteryCharge() {
    _serial->println(F("AT+CBC"));
    return readResponse(1000);
}

String QuectelEC200U::getWifiScan() {
    _serial->println(F("AT+QWIFISCAN"));
    return readResponse(10000); // Wi-Fi scan can take longer
}

// ===== Advanced TCP/IP =====
bool QuectelEC200U::switchDataAccessMode(int connectID, int accessMode) {
    return sendAT("AT+QISWTMD=" + String(connectID) + "," + String(accessMode), (accessMode == 2 ? F("CONNECT") : F("OK")));
}

bool QuectelEC200U::echoSendData(bool enable) {
    return sendAT(String("AT+QISDE=") + (enable ? "1" : "0"));
}

// ===== QCFG - Extended settings =====
bool QuectelEC200U::setNetworkScanMode(int mode) {
    return sendAT("AT+QCFG=\"nwscanmode\"," + String(mode));
}

bool QuectelEC200U::setBand(const String &gsm_mask, const String &lte_mask) {
    return sendAT("AT+QCFG=\"band\"," + gsm_mask + "," + lte_mask);
}

// ===== Modem Identification =====
String QuectelEC200U::getManufacturerIdentification() {
    _serial->println(F("AT+GMI"));
    return readResponse(1000);
}

String QuectelEC200U::getModelIdentification() {
    _serial->println(F("AT+GMM"));
    return readResponse(1000);
}

String QuectelEC200U::getFirmwareRevision() {
    _serial->println(F("AT+GMR"));
    return readResponse(1000);
}

// ===== General Commands =====
bool QuectelEC200U::restoreFactoryDefaults() {
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

String QuectelEC200U::showCurrentConfiguration() {
    _serial->println(F("AT&V"));
    return readResponse(2000);
}

bool QuectelEC200U::storeConfiguration(int profile) {
    return sendAT("AT&W" + String(profile));
}

bool QuectelEC200U::restoreConfiguration(int profile) {
    return sendAT("ATZ" + String(profile));
}

bool QuectelEC200U::setResultCodeEcho(bool enable) {
    return sendAT(String("ATQ") + (enable ? "0" : "1"));
}

bool QuectelEC200U::setResultCodeFormat(bool verbose) {
    return sendAT(String("ATV") + (verbose ? "1" : "0"));
}

bool QuectelEC200U::setCommandEcho(bool enable) {
    return sendAT(String("ATE") + (enable ? "1" : "0"));
}

bool QuectelEC200U::repeatPreviousCommand() {
    _serial->println(F("A/"));
    return expectURC(F("OK"), 3000);
}

bool QuectelEC200U::setSParameter(int s, int value) {
    return sendAT("ATS" + String(s) + "=" + String(value));
}

bool QuectelEC200U::setFunctionMode(int fun, int rst) {
    return sendAT("AT+CFUN=" + String(fun) + "," + String(rst));
}

bool QuectelEC200U::setErrorMessageFormat(int format) {
    return sendAT("AT+CMEE=" + String(format));
}

bool QuectelEC200U::setTECharacterSet(const String &chset) {
    return sendAT("AT+CSCS=\"" + chset + "\"");
}

bool QuectelEC200U::setURCOutputRouting(const String &port) {
    return sendAT("AT+QURCCFG=\"urcport\",\"" + port + "\"");
}

// ===== UART Control Commands =====
bool QuectelEC200U::setDCDFunctionMode(int mode) {
    return sendAT(String("AT&C") + mode);
}

bool QuectelEC200U::setDTRFunctionMode(int mode) {
    return sendAT(String("AT&D") + mode);
}

bool QuectelEC200U::setUARTFlowControl(int dce_by_dte, int dte_by_dce) {
    return sendAT("AT+IFC=" + String(dce_by_dte) + "," + String(dte_by_dce));
}

bool QuectelEC200U::setUARTFrameFormat(int format, int parity) {
    return sendAT("AT+ICF=" + String(format) + "," + String(parity));
}

bool QuectelEC200U::setUARTBaudRate(long rate) {
    return sendAT("AT+IPR=" + String(rate));
}

// ===== Status Control and Extended Settings =====
String QuectelEC200U::getActivityStatus() {
    _serial->println(F("AT+CPAS"));
    return readResponse(1000);
}

bool QuectelEC200U::setURCIndication(const String &urc_type, bool enable) {
    return sendAT("AT+QINDCFG=\"" + urc_type + "\"," + (enable ? "1" : "0"));
}

// ===== (U)SIM Related Commands =====
String QuectelEC200U::getIMSI() {
    _serial->println(F("AT+CIMI"));
    return readResponse(1000);
}

String QuectelEC200U::getICCID() {
    _serial->println(F("AT+QCCID"));
    return readResponse(1000);
}

String QuectelEC200U::getPinRetries() {
    _serial->println(F("AT+QPINC"));
    return readResponse(1000);
}

// ===== Network Service Commands =====
String QuectelEC200U::getDetailedSignalQuality() {
    _serial->println(F("AT+QCSQ"));
    return readResponse(1000);
}

String QuectelEC200U::getNetworkTime() {
    _serial->println(F("AT+QLTS"));
    return readResponse(1000);
}

String QuectelEC200U::getNetworkInfo() {
    _serial->println(F("AT+QNWINFO"));
    return readResponse(1000);
}

// ===== Advanced TCP/IP Configuration =====
bool QuectelEC200U::setTCPConfig(const String &param, const String &value) {
    return sendAT("AT+QICFG=\"" + param + "\"," + value);
}

String QuectelEC200U::getSocketStatus(int connectID) {
    _serial->println("AT+QISTATE=" + String(connectID));
    return readResponse(1000);
}

int QuectelEC200U::getTCPError() {
    _serial->println(F("AT+QIGETERROR"));
    String resp = readResponse(1000);
    return _parseCsvInt(resp, F("+QIGETERROR: "), 0);
}

// ===== Asynchronous PDP Context =====
bool QuectelEC200U::activatePDPAsync(int ctxId) {
    return sendAT("AT+QIACTEX=" + String(ctxId) + ",1", F("OK"), 1000);
}

bool QuectelEC200U::deactivatePDPAsync(int ctxId) {
    return sendAT("AT+QIDEACTEX=" + String(ctxId) + ",1", F("OK"), 1000);
}

// ===== Context Configuration =====
bool QuectelEC200U::configureContext(int ctxId, int type, const String &apn, const String &user, const String &pass, int auth) {
    String cmd = "AT+QICSGP=" + String(ctxId) + "," + String(type) + ",\"" + apn + "\",\"" + user + "\",\"" + pass + "\"," + String(auth);
    return sendAT(cmd);
}

// ===== General Modem Configuration =====
bool QuectelEC200U::setModemConfig(const String &param, const String &value) {
    return sendAT("AT+QCFG=\"" + param + "\"," + value);
}

// ===== Call-Related Commands =====
bool QuectelEC200U::setVoiceHangupControl(int mode) {
    return sendAT("AT+CVHU=" + String(mode));
}

bool QuectelEC200U::hangupVoiceCall() {
    return sendAT("AT+CHUP");
}

bool QuectelEC200U::setConnectionTimeout(int seconds) {
    return sendAT("ATS7=" + String(seconds));
}

// ===== Phonebook Commands =====
String QuectelEC200U::getSubscriberNumber() {
    _serial->println(F("AT+CNUM"));
    return readResponse(1000);
}

String QuectelEC200U::findPhonebookEntries(const String &findtext) {
    _serial->println("AT+CPBF=\"" + findtext + "\"");
    return readResponse(5000);
}

String QuectelEC200U::readPhonebookEntry(int index1, int index2) {
    String cmd = "AT+CPBR=" + String(index1);
    if (index2 != -1) {
        cmd += "," + String(index2);
    }
    _serial->println(cmd);
    return readResponse(5000);
}

bool QuectelEC200U::selectPhonebookStorage(const String &storage) {
    return sendAT("AT+CPBS=\"" + storage + "\"");
}

bool QuectelEC200U::writePhonebookEntry(int index, const String &number, const String &text, int type) {
    return sendAT("AT+CPBW=" + String(index) + ",\"" + number + "\"," + String(type) + ",\"" + text + "\"");
}

// ===== SMS Commands =====
bool QuectelEC200U::setMessageFormat(int mode) {
    return sendAT("AT+CMGF=" + String(mode));
}

bool QuectelEC200U::setServiceCenterAddress(const String &sca) {
    return sendAT("AT+CSCA=\"" + sca + "\"");
}

String QuectelEC200U::listMessages(const String &stat) {
    _serial->println("AT+CMGL=\"" + stat + "\"");
    return readResponse(10000);
}

bool QuectelEC200U::setNewMessageIndication(int mode, int mt, int bm, int ds, int bfr) {
    return sendAT("AT+CNMI=" + String(mode) + "," + String(mt) + "," + String(bm) + "," + String(ds) + "," + String(bfr));
}

// ===== Packet Domain Commands =====
bool QuectelEC200U::gprsAttach(bool attach) {
    return sendAT("AT+CGATT=" + String(attach ? 1 : 0));
}

bool QuectelEC200U::setGPRSClass(const String &gprs_class) {
    return sendAT("AT+CGCLASS=\"" + gprs_class + "\"");
}

bool QuectelEC200U::setPacketDomainEventReporting(int mode) {
    return sendAT("AT+CGEREP=" + String(mode));
}

// Hardware


// ===== Supplementary Service Commands =====
bool QuectelEC200U::setCallForwarding(int reason, int mode, const String &number, int time) {
    return sendAT("AT+CCFC=" + String(reason) + "," + String(mode) + ",\"" + number + "\"," + String(time));
}

bool QuectelEC200U::setCallWaiting(int mode) {
    return sendAT("AT+CCWA=" + String(mode));
}

bool QuectelEC200U::setCallingLineIdentificationPresentation(bool enable) {
    return sendAT("AT+CLIP=" + String(enable ? 1 : 0));
}

bool QuectelEC200U::setCallingLineIdentificationRestriction(int mode) {
    return sendAT("AT+CLIR=" + String(mode));
}

// ===== More Audio Commands =====
bool QuectelEC200U::recordAudio(const String &filename) {
    return sendAT("AT+QAUDRD=\"" + filename + "\"");
}

bool QuectelEC200U::playAudio(const String &filename) {
    return sendAT("AT+QAUDPLAY=\"" + filename + "\"");
}

bool QuectelEC200U::stopAudio() {
    return sendAT("AT+QAUDSTOP");
}

bool QuectelEC200U::playTextToSpeech(const String &text) {
    return sendAT("AT+QTTS=1,\"" + text + "\"");
}



// ===== Remaining TCP/IP Commands =====
bool QuectelEC200U::sendHexData(int connectID, const String &hex_string) {
    return sendAT("AT+QISENDEX=" + String(connectID) + ",\"" + hex_string + "\"");
}


// ===== Advanced Error Reporting and SIM =====
String QuectelEC200U::getExtendedErrorReports() {
    _serial->println(F("AT+CEER"));
    return readResponse(2000);
}

String QuectelEC200U::getSIMStatus() {
    _serial->println(F("AT+CPIN?"));
    return readResponse(1000);
}



// Power Management
void QuectelEC200U::powerOn(int pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW); // Set LOW and keep it there
  delay(2000); // Wait for boot
}

// ==========================================
// Developer Guide Implementation (New Features)
// ==========================================

// [A] Network & SIM Control
bool QuectelEC200U::switchSimCard() {
    return sendAT(F("AT+QSIMCHK"));
}

bool QuectelEC200U::toggleISIM(bool enable) {
    String cmd = F("AT+QIMSCFG=\"isim\",");
    cmd += enable ? "1" : "0";
    return sendAT(cmd);
}

bool QuectelEC200U::setDSDSMode(bool dsds) {
    String cmd = F("AT+QDSTYPE=");
    cmd += dsds ? "1" : "0";
    return sendAT(cmd);
}

String QuectelEC200U::getOperatorName() {
    sendATRaw(F("AT+QSPN"));
    char buffer[256];
    readResponse(buffer, sizeof(buffer), 2000);
    return extractQuotedString(buffer, "+QSPN");
}

bool QuectelEC200U::preventNetworkModeSwitch(bool enable) {
    String cmd = F("AT+QCFG=\"cops_no_mode_change\",");
    cmd += enable ? "1" : "0";
    return sendAT(cmd);
}

// [B] Audio & Voice
bool QuectelEC200U::blockIncomingCalls(bool enable) {
    String cmd = F("AT+QREFUSECS=");
    cmd += enable ? "1" : "0";
    return sendAT(cmd);
}

bool QuectelEC200U::playAudioDuringCall(const char* filename) {
    String cmd = F("AT+QAUDPLAY=\"");
    cmd += filename;
    cmd += "\"";
    return sendAT(cmd);
}

bool QuectelEC200U::configureAudioCodecIIC(int mode) {
    String cmd = F("AT+QAUDCFG=\"iic\",");
    cmd += mode;
    return sendAT(cmd);
}

// [C] Data & TCP/IP
bool QuectelEC200U::setTCPMSS(int mss) {
    String cmd = F("AT+QCFG=\"tcp/mss\",");
    cmd += mss;
    return sendAT(cmd);
}

bool QuectelEC200U::setBIPStatusURC(bool enable) {
    String cmd = F("AT+QCFG=\"bip/status\",");
    cmd += enable ? "1" : "0";
    return sendAT(cmd);
}

// [D] System & Hardware
bool QuectelEC200U::setUSBModeCDC() {
    return sendAT(F("AT+QUSBCFG=3,1"));
}

bool QuectelEC200U::configureRIAuto(bool enable) {
    if (enable) return sendAT(F("AT+QCFG=\"urc/ri/ring\",\"auto\""));
    else return sendAT(F("AT+QCFG=\"urc/ri/ring\",\"off\""));
}

bool QuectelEC200U::configureGNSSURC(bool enable) {
    String cmd = F("AT+QGPSCFG=\"urc\",");
    cmd += enable ? "1" : "0";
    return sendAT(cmd);
}



