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
SoftwareSerial SerialAT(7, 8);
QuectelEC200U modem(SerialAT);
#endif

const char* APN = "your.apn";

void setup() {
  Serial.begin(115200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  delay(500);
  Serial.println("Raw SSL socket demo");

  modem.begin();
  modem.attachData(APN);
  modem.activatePDP(1);

  // Assume SSL ctx 1 already configured (see HTTPS_TLS_Demo)
  // QSSLOPEN=<ctxId>,<sslCtxId>,<host>,<port>,<access_mode>
  if (!modem.sendCommand("AT+QSSLOPEN=1,1,\"example.com\",443,2", "OK", 5000)) {
    Serial.println("QSSLOPEN cmd failed");
    return;
  }
  // Wait URC for open result (socket 0 assumed by FW)
  String urc = modem.readResponse(15000);
  Serial.println(urc);

  // Send a TLS ClientHello (for demo we send an HTTPS request; real TLS requires proper handshake handled by modem)
  String http = String("GET / HTTP/1.1\r\nHost: example.com\r\nConnection: close\r\n\r\n");
  String cmd = String("AT+QSSLSEND=0,") + http.length();
  if (!modem.sendCommand(cmd, ">", 2000)) {
    Serial.println("QSSLSEND prompt failed");
    return;
  }
  Serial1.print(http);
  String r = modem.readResponse(8000);
  Serial.println(r);

  // Read some bytes
  modem.sendCommand("AT+QSSLRECV=0,1024");
  String data = modem.readResponse(10000);
  Serial.println(data);

  modem.sendCommand("AT+QSSLCLOSE=0");
}

void loop() {}
