#include <QuectelEC200U.h>
// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
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
SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
QuectelEC200U modem(SerialAT);
#endif


void setup() {
  Serial.begin(115200);
#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#else
  SerialAT.begin(9600);
#endif
  modem.begin();

  String response;
  if (modem.httpsGet("https://example.com", response)) {
    Serial.println("HTTPS Response:");
    Serial.println(response);
  } else {
    Serial.println("HTTPS request failed");
  }
}

void loop() {}