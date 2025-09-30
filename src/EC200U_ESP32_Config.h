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

  // Default APN can be overridden in sketches before including this header
  #ifndef EC200U_DEFAULT_APN
  #define EC200U_DEFAULT_APN "internet"
  #endif

  // Bring up network registration and PDP context using a default APN.
  // Returns true on success. Adjust APN/credentials in your sketch if needed.
  inline bool EC200U_defaultInit(QuectelEC200U& modem,
                                 const char* apn = EC200U_DEFAULT_APN,
                                 const char* user = "",
                                 const char* pass = "",
                                 int auth = 0,
                                 uint32_t regTimeoutMs = 60000) {
    // Ensure SIM and network registration
    if (!modem.isSimReady()) {
      // Give it a moment to settle if just powered
      delay(1000);
    }
    if (!modem.waitForNetwork(regTimeoutMs)) {
      return false;
    }
    // Attach and activate data on context 1
    if (!modem.attachData(apn, user, pass, auth)) return false;
    if (!modem.activatePDP(1)) return false;
    return true;
  }
#endif
