/*
  QuectelEC200U_CN - Arduino library for Quectel EC200U (CN-AA)
  Author: misternegative21
  Maintainer: MisterNegative21 <misternegative21@gmail.com>
  Repository: https://github.com/MISTERNEGATIVE21/QuectelEC200U
  License: MIT (see LICENSE)

  Quectel and EC200U are trademarks of Quectel Wireless Solutions Co., Ltd.
  This library is an independent, unofficial project and is not affiliated with or endorsed by Quectel.
*/

#ifndef QUECTEL_EC200U_ADV_H
#define QUECTEL_EC200U_ADV_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Command history for Ctrl+Z functionality
#define MAX_HISTORY 20
#define MAX_CMD_LENGTH 256
#define HTTP_URL_CHUNK_SIZE 2048

// Modem states
enum ModemState {
  MODEM_UNINITIALIZED,
  MODEM_INITIALIZING,
  MODEM_READY,
  MODEM_ERROR,
  MODEM_NETWORK_CONNECTED,
  MODEM_DATA_READY
};

enum class ErrorCode {
  NONE = 0,
  UNKNOWN = -1,
  MODEM_NOT_RESPONDING = -2,
  SIM_NOT_READY = -3,
  SIGNAL_QUALITY_LOW = -4,
  GPRS_NOT_ATTACHED = -5,
  APN_CONFIG_FAILED = -6,
  AUTH_CONFIG_FAILED = -7,
  PDP_ACTIVATION_FAILED = -8,
  HTTP_ERROR = -10,
  HTTP_CONTEXT_ID_FAILED = -11,
  HTTP_SSL_CONTEXT_ID_FAILED = -12,
  HTTP_URL_FAILED = -13,
  HTTP_URL_WRITE_FAILED = -14,
  HTTP_POST_FAILED = -15,
  HTTP_POST_DATA_WRITE_FAILED = -16,
  HTTP_POST_URC_FAILED = -17,
  HTTP_GET_FAILED = -18,
  HTTP_GET_URC_FAILED = -19,
  HTTP_READ_FAILED = -20,
  FTP_ERROR = -30,
  MQTT_ERROR = -40,
  TCP_ERROR = -50,
  SSL_ERROR = -60,
  FS_ERROR = -70,
};

class QuectelEC200U_Adv {
  public:
    // HardwareSerial constructor (auto-configure on begin). On ESP32, optional RX/TX pins are supported.
    QuectelEC200U(HardwareSerial &serial, uint32_t baud = 115200, int8_t rxPin = -1, int8_t txPin = -1);
    // Generic Stream constructor (e.g., SoftwareSerial, USB CDC). Stream must be pre-begun by the caller.
    QuectelEC200U(Stream &stream);

    bool begin(bool forceReinit = false);
    void enableDebug(Stream &debugStream);
    bool sendAT(const String &cmd, const String &expect = "OK", uint32_t timeout = 3000);
    [[deprecated("Use readResponse(char*, size_t, uint32_t) instead")]] String readResponse(uint32_t timeout);
    int readResponse(char* buffer, size_t length, uint32_t timeout);

    // State management
    ModemState getState() const { return _state; }
    bool isInitialized() const { return _initialized; }
    bool isNetworkReady() const { return _networkRegistered; }
    void setState(ModemState state) { _state = state; }

    // Core utilities
    String getIMEI();
    int getSignalStrength();
    bool setAPN(const char* apn);
    String getModemInfo();
    bool factoryReset();
    [[deprecated("Use begin() instead")]] bool modem_init();
    
    // Network + PDP
    bool waitForNetwork(uint32_t timeoutMs = 60000);
    bool attachData(const char* apn, const char* user = "", const char* pass = "", int auth = 0);
    bool activatePDP(int ctxId = 1);
    bool deactivatePDP(int ctxId = 1);
    int getRegistrationStatus(bool eps = true);
    bool isSimReady();
    String getOperator();
    
    // SMS
    bool sendSMS(const char* number, const char* text);
    String readSMS(int index);
    bool deleteSMS(int index);
    int getSMSCount();
    
    // Voice Call
    bool dial(const char* number);
    bool hangup();
    bool answer();
    String getCallList();
    bool enableCallerId(bool enable);
    
    // HTTP
    bool httpGet(const String &url, String &response, String headers[] = nullptr, size_t header_size = 0);
    bool httpPost(const String &url, const String &data, String &response, String headers[] = nullptr, size_t header_size = 0);
    bool httpPost(const String &url, const JsonDocument &json, String &response, String headers[] = nullptr, size_t header_size = 0);
    
    // HTTPS
    bool httpsGet(const String &url, String &response, String headers[] = nullptr, size_t header_size = 0);
    bool httpsPost(const String &url, const String &data, String &response, String headers[] = nullptr, size_t header_size = 0);
    bool httpsPost(const String &url, const JsonDocument &json, String &response, String headers[] = nullptr, size_t header_size = 0);

    // Error handling
    ErrorCode getLastError();
    String getLastErrorString();
    
    // MQTT
    bool mqttConnect(const String &server, int port);
    bool mqttPublish(const String &topic, const String &message);
    bool mqttSubscribe(const String &topic);
    bool mqttDisconnect();
    
    // TCP sockets
    int tcpOpen(const String &host, int port, int ctxId = 1, int socketId = 0);
    bool tcpSend(int socketId, const String &data);
    bool tcpRecv(int socketId, String &out, size_t bytes = 512, uint32_t timeout = 5000);
    bool tcpClose(int socketId);
    
    // USSD
    bool sendUSSD(const String &code, String &response);
    
    // NTP / Clock
    bool ntpSync(const String &server = "pool.ntp.org", int timezone = 0);
    String getClock();
    bool setClock(const String &datetime);
    
    // GNSS
    bool startGNSS();
    bool stopGNSS();
    bool isGNSSOn();
    bool setGNSSConfig(const String &item, const String &value);
    String getNMEASentence(const String &type = "RMC");
    String getGNSSLocation();
    String getGNSSLocation(uint32_t fixWaitMs);
    
    // TTS
    bool playTTS(const char* text);
    
    // FTP
    bool ftpLogin(const String &server, const String &user, const String &pass);
    bool ftpDownload(const String &filename, String &data);
    bool ftpLogout();
    
    // Filesystem
    bool fsList(String &out);
    bool fsUpload(const String &path, const String &content);
    bool fsRead(const String &path, String &out, size_t length = 0);
    bool fsDelete(const String &path);
    bool fsExists(const String &path);
    
    // SSL/TLS
    bool sslConfigure(int ctxId, const String &caPath, bool verify = true);
    bool sslUploadCert(const String &cert, const String &path);
    
    // PSM
    bool enablePSM(bool enable);
    
    // Command history (Ctrl+Z support)
    void addToHistory(const String &cmd);
    String getFromHistory(int index);
    String getPreviousCommand();
    String getNextCommand();
    int getHistoryCount() const { return _historyCount; }
    void clearHistory();
    
    // Power management
    bool powerOff();
    bool reboot();
    
    // Utility functions
    String extractQuotedString(const char* response, const String &tag);
    int extractInteger(const char* response, const String &tag);
    bool waitForResponse(const String &expect, uint32_t timeout);
    bool parseJson(const String &jsonString, JsonDocument &doc);
    
    // Ping
    bool ping(const String &host, int contextID = 1, int timeout = 4, int pingnum = 4);
    
    // NTP
    bool ntpSync(const String &server, int contextID = 1, int port = 123);

    // DNS
    bool setDNS(const String &primary, const String &secondary, int contextID = 1);
    String getIpByHostName(const String &hostname, int contextID = 1);

    // ADC
    int readADC();

    // Struct to hold PDP Context information
    struct PDPContext {
        int cid;
        String pdp_type;
        String apn;
        String p_addr;
        String dns_p;
        String dns_s;
        // Add other fields as necessary from the full AT+CGDCONT response
    };

    // Packet Domain
    String getPacketDataCounter();
    String readDynamicPDNParameters(int cid = 1);
    PDPContext getPDPContext(int cid = 1);

    // Audio
    bool setSpeakerVolume(int level);
    bool setRingerVolume(int level);
    bool setMicMute(bool mute);
    bool setMicGain(int channel, int level);
    bool setSidetone(bool enable, int level);
    bool setAudioChannel(int channel);
    bool setAudioInterface(const String &params);
    bool audioLoopback(bool enable);

    // Hardware
    String getBatteryCharge();
    String getWifiScan();
    
    // Advanced TCP/IP
    bool switchDataAccessMode(int connectID, int accessMode);
    bool echoSendData(bool enable);

    // QCFG - Extended settings
    bool setNetworkScanMode(int mode);
    bool setBand(const String &gsm_mask, const String &lte_mask);

    // Modem Identification
    String getManufacturerIdentification();
    String getModelIdentification();
    String getFirmwareRevision();

    // General Commands
    bool restoreFactoryDefaults();
    String showCurrentConfiguration();
    bool storeConfiguration(int profile = 0);
    bool restoreConfiguration(int profile = 0);
    bool setResultCodeEcho(bool enable);
    bool setResultCodeFormat(bool verbose);
    bool setCommandEcho(bool enable);
    bool repeatPreviousCommand();
    bool setSParameter(int s, int value);
    bool setFunctionMode(int fun, int rst = 0);
    bool setErrorMessageFormat(int format);
    bool setTECharacterSet(const String &chset);
    bool setURCOutputRouting(const String &port);

    // UART Control Commands
    bool setDCDFunctionMode(int mode);
    bool setDTRFunctionMode(int mode);
    bool setUARTFlowControl(int dce_by_dte, int dte_by_dce);
    bool setUARTFrameFormat(int format, int parity);
    bool setUARTBaudRate(long rate);

    // Status Control and Extended Settings
    String getActivityStatus();
    bool setURCIndication(const String &urc_type, bool enable);


    // Advanced Error Reporting and SIM
    String getExtendedErrorReports();
    String getSIMStatus();


    
    
    
    
    
    
    
  private:
    Stream *_serial;
    Stream *_debugSerial;
    HardwareSerial *_hwSerial;
    uint32_t _baud;
    int8_t _rxPin;
    int8_t _txPin;
    ModemState _state;
    ErrorCode _lastError;
    
    // Command history
    String _cmdHistory[MAX_HISTORY];
    int _historyCount;
    int _historyIndex;
    
    // Initialization flags
    bool _initialized;
    bool _echoDisabled;
    bool _simChecked;
    bool _networkRegistered;
    
    void flushInput();
    bool expectURC(const String &tag, uint32_t timeout);
    bool initializeModem();
    void logDebug(const String &msg);
    void logError(const String &msg);
    void updateNetworkStatus();
    void _sendHttpHeaders(String headers[], size_t header_size);
    bool _sendHttpRequest(const String &url, const String &data, String &response, String headers[], size_t header_size, bool ssl, bool isPost);
    String _getSignalStrengthString(int signal);
    String _getRegistrationStatusString(int regStatus);
    int _parseCsvInt(const String& response, const String& tag, int index);
};

#endif