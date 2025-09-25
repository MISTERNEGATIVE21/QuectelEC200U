#include <QuectelEC200U.h>

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
#ifndef AT_RX_PIN
#define AT_RX_PIN 16
#endif
#ifndef AT_TX_PIN
#define AT_TX_PIN 17
#endif
QuectelEC200U modem(SerialAT, 115200, AT_RX_PIN, AT_TX_PIN);
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
  // On ESP32 the modem will auto-begin the UART with pins in modem.begin(); others: start SoftwareSerial here
#if !defined(ARDUINO_ARCH_ESP32)
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
