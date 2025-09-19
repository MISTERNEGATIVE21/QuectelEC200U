# QuectelEC200U Arduino Library

This is an Arduino library for the **Quectel EC200U LTE Cat 1 module**, 
providing a simple wrapper around its AT commands.

## ✨ Features
- SMS (send/receive)
- HTTP (GET/POST)
- HTTPS (GET/POST)
- MQTT (connect, publish, subscribe)
- GNSS (get location)
- TTS (Text-to-Speech)
- FTP (upload/download)
- PSM (Power Saving Mode)

## 📂 Installation
1. Download as ZIP and install via Arduino IDE:
   - `Sketch → Include Library → Add .ZIP Library...`
2. Or clone into your Arduino `libraries/` folder.

## 📖 Usage
Include the header:

```cpp
#include <QuectelEC200U.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  modem.begin();
  modem.sendSMS("+1234567890", "Hello from EC200U!");
}
