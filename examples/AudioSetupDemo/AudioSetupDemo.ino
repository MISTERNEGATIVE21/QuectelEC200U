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

void setup() {
  Serial.begin(115200);
  delay(200);
#if !defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(9600);
#endif
  modem.begin();

  Serial.println("Configuring audio...");
  modem.setAudioChannel(2);      // loudspeaker
  modem.setSpeakerVolume(80);    // 0..100
  modem.setMicMute(false);
  modem.setMicGain(0, 8);        // main mic, 0..15
  modem.setSidetone(true, 5);    // enable sidetone
  modem.setRingerVolume(70);

  Serial.println("Enabling loopback for test (5s)...");
  modem.audioLoopback(true);
  delay(5000);
  modem.audioLoopback(false);
  Serial.println("Audio test complete.");
}

void loop() {}
