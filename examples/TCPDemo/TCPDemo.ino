#include <QuectelEC200U.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
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