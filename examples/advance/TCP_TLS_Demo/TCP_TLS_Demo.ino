#include <QuectelEC200U.h>
#include "ca_cert.h"

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8);
QuectelEC200U modem(SerialAT);
#endif

const char* cert_path = "tcp_ca.pem";

void setup() {
  Serial.begin(115200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  modem.begin();

  modem.attachData("your.apn");
  modem.activatePDP(1);

  // Upload the CA certificate to the module's filesystem
  // Note: You should use the correct CA certificate for your host
  Serial.println("Uploading CA certificate...");
  if (modem.fsUpload(cert_path, cloudflare_ca_cert)) {
    Serial.println("Certificate uploaded.");
  } else {
    Serial.println("Failed to upload certificate.");
  }

  // Before making a TCP TLS connection, you must configure the SSL context.
  if (modem.sslConfigure(1, cert_path)) {
    Serial.println("SSL context configured.");
  } else {
    Serial.println("Failed to configure SSL context.");
  }

  Serial.println("Opening TCP TLS to www.cloudflare.com:443...");
  int sid = modem.tcpOpen("www.cloudflare.com", 443);
  if (sid >= 0) {
    String req = "GET / HTTP/1.1\r\nHost: www.cloudflare.com\r\nConnection: close\r\n\r\n";
    if (modem.tcpSend(sid, req)) {
      String out;
      if (modem.tcpRecv(sid, out, 1024, 8000)) Serial.println(out);
    }
    modem.tcpClose(sid);
  } else {
    Serial.println("TCP TLS open failed");
  }
}

void loop() {}