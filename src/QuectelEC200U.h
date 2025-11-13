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

// Command history for Ctrl+Z functionality
#define MAX_HISTORY 20
#define MAX_CMD_LENGTH 256

// Modem states
enum ModemState {
  MODEM_UNINITIALIZED,
  MODEM_INITIALIZING,
  MODEM_READY,
  MODEM_ERROR,
  MODEM_NETWORK_CONNECTED,
  MODEM_DATA_READY
};

class QuectelEC200U {
  public:
    // HardwareSerial constructor (auto-configure on begin). On ESP32, optional RX/TX pins are supported.
    QuectelEC200U(HardwareSerial &serial, uint32_t baud = 115200, int8_t rxPin = -1, int8_t txPin = -1);
    // Generic Stream constructor (e.g., SoftwareSerial, USB CDC). Stream must be pre-begun by the caller.
    QuectelEC200U(Stream &stream);

    bool begin(bool forceReinit = false);
    void enableDebug(Stream &debugStream);
    bool sendAT(const String &cmd, const String &expect = "OK", uint32_t timeout = 3000);
    String readResponse(uint32_t timeout);

    // State management
    ModemState getState() const { return _state; }
    bool isInitialized() const { return _initialized; }
    bool isNetworkReady() const { return _networkRegistered; }
    void setState(ModemState state) { _state = state; }

    // Core utilities
    String getIMEI();
    int getSignalStrength();
    bool setAPN(const String &apn);
    String getModemInfo();
    bool factoryReset();
    
    // Network + PDP
    bool waitForNetwork(uint32_t timeoutMs = 60000);
    bool attachData(const String &apn, const String &user = "", const String &pass = "", int auth = 0);
    bool activatePDP(int ctxId = 1);
    bool deactivatePDP(int ctxId = 1);
    int getRegistrationStatus(bool eps = true);
    bool isSimReady();
    String getOperator();
    
    // SMS
    bool sendSMS(const String &number, const String &text);
    String readSMS(int index);
    bool deleteSMS(int index);
    int getSMSCount();
    
    // Voice Call
    bool dial(const String &number);
    bool hangup();
    bool answer();
    String getCallList();
    bool enableCallerId(bool enable);
    
    // HTTP
    bool httpGet(const String &url, String &response);
    bool httpPost(const String &url, const String &data, String &response);
    
    // HTTPS
    bool httpsGet(const String &url, String &response);
    bool httpsPost(const String &url, const String &data, String &response);
    
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
    bool playTTS(const String &text);
    
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
    
    // PSM
    bool enablePSM(bool enable);
    
    // Audio
    bool setSpeakerVolume(int level);
    bool setRingerVolume(int level);
    bool setMicMute(bool mute);
    bool setMicGain(int channel, int level);
    bool setSidetone(bool enable, int level);
    bool setAudioChannel(int channel);
    bool setAudioInterface(const String &params);
    bool audioLoopback(bool enable);
    
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
    String extractQuotedString(const String &response, const String &tag);
    int extractInteger(const String &response, const String &tag);
    bool waitForResponse(const String &expect, uint32_t timeout);
    
  private:
    Stream *_serial;
    Stream *_debugSerial;
    HardwareSerial *_hwSerial;
    uint32_t _baud;
    int8_t _rxPin;
    int8_t _txPin;
    ModemState _state;
    
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
};

#endif