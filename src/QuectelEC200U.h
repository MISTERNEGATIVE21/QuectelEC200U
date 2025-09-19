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

    // ===== SMS =====
    bool sendSMS(const String &number, const String &text);
    String readSMS(int index);

    // ===== HTTP =====
    bool httpGet(const String &url, String &response);
    bool httpPost(const String &url, const String &data, String &response);
    
    // ===== HTTPS =====
    bool httpsGet(const String &url, String &response);
    bool httpsPost(const String &url, const String &data, String &response);

    // ===== MQTT =====
    bool mqttConnect(const String &server, int port);
    bool mqttPublish(const String &topic, const String &message);
    bool mqttSubscribe(const String &topic);

    // ===== GNSS =====
    bool startGNSS();
    String getGNSSLocation();

    // ===== TTS =====
    bool playTTS(const String &text);

    // ===== FTP =====
    bool ftpLogin(const String &server, const String &user, const String &pass);
    bool ftpDownload(const String &filename, String &data);

    // ===== PSM =====
    bool enablePSM(bool enable);

  private:
    HardwareSerial *_serial;
    uint32_t _baud;
};

#endif
