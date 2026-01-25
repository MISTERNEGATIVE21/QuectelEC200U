/*
  Quectel EC200U AT Command Terminal - Optimized Version

  This example provides an efficient, event-driven AT command terminal for the Quectel EC200U modem.
  
  Features:
  - One-time modem initialization (no repeated restarts)
  - Event-driven loop (only processes when data is available)
  - Command history with Ctrl+Z support
  - No continuous loop refresh
  - Built-in utility commands (/help, /status, /info, etc.)

  Hardware Setup:
  - Connect EC200U TX to ESP32 GPIO16 (EC200U_RX_PIN)
  - Connect EC200U RX to ESP32 GPIO17 (EC200U_TX_PIN)
  - Connect PW_KEY to GPIO10 for power control
  
  Wiring:
    ESP32 GPIO16 (RX) ←→ EC200U TX
    ESP32 GPIO17 (TX) ←→ EC200U RX
    ESP32 GPIO10      ←→ EC200U PW_KEY
    ESP32 GND         ←→ EC200U GND
*/

#include <QuectelEC200U.h>

// Pin Definitions
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17
#define EC200U_PW_KEY_PIN 10
#define EC200U_STATUS_PIN 21 // Example status pin

// Global Objects
#if defined(ARDUINO_ARCH_ZEPHYR)
  HardwareSerial& SerialAT = Serial1;
#else
  HardwareSerial SerialAT(1);
#endif
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);

// Global Variables
String inputBuffer = "";
bool terminalActive = true;



// Power on the modem (one-time operation)
void powerOnModem() {
#if defined(ARDUINO_ARCH_ESP32)
#endif
}

// Print help menu
void printHelp() {
  Serial.println(F("\n╔═══════════════════════════════════════════════╗"));
  Serial.println(F("║    AT Command Terminal - Help Menu           ║"));
  Serial.println(F("╠═══════════════════════════════════════════════╣"));
  Serial.println(F("║ Built-in Commands:                            ║"));
  Serial.println(F("║   /help      Show this help menu              ║"));
  Serial.println(F("║   /history   Show command history             ║"));
  Serial.println(F("║   /clear     Clear command history            ║"));
  Serial.println(F("║   /status    Show modem status                ║"));
  Serial.println(F("║   /info      Show detailed modem info         ║"));
  Serial.println(F("║   /signal    Show signal strength             ║"));
  Serial.println(F("║   /operator  Show network operator            ║"));
  Serial.println(F("║   /quit      Exit terminal                    ║"));
  Serial.println(F("║                                               ║"));
  Serial.println(F("║ Keyboard Shortcuts:                           ║"));
  Serial.println(F("║   Ctrl+Z     Recall previous command          ║"));
  Serial.println(F("║   Ctrl+P     Navigate up in history           ║"));
  Serial.println(F("║   Ctrl+N     Navigate down in history         ║"));
  Serial.println(F("║                                               ║"));
  Serial.println(F("║ AT Commands:                                  ║"));
  Serial.println(F("║   Just type any AT command directly           ║"));
  Serial.println(F("║   Example: AT+CSQ, AT+COPS?, AT+CPIN?         ║"));
  Serial.println(F("╚═══════════════════════════════════════════════╝\n"));
}

// Print modem status
void printStatus() {
  Serial.println(F("\n╔═══════════════════════════════════════════════╗"));
  Serial.println(F("║            Modem Status Report                ║"));
  Serial.println(F("╠═══════════════════════════════════════════════╣"));
  
  int reg = modem.getRegistrationStatus();
  Serial.print(F("║ Registration: "));
  switch(reg) {
    case 0:
      Serial.println(F("Not searching                     ║"));
      break;
    case 1:
      Serial.println(F("Registered (Home)                 ║"));
      break;
    case 2:
      Serial.println(F("Searching...                      ║"));
      break;
    case 3:
      Serial.println(F("Registration denied               ║"));
      break;
    case 4:
      Serial.println(F("Unknown                           ║"));
      break;
    case 5:
      Serial.println(F("Registered (Roaming)              ║"));
      break;
    default:
      Serial.println(F("Unknown                           ║"));
      break;
  }
  
  Serial.print(F("║ Network: "));
  Serial.print((reg == 1 || reg == 5) ? F("Connected") : F("Not Connected"));
  Serial.println(F("                     ║"));
  
  int signal = modem.getSignalStrength();
  Serial.print(F("║ Signal: "));
  if (signal >= 0) {
    String sigStr = String(signal);
    if (signal < 10) sigStr = " " + sigStr;
    Serial.print(sigStr);
    Serial.print(F(" ("));
    if (signal == 0) Serial.print(F("< -113"));
    else if (signal == 1) Serial.print(F("-111  "));
    else if (signal >= 2 && signal <= 30) {
      int dbm = -109 + (signal - 2) * 2;
      String dbmStr = String(dbm);
      if (dbm > -100) dbmStr = " " + dbmStr;
      Serial.print(dbmStr);
    }
    else if (signal == 31) Serial.print(F("> -51  "));
    else Serial.print(F("?      "));
    Serial.println(F(" dBm)              ║"));
  } else {
    Serial.println(F("Unknown                               ║"));
  }
  
  Serial.println(F("╚═══════════════════════════════════════════════╝\n"));
}

// Show command history
void showHistory() {
  Serial.println(F("\n╔═══════════════════════════════════════════════╗"));
  Serial.println(F("║            Command History                    ║"));
  Serial.println(F("╠═══════════════════════════════════════════════╣"));
  
  int count = modem.getHistoryCount();
  if (count == 0) {
    Serial.println(F("║ No commands in history                        ║"));
  } else {
    for (int i = 0; i < count; i++) {
      Serial.print(F("║ "));
      if (i + 1 < 10) Serial.print(F(" "));
      Serial.print(i + 1);
      Serial.print(F(". "));
      String cmd = modem.getFromHistory(i);
      Serial.print(cmd);
      for (int j = cmd.length(); j < 41; j++) Serial.print(F(" "));
      Serial.println(F("║"));
    }
  }
  
  Serial.println(F("╚═══════════════════════════════════════════════╝\n"));
}

// Process user command
void processCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;
  
  // Handle built-in commands
  if (cmd.startsWith(F("/"))) {
    if (cmd == F("/help")) {
      printHelp();
    } else if (cmd == F("/history")) {
      showHistory();
    } else if (cmd == F("/clear")) {
      modem.clearHistory();
      Serial.println(F("✓ Command history cleared"));
    } else if (cmd == F("/status")) {
      printStatus();
    } else if (cmd == F("/info")) {
      Serial.println();
      Serial.println(modem.getModemInfo());
      Serial.println();
    } else if (cmd == F("/signal")) {
      int sig = modem.getSignalStrength();
      Serial.print(F("Signal Strength: "));
      if (sig >= 0) {
        Serial.print(sig);
        Serial.print(F(" ("));
        if (sig == 0) Serial.print(F("< -113 dBm"));
        else if (sig == 1) Serial.print(F("-111 dBm"));
        else if (sig >= 2 && sig <= 30) {
          Serial.print(-109 + (sig - 2) * 2);
          Serial.print(F(" dBm"));
        }
        else if (sig == 31) Serial.print(F("> -51 dBm"));
        else Serial.print(F("Unknown"));
        Serial.println(F(")"));
      } else {
        Serial.println(F("Unable to read"));
      }
    } else if (cmd == F("/operator")) {
      String op = modem.getOperator();
      Serial.print(F("Network Operator: "));
      Serial.println(op.length() > 0 ? op : F("Unknown"));
    } else if (cmd == F("/quit")) {
      Serial.println(F("\n✓ Exiting terminal. Reset board to restart."));
      terminalActive = false;
      return;
    } else {
      Serial.println(F("✗ Unknown command. Type /help for available commands."));
    }
    Serial.print(F("> "));
    return;
  }
  
  // Add to history (avoid duplicates)
  modem.addToHistory(cmd);
  
  // Send AT command directly to modem
  Serial.print(F(">> "));
  Serial.println(cmd);
  SerialAT.println(cmd);
  
  // Wait for response (simple timeout-based approach)
  uint32_t start = millis();
  String response = "";
  bool responseComplete = false;
  
  while (millis() - start < 5000 && !responseComplete) {
    while (SerialAT.available()) {
      char c = SerialAT.read();
      response += c;
      Serial.write(c);  // Echo to serial monitor
      
      // Check for complete response
      if (response.endsWith(F("\r\nOK\r\n")) || 
          response.endsWith(F("\r\nERROR\r\n")) ||
          response.indexOf(F("+CME ERROR:")) != -1 ||
          response.indexOf(F("+CMS ERROR:")) != -1) {
        responseComplete = true;
      }
    }
    
    if (!SerialAT.available() && !responseComplete) {
      delay(10);  // Small delay to allow buffer to fill
    }
  }
  
  if (!responseComplete && response.length() == 0) {
    Serial.println(F("[No response]"));
  } else if (!responseComplete) {
    Serial.println(F("\n[Timeout - partial response]"));
  }
  
  Serial.print(F("> "));
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);
  
  Serial.println(F("\n"));
  Serial.println(F("╔═══════════════════════════════════════════════╗"));
  Serial.println(F("║   Quectel EC200U AT Command Terminal         ║"));
  Serial.println(F("║   Event-Driven, One-Time Initialization       ║"));
  Serial.println(F("║   Version 2.0                                 ║"));
  Serial.println(F("╚═══════════════════════════════════════════════╝"));
  Serial.println();
  
  // Power on modem (one-time operation)
  powerOnModem();
  
  // Initialize serial communication with modem
  Serial.println(F("Initializing UART communication..."));
#if defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(115200, SERIAL_8N1, EC200U_RX_PIN, EC200U_TX_PIN);
#elif defined(ARDUINO_ARCH_ZEPHYR)
  SerialAT.begin(115200);
#else
  SerialAT.begin(115200);
#endif
  Serial.println(F("✓ UART ready"));
  
  // Enable debug output
  modem.enableDebug(Serial);
  
  // Initialize modem (one-time operation, no restarts)
  Serial.println(F("\nInitializing modem..."));
  Serial.println(F("(This happens only once - no repeated restarts)"));
  
  if (!modem.begin()) {
    Serial.println(F("⚠ Modem initialization failed!"));
    Serial.println(F("  Don't worry - you can still send AT commands manually."));
    Serial.println(F("  The modem may take a few moments to respond."));
  } else {
    Serial.println(F("✓ Modem initialized successfully!"));
    Serial.println(F("  Ready to accept commands."));
  }
  
  Serial.println(F("\nTerminal is ready. Type /help for available commands."));
  Serial.println(F("Enter AT commands directly (e.g., AT+CSQ, AT+COPS?)"));
  Serial.print(F("> "));
}

void loop() {
  // Exit if terminal is inactive
  if (!terminalActive) {
    delay(1000);
    return;
  }
  
  // Handle unsolicited response codes (URCs) from modem
  // Only read when data is available - no continuous polling
  if (SerialAT.available()) {
    // Check if we're in the middle of user input
    bool wasUserTyping = (inputBuffer.length() > 0);
    
    if (wasUserTyping) {
      Serial.println();  // New line before URC
    }
    
    Serial.print(F("[URC] "));
    while (SerialAT.available()) {
      char c = SerialAT.read();
      Serial.write(c);
      delay(1);  // Small delay for stable reading
    }
    Serial.println();
    
    // Restore prompt and user input if they were typing
    if (wasUserTyping) {
      Serial.print(F("> "));
      Serial.print(inputBuffer);
    }
  }
  
  // Handle user input - only when data is available
  if (Serial.available()) {
    char c = Serial.read();
    
    // Ctrl+Z (ASCII 26) - Recall previous command
    if (c == 26) {
      String prev = modem.getPreviousCommand();
      if (prev.length() > 0) {
        // Clear current line
        for (int i = 0; i < inputBuffer.length(); i++) {
          Serial.write('\b');
          Serial.write(' ');
          Serial.write('\b');
        }
        inputBuffer = prev;
        Serial.print(inputBuffer);
      }
      return;
    }
    
    // Ctrl+P (ASCII 16) - Previous in history
    if (c == 16) {
      String prev = modem.getPreviousCommand();
      if (prev.length() > 0) {
        for (int i = 0; i < inputBuffer.length(); i++) {
          Serial.write('\b');
          Serial.write(' ');
          Serial.write('\b');
        }
        inputBuffer = prev;
        Serial.print(inputBuffer);
      }
      return;
    }
    
    // Ctrl+N (ASCII 14) - Next in history
    if (c == 14) {
      String next = modem.getNextCommand();
      for (int i = 0; i < inputBuffer.length(); i++) {
        Serial.write('\b');
        Serial.write(' ');
        Serial.write('\b');
      }
      inputBuffer = next;
      if (next.length() > 0) {
        Serial.print(inputBuffer);
      }
      return;
    }
    
    // Backspace or Delete
    if (c == '\b' || c == 127) {
      if (inputBuffer.length() > 0) {
        inputBuffer.remove(inputBuffer.length() - 1);
        Serial.write('\b');
        Serial.write(' ');
        Serial.write('\b');
      }
      return;
    }
    
    // Enter - Process command
    if (c == '\n' || c == '\r') {
      Serial.println();
      processCommand(inputBuffer);
      inputBuffer = "";
      return;
    }
    
    // Regular printable character
    if (isPrintable(c) && inputBuffer.length() < 128) {
      inputBuffer += c;
      Serial.write(c);  // Echo character
    }
  }
  
  // CRITICAL: Only delay when idle to save CPU
  // This is the key difference - no constant loop refresh
  if (!Serial.available() && !SerialAT.available()) {
    delay(10);  // Small delay only when nothing to do
  }
}