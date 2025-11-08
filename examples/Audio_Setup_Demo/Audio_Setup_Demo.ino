#include <QuectelEC200U.h>
// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 18
#define EC200U_TX_PIN 17

// Set the EC200U modem power key pin
#define EC200U_PW_KEY_PIN 10

// Set the EC200U modem power status pin
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial& SerialAT = Serial2;
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);

void EC200U_powerOn() {
  pinMode(EC200U_PW_KEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);

  // Check if the modem is already on
  if (digitalRead(EC200U_STATUS_PIN) == LOW) {
    // Power on the modem
    digitalWrite(EC200U_PW_KEY_PIN, LOW);
    delay(500);
    digitalWrite(EC200U_PW_KEY_PIN, HIGH);
    delay(3000);
  }
}
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8);
QuectelEC200U modem(SerialAT);
#endif

void setup() {
  Serial.begin(115200);
  delay(200);
#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#else
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
