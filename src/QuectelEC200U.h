/*
  QuectelEC200U_CN - Arduino library for Quectel EC200U (CN-AA)
  Author: misternegative21
  Maintainer: MisterNegative21 <misternegative21@gmail.com>
  Repository: https://github.com/misternegative21Quaanctelec200u
  License: MIT (see LICENSE)

  Quectel and EC200U are trademarks of Quectel Wireless Solutions Co., Ltd.
  This library is an independent, unofficial project and is not affiliated with or endorsed by Quectel.
*/

#ifndef QUECTEL_EC200U_H
#define QUECTEL_EC200U_H

#include <Arduino.h>

class QuectelEC200U {
  public:
    QuectelEC200U(HardwareSerial &serial, uint32_t baud = 115200);

    void begin();
    bool sendCommand(const String &cmd, const String &expect = "OK", uint32_t timeout = 1000);
    String readResponse(uint32_t timeout);

    // ===== Core =====
    String getIMEI();
    int getSignalStrength();
    bool setAPN(const String &apn);

    // Network + PDP
    bool waitForNetwork(uint32_t timeoutMs = 60000);
    bool attachData(const String &apn, const String &user = "", const String &pass = "", int auth = 0);
    bool activatePDP(int ctxId = 1);
    bool deactivatePDP(int ctxId = 1);
    int  getRegistrationStatus(bool eps = true);
    bool isSimReady();

    // ===== SMS =====
    bool sendSMS(const String &number, const String &text);
    String readSMS(int index);

    // ===== HTTP =====
    bool httpGet(const String &url, String &response);
    bool httpPost(const String &url, const String &data, String &response);
    
    // ===== HTTPS =====
    bool httpsGet(const String &url, String &response);
    bool httpsPost(const String &url, const String &data, String &response);

    // ===== TCP sockets =====
    int  tcpOpen(const String &host, int port, int ctxId = 1, int socketId = 0);
    bool tcpSend(int socketId, const String &data);
    bool tcpRecv(int socketId, String &out, size_t bytes = 512, uint32_t timeout = 5000);
    bool tcpClose(int socketId);

    // ===== USSD =====
    bool sendUSSD(const String &code, String &response);

    // ===== NTP / Clock =====
    bool ntpSync(const String &server = "pool.ntp.org", int timezone = 0);
    String getClock();
    bool setClock(const String &datetime); // "YY/MM/DD,HH:MM:SS+TZ"

    // ===== GNSS =====
    bool startGNSS();
    bool stopGNSS();
    bool isGNSSOn();
    bool setGNSSConfig(const String &item, const String &value);
    String getNMEASentence(const String &type = "RMC");
    String getGNSSLocation();
    String getGNSSLocation(uint32_t fixWaitMs);

    // ===== TTS =====
    bool playTTS(const String &text);

    // ===== FTP =====
    bool ftpLogin(const String &server, const String &user, const String &pass);
    bool ftpDownload(const String &filename, String &data);

    // ===== Filesystem =====
    bool fsList(String &out);
    bool fsUpload(const String &path, const String &content);
    bool fsRead(const String &path, String &out, size_t length = 0);
    bool fsDelete(const String &path);

    // ===== SSL/TLS =====
    bool sslConfigure(int ctxId, const String &caPath, bool verify = true);

    // ===== PSM =====
    bool enablePSM(bool enable);

  private:
    HardwareSerial *_serial;
    uint32_t _baud;
    void flushInput(); // drain any pending input
    bool expectURC(const String &tag, uint32_t timeout);
};

#endif
