/*
  Quectel EC200U Advanced Features Demo
  
  This example demonstrates the advanced features from the Developer Implementation Guide:
  - SIM Control (Switch SIM, Toggle ISIM, DSDS)
  - Audio & Voice (Block Incoming, Play Audio, Codec IIC)
  - Data & TCP/IP (TCP MSS, BIP Status)
  - System & Hardware (USB CDC, RI Auto, GNSS URC)
  
  Hardware Setup:
  - Connect EC200U TX to ESP32 GPIO16 (EC200U_RX_PIN)
  - Connect EC200U RX to ESP32 GPIO17 (EC200U_TX_PIN)
  - Connect PW_KEY to GPIO10
*/

#include <QuectelEC200U.h>

// Pin Definitions
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17
#define EC200U_PW_KEY_PIN 10

// Global Objects
HardwareSerial SerialAT(1);
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println(F("\nQuectel EC200U Advanced Features Demo"));
  
  // Power on modem
  Serial.println(F("Powering on modem..."));
  modem.powerOn(EC200U_PW_KEY_PIN);
  
  // Initialize UART
  SerialAT.begin(115200, SERIAL_8N1, EC200U_RX_PIN, EC200U_TX_PIN);
  modem.enableDebug(Serial);
  
  // Initialize Modem
  if (!modem.begin()) {
    Serial.println(F("Modem initialization failed!"));
    return;
  }
  
  Serial.println(F("Modem Ready!"));
  printMenu();
}

void loop() {
  if (Serial.available()) {
    char key = Serial.read();
    handleInput(key);
    printMenu();
  }
}

void printMenu() {
  Serial.println(F("\n--- Advanced Features Menu ---"));
  Serial.println(F("[1] Network & SIM"));
  Serial.println(F("    a. Switch SIM Card (AT+QSIMCHK)"));
  Serial.println(F("    b. Toggle ISIM (AT+QIMSCFG=\"isim\")"));
  Serial.println(F("    c. Set DSDS Mode (AT+QDSTYPE)"));
  Serial.println(F("    d. Get Operator Name (AT+QSPN)"));
  Serial.println(F("    e. Prevent Network Mode Switch"));
  
  Serial.println(F("[2] Audio & Voice"));
  Serial.println(F("    f. Block Incoming Calls"));
  Serial.println(F("    g. Play Audio During Call"));
  Serial.println(F("    h. Configure Codec IIC"));
  
  Serial.println(F("[3] System & Hardware"));
  Serial.println(F("    i. Set USB to CDC ACM"));
  Serial.println(F("    j. Configure RI Auto"));
  Serial.println(F("    k. Configure GNSS URC"));
  
  Serial.print(F("Select option: "));
}

void handleInput(char key) {
  Serial.println(key);
  switch (key) {
    case 'a':
      Serial.println(F("Switching SIM..."));
      if (modem.switchSimCard()) Serial.println(F("Success"));
      else Serial.println(F("Failed"));
      break;
    case 'b':
      Serial.println(F("Toggling ISIM..."));
      if (modem.toggleISIM(true)) Serial.println(F("Enabled"));
      else Serial.println(F("Failed"));
      break;
    case 'c':
      Serial.println(F("Setting DSDS Mode..."));
      if (modem.setDSDSMode(true)) Serial.println(F("Set to DSDS"));
      else Serial.println(F("Failed"));
      break;
    case 'd':
      Serial.print(F("Operator Name: "));
      Serial.println(modem.getOperatorName());
      break;
    case 'e':
      Serial.println(F("Preventing Mode Switch..."));
      if (modem.preventNetworkModeSwitch(true)) Serial.println(F("Enabled"));
      else Serial.println(F("Failed"));
      break;
    case 'f':
      Serial.println(F("Blocking Incoming Calls..."));
      if (modem.blockIncomingCalls(true)) Serial.println(F("Blocked"));
      else Serial.println(F("Failed"));
      break;
    case 'g':
      Serial.println(F("Playing Audio (test.wav)..."));
      if (modem.playAudioDuringCall("test.wav")) Serial.println(F("Playing"));
      else Serial.println(F("Failed"));
      break;
    case 'h':
      Serial.println(F("Configuring Codec IIC..."));
      if (modem.configureAudioCodecIIC(1)) Serial.println(F("Configured"));
      else Serial.println(F("Failed"));
      break;
    case 'i':
      Serial.println(F("Setting USB to CDC..."));
      if (modem.setUSBModeCDC()) Serial.println(F("Set"));
      else Serial.println(F("Failed"));
      break;
    case 'j':
      Serial.println(F("Configuring RI Auto..."));
      if (modem.configureRIAuto(true)) Serial.println(F("Configured"));
      else Serial.println(F("Failed"));
      break;
    case 'k':
      Serial.println(F("Configuring GNSS URC..."));
      if (modem.configureGNSSURC(true)) Serial.println(F("Enabled"));
      else Serial.println(F("Failed"));
      break;
    default:
      Serial.println(F("Invalid option"));
  }
}
