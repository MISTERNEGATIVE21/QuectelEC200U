#pragma once
// Standard ESP32 wiring configuration for EC200U
#if defined(ARDUINO_ARCH_ESP32)
  #ifndef EC200U_RX
  #define EC200U_RX 18    // ESP32 pin connected to EC200U TX
  #endif
  #ifndef EC200U_TX
  #define EC200U_TX 17    // ESP32 pin connected to EC200U RX
  #endif
  #ifndef PW_KEY
  #define PW_KEY   10     // EC200U PWRKEY control (active LOW to power on)
  #endif

  // Use UART2 on ESP32
  #define EC200U_UART Serial2

  inline void EC200U_powerOn() {
    pinMode(PW_KEY, OUTPUT);
    digitalWrite(PW_KEY, LOW);
    delay(3000);
  }
#endif
