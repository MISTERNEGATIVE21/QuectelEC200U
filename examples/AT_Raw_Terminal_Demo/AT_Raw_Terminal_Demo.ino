/*
  EC200U AT Raw Terminal Demo

  Use the Arduino Serial Monitor to type raw AT commands directly to the Quectel EC200U.
  Responses from the modem are printed back to the Serial Monitor.

  Serial Monitor settings: 115200 baud, "Both NL & CR" (recommended)

  Quick EC200U AT command cheat sheet (subset – see official Quectel docs for full list):
  - Basic/Device:        AT, ATE0, ATI, AT+GMR
  - SIM/Registration:    AT+CPIN?, AT+ICCID, AT+COPS?, AT+CREG?, AT+CEREG?
  - Signal/Network:      AT+CSQ, AT+CGATT?
  - PDP/Packet Data:     AT+CGDCONT, AT+QICSGP, AT+QIACT, AT+QIDEACT
  - TCP/UDP Sockets:     AT+QIOPEN, AT+QISEND, AT+QIRD, AT+QICLOSE
  - HTTP/HTTPS:          AT+QHTTPCFG, AT+QHTTPURL, AT+QHTTPGET, AT+QHTTPPOST, AT+QHTTPREAD
  - MQTT:                AT+QMTOPEN, AT+QMTCONN, AT+QMTPUB, AT+QMTSUB, AT+QMTDISC, AT+QMTCLOSE
  - SMS:                 AT+CMGF, AT+CMGS, AT+CMGR, AT+CMGL, AT+CMGD
  - Voice Call:          ATD<number>;, ATH, ATA, AT+CLCC, AT+CLIP
  - GNSS:                AT+QGPS=1, AT+QGPSEND, AT+QGPSLOC=2, AT+QGPSCFG, AT+QGPSGNMEA
  - Time/NTP:            AT+QNTP=1,"pool.ntp.org", AT+CCLK?
  - Filesystem:          AT+QFLST, AT+QFUPL, AT+QFOPEN, AT+QFREAD, AT+QFCLOSE, AT+QFDEL
  - TLS/SSL:             AT+QSSLCFG
  - TTS:                 AT+QTTS=1,"Hello"
  - Power Saving:        AT+CPSMS=1
  - Audio:               AT+CLVL, AT+CRSL, AT+CMUT, AT+QMIC, AT+QSIDET, AT+QAUDCH, AT+QDAI, AT+QAUDLOOP
  - USSD:                AT+CUSD=1,"*123#",15

  Tip: Many commands require Ctrl+Z (0x1A) to finish data entry (e.g., SMS body after AT+CMGS).
  For complete details, refer to the official "Quectel EC200U AT Commands Manual" for your firmware.
*/

#if defined(ARDUINO_ARCH_ESP32)
#include <EC200U_ESP32_Config.h> // EC200U_RX=18, EC200U_TX=17, PW_KEY=10 and EC200U_powerOn()
#endif

#if !defined(ARDUINO_ARCH_ESP32)
#include <SoftwareSerial.h>
#ifndef AT_RX_PIN
#define AT_RX_PIN 7
#endif
#ifndef AT_TX_PIN
#define AT_TX_PIN 8
#endif
SoftwareSerial SerialAT(AT_RX_PIN, AT_TX_PIN);
#endif

void setup() {
  Serial.begin(115200);

#if defined(ARDUINO_ARCH_ESP32)
  // Power on EC200U and start UART2 with standard pins
  EC200U_powerOn();
  Serial2.begin(115200, SERIAL_8N1, EC200U_RX, EC200U_TX);
#else
  // Start software serial (many AVR boards are more reliable at 9600)
  SerialAT.begin(9600);
#endif

  Serial.println();
  Serial.println(F("EC200U AT Raw Terminal"));
  Serial.println(F("- Type AT commands here and press Enter (set Serial Monitor to Both NL & CR)"));
  Serial.println(F("- Press Ctrl+Z to finish inputs like SMS (AT+CMGS)."));
}

void loop() {
#if defined(ARDUINO_ARCH_ESP32)
  // PC -> Modem
  while (Serial.available()) {
    int c = Serial.read();
    Serial2.write((uint8_t)c);
  }
  // Modem -> PC
  while (Serial2.available()) {
    int c = Serial2.read();
    Serial.write((uint8_t)c);
  }
#else
  // PC -> Modem
  while (Serial.available()) {
    int c = Serial.read();
    SerialAT.write((uint8_t)c);
  }
  // Modem -> PC
  while (SerialAT.available()) {
    int c = SerialAT.read();
    Serial.write((uint8_t)c);
  }
#endif
}
