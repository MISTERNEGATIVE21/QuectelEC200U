#include <QuectelEC200U.h>
// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

// Set the EC200U modem power key pin
#define EC200U_PW_KEY_PIN 10

// Set the EC200U modem power status pin
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)


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

void EC200U_powerOn() {
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

  if (!modem.isSimReady()) Serial.println("SIM not ready");
  if (!modem.waitForNetwork()) Serial.println("No network");

  // Set APN and bring up PDP
  modem.attachData("your.apn.here");
  modem.activatePDP(1);

  int sock = modem.tcpOpen("example.com", 80, 1, 0);
  if (sock >= 0) {
    modem.tcpSend(sock, "GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n");
    String body;
    if (modem.tcpRecv(sock, body, 1024, 8000)) {
      Serial.println(body);
    }
    modem.tcpClose(sock);
  } else {
    Serial.println("TCP open failed");
  }
}

void loop() {}