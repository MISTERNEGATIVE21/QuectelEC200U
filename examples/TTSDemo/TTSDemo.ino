#include <QuectelEC200U.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  modem.begin();

  Serial.println("Playing TTS...");
  if (modem.playTTS("Hello, this is EC200U speaking"))
    Serial.println("TTS command sent");
  else
    Serial.println("TTS failed");
}

void loop() {}
