#include <QuectelEC200U_CN.h>
QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  modem.begin();
  String response;
  if (modem.httpGet("http://example.com", response))
    Serial.println("Response: " + response);
}
void loop() {}
