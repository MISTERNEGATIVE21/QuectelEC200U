#include <QuectelEC200U_CN.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
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
