#include <QuectelEC200U.h>
// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

// Set the EC200U modem power key pin
#define EC200U_PW_KEY_PIN 10

// Set the EC200U modem power status pin
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial SerialAT(1);
  QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#elif defined(ARDUINO_ARCH_ZEPHYR)
  HardwareSerial& SerialAT = Serial1;
  QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
  #include <SoftwareSerial.h>
  SoftwareSerial SerialAT(10, 11);
  QuectelEC200U modem(SerialAT, 9600, EC200U_RX_PIN, EC200U_TX_PIN);
#endif

#if defined(ARDUINO_ARCH_ESP32)
void EC200U_powerOn() {
  pinMode(EC200U_PW_KEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);

  // Check if the modem is already on
  if (digitalRead(EC200U_STATUS_PIN) == LOW) {
    // Power on the modem
    digitalWrite(EC200U_PW_KEY_PIN, LOW);
    delay(2000);
  }
}
#endif

void setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#elif defined(ARDUINO_ARCH_ZEPHYR)
  SerialAT.begin(115200);
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
