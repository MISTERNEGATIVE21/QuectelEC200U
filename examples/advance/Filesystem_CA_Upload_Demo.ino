#include <QuectelEC200U.h>

// Filesystem demo: upload a CA file, list directory, read back

QuectelEC200U modem(Serial1, 115200);

const char* CA_PEM = "-----BEGIN CERTIFICATE-----\n...your CA PEM here...\n-----END CERTIFICATE-----\n";

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("Filesystem CA upload demo");

  modem.begin();

  bool ok = modem.fsUpload("ca.pem", CA_PEM);
  Serial.print("Upload: "); Serial.println(ok ? "OK" : "FAIL");

  String list;
  if (modem.fsList(list)) {
    Serial.println("Files:");
    Serial.println(list);
  }

  String content;
  if (modem.fsRead("ca.pem", content, 1024)) {
    Serial.println("Read ca.pem:");
    Serial.println(content);
  }
}

void loop() {}
