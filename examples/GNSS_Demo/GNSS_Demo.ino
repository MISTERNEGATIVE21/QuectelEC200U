#include <QuectelEC200U.h>
// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

// Set the EC200U modem power key pin
#define EC200U_PW_KEY_PIN 10

// Set the EC200U modem power status pin
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)


#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial& SerialAT = Serial1;
#else
  #include <SoftwareSerial.h>
  SoftwareSerial SerialAT(10, 11);
#endif
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);

void EC200U_powerOn() {
  pinMode(EC200U_PW_KEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);

  // Check if the modem is already on
  if (digitalRead(EC200U_STATUS_PIN) == LOW) {
    // Power on the modem
    // Power on the modem
    digitalWrite(EC200U_PW_KEY_PIN, LOW);
    delay(2000);
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

  Serial.println("Starting GNSS...");
  if (modem.startGNSS()) {
    delay(2000); // wait briefly for a fix (demo)
    Serial.println("Reading GNSS location...");
    Serial.println(modem.getGNSSLocation());
  } else {
    Serial.println("Failed to start GNSS");
  }
}

void loop() {
}
