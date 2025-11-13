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
  delay(200);
#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#else
  SerialAT.begin(9600);
#endif
  modem.begin();

  // Enable caller ID notifications
  modem.enableCallerId(true);

  // Dial a number (replace with a valid number)
  Serial.println("Dialing...");
  if (modem.dial("+1234567890")) {
    Serial.println("Dial command sent. Hanging up in 5 seconds...");
    delay(5000);
    modem.hangup();
  } else {
    Serial.println("Dial failed");
  }

  // Print current calls (if any)
  Serial.println(modem.getCallList());
}

void loop() {}
