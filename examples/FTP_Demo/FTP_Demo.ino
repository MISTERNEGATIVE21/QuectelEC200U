#include <QuectelEC200U.h>
#if defined(ARDUINO_ARCH_ESP32)
#include <EC200U_ESP32_Config.h>
HardwareSerial& SerialAT = EC200U_UART; // Serial2
QuectelEC200U modem(SerialAT, 115200, EC200U_RX, EC200U_TX);
#else
#include <SoftwareSerial.h>
#ifndef AT_RX_PIN
#define AT_RX_PIN 7
#endif
#ifndef AT_TX_PIN
#define AT_TX_PIN 8
#endif
SoftwareSerial SerialAT(AT_RX_PIN, AT_TX_PIN);
QuectelEC200U modem(SerialAT);
#endif

void setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#else
  SerialAT.begin(9600);
#endif
  modem.begin();

  Serial.println("Logging into FTP...");
  if (modem.ftpLogin("ftp.example.com", "user", "pass")) {
    String fileData;
    if (modem.ftpDownload("test.txt", fileData))
      Serial.println("Downloaded file: " + fileData);
    else
      Serial.println("FTP download failed");
  } else {
    Serial.println("FTP login failed");
  }
}

void loop() {}
