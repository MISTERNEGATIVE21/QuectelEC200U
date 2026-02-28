/*
  QuectelEC200U_CN - Arduino library for Quectel EC200U (CN-AA)
  Author: misternegative21
  Maintainer: MisterNegative21 <misternegative21@gmail.com>
  Repository: https://github.com/MISTERNEGATIVE21/QuectelEC200U
  License: MIT (see LICENSE)

  Quectel and EC200U are trademarks of Quectel Wireless Solutions Co., Ltd.
  This library is an independent, unofficial project and is not affiliated with or endorsed by Quectel.
*/

#ifndef QUECTEL_EC200U_H
#define QUECTEL_EC200U_H

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

class QuectelEC200U {
  public:
    // HardwareSerial constructor (auto-configure on begin). On ESP32, optional RX/TX pins are supported.
    QuectelEC200U(HardwareSerial &serial, uint32_t baud = 115200, int8_t rxPin = -1, int8_t txPin = -1);
    
    // Generic Stream constructor (for SoftwareSerial or other streams)
    QuectelEC200U(Stream &stream);

    bool begin(bool forceReinit = false);
    
    // Power Management
    void powerOn(int pin);

    // Core Communication
    bool sendAT(const char* cmd);
    inline bool sendAT(const String &cmd) { return sendAT(cmd.c_str()); }
    
    bool sendAT(const char* cmd, const char* expect, uint32_t timeout = 1000);
    inline bool sendAT(const String &cmd, const String &expect, uint32_t timeout = 1000) { return sendAT(cmd.c_str(), expect.c_str(), timeout); }
    
    void sendATRaw(const char* cmd);
    inline void sendATRaw(const String &cmd) { sendATRaw(cmd.c_str()); }
    
    String readResponse(uint32_t timeout = 1000);
    int readResponse(char* buffer, size_t length, uint32_t timeout);
    
    bool sendCommand(const char* cmd, const char* expected, uint32_t timeout = 1000);
    inline bool sendCommand(const String &cmd, const String &expected, uint32_t timeout = 1000) { return sendCommand(cmd.c_str(), expected.c_str(), timeout); }
    void enableDebug(Stream &debugSerial);

    // Advanced Features
    bool switchSimCard();
    bool toggleISIM(bool enable);
    bool setDSDSMode(bool enable);
    bool blockIncomingCalls(bool enable);
    bool setUSBModeCDC();

    // Advanced Network
    String getOperatorName();
    bool preventNetworkModeSwitch(bool enable);

    // Advanced Audio
    bool playAudioDuringCall(const char* filename);
    bool configureAudioCodecIIC(int mode);
    
    // WiFi and Bluetooth
    String getWifiScan();
    String scanBluetooth();
    
    // Audio/Volume Control
    bool setSpeakerVolume(int level);
    bool setRingerVolume(int level);
    bool setMicMute(bool mute);
    bool setMicGain(int channel, int level);
    bool setSidetone(bool enable, int level);
    bool setAudioChannel(int channel);
    bool setAudioInterface(const char* params);
    inline bool setAudioInterface(const String &params) { return setAudioInterface(params.c_str()); }


    // Advanced Data
    bool setTCPMSS(int mss);
    bool setBIPStatusURC(bool enable);
    bool switchDataAccessMode(int connectID, int accessMode);
    bool echoSendData(bool enable);

    // Advanced System
    bool configureRIAuto(bool enable);
    bool configureGNSSURC(bool enable);
    
    // Network/Band Control
    bool setNetworkScanMode(int mode);
    bool setBand(const char* gsm_mask, const char* lte_mask);
    inline bool setBand(const String &gsm_mask, const String &lte_mask) { return setBand(gsm_mask.c_str(), lte_mask.c_str()); }



    int getSignalStrength();
    String getBatteryCharge();
    bool setAPN(const char* apn);
    inline bool setAPN(const String &apn) { return setAPN(apn.c_str()); }

    String getModemInfo();
    bool factoryReset();
    bool restoreFactoryDefaults();
    String showCurrentConfiguration();
    bool storeConfiguration(int profile);
    bool restoreConfiguration(int profile);
    bool setResultCodeEcho(bool enable);
    bool setResultCodeFormat(bool verbose);
    bool setCommandEcho(bool enable);
    bool repeatPreviousCommand();
    bool setSParameter(int s, int value);
    bool setFunctionMode(int fun, int rst);
    bool setErrorMessageFormat(int format);
    bool setTECharacterSet(const char* chset);
    bool setURCOutputRouting(const char* port);
    
    // UART Control
    bool setDCDFunctionMode(int mode);
    bool setDTRFunctionMode(int mode);
    bool setUARTFlowControl(int dce_by_dte, int dte_by_dce);
    bool setUARTFrameFormat(int format, int parity);
    bool setUARTBaudRate(long rate);
    
    // Status
    String getActivityStatus();
    bool setURCIndication(const char* urc_type, bool enable);
    inline bool setURCIndication(const String &urc_type, bool enable) { return setURCIndication(urc_type.c_str(), enable); }


    [[deprecated("Use begin() instead")]] bool modem_init();
    
    // Network + PDP
    bool waitForNetwork(uint32_t timeoutMs = 60000);
    bool attachData(const char* apn, const char* user = "", const char* pass = "", int auth = 0);
    inline bool attachData(const String &apn, const String &user = String(""), const String &pass = String(""), int auth = 0) { return attachData(apn.c_str(), user.c_str(), pass.c_str(), auth); }

    bool activatePDP(int ctxId = 1);
    bool deactivatePDP(int ctxId = 1);
    bool activatePDPAsync(int ctxId = 1);
    bool deactivatePDPAsync(int ctxId = 1);
    bool configureContext(int ctxId, int type, const char* apn, const char* user, const char* pass, int auth);
    inline bool configureContext(int ctxId, int type, const String &apn, const String &user, const String &pass, int auth) { return configureContext(ctxId, type, apn.c_str(), user.c_str(), pass.c_str(), auth); }

    int getRegistrationStatus(bool eps = true);
    bool isSimReady();
    String getOperator();
    String getIMEI();
    String getManufacturerIdentification();
    String getModelIdentification();
    String getFirmwareRevision();
    String getModuleVersion();
    bool sendSMS(const char* number, const char* text);
    inline bool sendSMS(const String &number, const String &text) { return sendSMS(number.c_str(), text.c_str()); }

    String readSMS(int index);
    bool deleteSMS(int index);
    int getSMSCount();
    
    // Voice Call
    bool dial(const char* number);
    inline bool dial(const String &number) { return dial(number.c_str()); }

    bool hangup();
    bool answer();
    String getCallList();
    bool enableCallerId(bool enable);
    
    // HTTP
    bool httpGet(const char* url, String &response, String headers[] = nullptr, size_t header_size = 0);
    bool httpPost(const char* url, const char* data, String &response, String headers[] = nullptr, size_t header_size = 0);
    bool httpPost(const char* url, const JsonDocument &json, String &response, String headers[] = nullptr, size_t header_size = 0);
    
    // HTTPS
    bool httpsGet(const char* url, String &response, String headers[] = nullptr, size_t header_size = 0);
    bool httpsPost(const char* url, const char* data, String &response, String headers[] = nullptr, size_t header_size = 0);
    bool httpsPost(const char* url, const JsonDocument &json, String &response, String headers[] = nullptr, size_t header_size = 0);

    // Error handling
    ErrorCode getLastError();
    String getLastErrorString();
    
    // MQTT
    bool mqttConnect(const char* server, int port);
    inline bool mqttConnect(const String &server, int port) { return mqttConnect(server.c_str(), port); }
    bool mqttPublish(const char* topic, const char* message);
    inline bool mqttPublish(const String &topic, const String &message) { return mqttPublish(topic.c_str(), message.c_str()); }
    bool mqttSubscribe(const char* topic);
    inline bool mqttSubscribe(const String &topic) { return mqttSubscribe(topic.c_str()); }

    bool mqttDisconnect();
    
    // TCP sockets
    int tcpOpen(const char* host, int port, int ctxId = 1, int socketId = 0);
    inline int tcpOpen(const String &host, int port, int ctxId = 1, int socketId = 0) { return tcpOpen(host.c_str(), port, ctxId, socketId); }
    bool tcpSend(int socketId, const char* data);
    inline bool tcpSend(int socketId, const String &data) { return tcpSend(socketId, data.c_str()); }

    bool tcpRecv(int socketId, String &out, size_t bytes = 512, uint32_t timeout = 5000);
    bool tcpClose(int socketId);
    
    // USSD
    bool sendUSSD(const char* code, String &response);
    
    String getClock();
    bool setClock(const char* datetime);
    
    // GNSS
    bool startGNSS();
    bool stopGNSS();
    bool isGNSSOn();
    bool setGNSSConfig(const char* item, const char* value);
    String getNMEASentence(const char* type = "RMC");
    String getGNSSLocation();
    String getGNSSLocation(uint32_t fixWaitMs);
    
    // GNSS Data Struct
    struct GNSSData {
        String utc_time;
        String lat;
        String lon;
        String hdop;
        String altitude;
        String fix;
        String cog;
        String spkm;
        String spkn;
        String date;
        String nsat;
        bool valid;
    };
    GNSSData getGNSSData();
    GNSSData getGNSSData(uint32_t fixWaitMs);
    
    // TTS
    bool playTTS(const char* text);
    inline bool playTTS(const String &text) { return playTTS(text.c_str()); }

    
    // FTP
    bool ftpLogin(const char* server, const char* user, const char* pass);
    inline bool ftpLogin(const String &server, const String &user, const String &pass) { return ftpLogin(server.c_str(), user.c_str(), pass.c_str()); }

    bool ftpDownload(const char* filename, String &data);
    bool ftpLogout();
    
    // Filesystem
    bool fsList(String &out);
    bool fsUpload(const char* path, const char* content);
    inline bool fsUpload(const String &path, const String &content) { return fsUpload(path.c_str(), content.c_str()); }
    bool fsRead(const char* path, String &out, size_t length = 0);
    inline bool fsRead(const String &path, String &out, size_t length = 0) { return fsRead(path.c_str(), out, length); }
    bool fsDelete(const char* path);
    inline bool fsDelete(const String &path) { return fsDelete(path.c_str()); }
    bool fsExists(const char* path);
    inline bool fsExists(const String &path) { return fsExists(path.c_str()); }

    
    // SSL/TLS
    bool sslConfigure(int ctxId, const char* caPath, bool verify = true);
    inline bool sslConfigure(int ctxId, const String &caPath, bool verify = true) { return sslConfigure(ctxId, caPath.c_str(), verify); }
    bool sslUploadCert(const char* cert, const char* path);
    inline bool sslUploadCert(const String &cert, const String &path) { return sslUploadCert(cert.c_str(), path.c_str()); }

    
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
    bool reboot();
    bool powerOff();
    
    // Utility functions
    String extractQuotedString(const char* response, const char* tag);
    inline String extractQuotedString(const char* response, const String &tag) { return extractQuotedString(response, tag.c_str()); }
    
    int extractInteger(const char* response, const char* tag);
    inline int extractInteger(const char* response, const String &tag) { return extractInteger(response, tag.c_str()); }
    bool waitForResponse(const char* expect, uint32_t timeout);
    inline bool waitForResponse(const String &expect, uint32_t timeout) { return waitForResponse(expect.c_str(), timeout); }
    bool parseJson(const String &jsonString, JsonDocument &doc);
    
    // Ping
    bool ping(const char* host, int contextID = 1, int timeout = 4, int pingnum = 4);
    inline bool ping(const String &host, int contextID = 1, int timeout = 4, int pingnum = 4) { return ping(host.c_str(), contextID, timeout, pingnum); }
    bool ping(const char* host, String &report, int contextID = 1, int timeout = 4, int pingnum = 4);
    inline bool ping(const String &host, String &report, int contextID = 1, int timeout = 4, int pingnum = 4) { return ping(host.c_str(), report, contextID, timeout, pingnum); }

    
    // NTP
    bool ntpSync(const char* server, int timezone, int contextID = 1, int port = 123);

    // DNS
    bool setDNS(const char* primary, const char* secondary, int contextID = 1);
    String getIpByHostName(const char* hostname, int contextID = 1);

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
    String getDetailedSignalQuality();
    String getNetworkTime();
    String getNetworkInfo();

    // Call-Related Commands
    bool setVoiceHangupControl(int mode);
    bool hangupVoiceCall();
    bool setConnectionTimeout(int seconds);

    // Phonebook Commands
    String getSubscriberNumber();
    String findPhonebookEntries(const char* findtext);
    String readPhonebookEntry(int index1, int index2 = -1);
    bool selectPhonebookStorage(const char* storage);
    bool writePhonebookEntry(int index, const char* number, const char* text, int type = 129);

    // SMS Commands
    bool setMessageFormat(int mode);
    bool setServiceCenterAddress(const char* sca);
    String listMessages(const char* stat);
    bool setNewMessageIndication(int mode, int mt, int bm, int ds, int bfr);

    // Packet Domain Commands
    bool gprsAttach(bool attach);
    bool setGPRSClass(const char* gprs_class);
    bool setPacketDomainEventReporting(int mode);

    // Supplementary Service Commands
    bool setCallForwarding(int reason, int mode, const char* number, int time = 20);
    bool setCallWaiting(int mode);
    bool setCallingLineIdentificationPresentation(bool enable);
    bool setCallingLineIdentificationRestriction(int mode);

    // More Audio Commands
    bool recordAudio(const char* filename);
    bool playAudio(const char* filename);
    bool stopAudio();
    bool playTextToSpeech(const char* text);

    // Remaining TCP/IP Commands
    bool sendHexData(int connectID, const char* hex_string);


    // Advanced Error Reporting and SIM
    String getExtendedErrorReports();
    String getSIMStatus();
    String getIMSI();
    String getICCID();
    String getPinRetries();

    // Advanced TCP/IP Configuration
    bool setTCPConfig(const char* param, const char* value);
    String getSocketStatus(int connectID);
    int getTCPError();
    
    // General Modem Configuration
    bool setModemConfig(const char* param, const char* value);
    
    
    
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
    bool expectURC(const char* tag, uint32_t timeout);
    inline bool expectURC(const String &tag, uint32_t timeout) { return expectURC(tag.c_str(), timeout); }
    bool initializeModem();
    void logDebug(const String &msg);
    void logError(const String &msg);
    void updateNetworkStatus();
    void _sendHttpHeaders(String headers[], size_t header_size);
    bool _sendHttpRequest(const String &url, const String &data, String &response, String headers[], size_t header_size, bool ssl, bool isPost);
    String _collectResponse(uint32_t timeout);
    bool _extractHttpPayload(const String &raw, String &payload);
    String _getSignalStrengthString(int signal);
    String _getRegistrationStatusString(int regStatus);
    int _parseCsvInt(const String& response, const String& tag, int index);
    String _parseCsvString(const String& response, const String& tag, int index);
    String _extractFirstLine(const String &resp) const;
};

#endif