#include <QuectelEC200U.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  modem.begin();

  Serial.println("Enabling PSM (Power Saving Mode)...");
  if (modem.enablePSM(true))
    Serial.println("PSM enabled");
  else
    Serial.println("Failed to enable PSM");
}

void loop() {}
