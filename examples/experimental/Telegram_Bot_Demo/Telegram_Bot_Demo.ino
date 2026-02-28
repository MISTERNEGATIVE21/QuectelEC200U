/*
  Telegram Bot Demo using Native QuectelEC200U HTTP(S) Commands

  This example demonstrates how to use the EC200U modem to send and receive
  messages from the Telegram API directly using its built-in SSL capabilities,
  without the need for external networking libraries like WiFiClientSecure or
  PPPoS.

  Dependencies:
  - QuectelEC200U
  - ArduinoJson (required by QuectelEC200U and QuectelTelegramBot)

  Instructions:
  1. Replace "your_apn", "your_bot_token", and "your_chat_id" with real values.
  2. Upload to your ESP32.
  3. Send a message to your bot on Telegram to see it echo back, or send
  "status".
*/

#include <QuectelEC200U.h>
#include <QuectelTelegramBot.h>

#define EC200U_RX 18 // ESP32 pin connected to EC200U TX
#define EC200U_TX 17 // ESP32 pin connected to EC200U RX
#define PW_KEY 10

const char *APN = "internet"; // Replace with your APN
const char *USER = "";
const char *PASS = "";

// Telegram Bot Details
#define BOT_TOKEN "your_bot_token"
#define CHAT_ID "your_chat_id" // To send a startup message

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial SerialAT(1);
QuectelEC200U modem(SerialAT, 115200, EC200U_RX, EC200U_TX);
#elif defined(ARDUINO_ARCH_ZEPHYR)
HardwareSerial &SerialAT = Serial1;
QuectelEC200U modem(SerialAT, 115200, EC200U_RX, EC200U_TX);
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(EC200U_RX, EC200U_TX);
QuectelEC200U modem(SerialAT);
#endif

QuectelTelegramBot bot(BOT_TOKEN, modem);

const unsigned long BOT_MTBS = 5000; // mean time between scan messages
unsigned long bot_lasttime;

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;

    Serial.print("Received from ");
    Serial.print(from_name);
    Serial.print(": ");
    Serial.println(text);

    if (text == "/start") {
      String welcome =
          "Welcome to native EC200U Telegram Bot, " + from_name + "!\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    } else if (text == "status") {
      String statusMsg = "System is online via native Quectel AT HTTP(S)!\n";
      statusMsg +=
          "Modem Signal: " + String(modem.getSignalStrength()) + " CSQ";
      bot.sendMessage(chat_id, statusMsg, "");
    } else {
      // Echo back
      bot.sendMessage(chat_id, "You said: " + text, "");
    }
  }
}

static void powerOnModem() {
  pinMode(PW_KEY, OUTPUT);
  // Power on pulse
  digitalWrite(PW_KEY, LOW);
  delay(2000);
  digitalWrite(PW_KEY, HIGH);
  delay(200);
}

void setup() {
  Serial.begin(115200);

#if defined(ARDUINO_ARCH_ESP32)
  SerialAT.begin(115200, SERIAL_8N1, EC200U_RX, EC200U_TX);
#elif defined(ARDUINO_ARCH_ZEPHYR)
  SerialAT.begin(115200);
#else
  SerialAT.begin(9600);
#endif

  powerOnModem();

  Serial.println("Initializing modem...");
  if (!modem.begin()) {
    Serial.println("Modem initialization failed!");
    while (1)
      delay(1000);
  }

  Serial.println("Waiting for network...");
  if (!modem.waitForNetwork(60000)) {
    Serial.println("Network registration failed!");
    while (1)
      delay(1000);
  }

  Serial.println("Attaching to data network...");
  if (!modem.attachData(APN, USER, PASS)) {
    Serial.println("Data attachment failed!");
    while (1)
      delay(1000);
  }

  // Telegram API requires SSL, so we must configure an SSL context on the modem
  // Passing verify=false allows it to connect without provisioning the root CA
  Serial.println("Configuring SSL for Telegram...");
  modem.sslConfigure(1, "", false);

  Serial.println("Sending startup message to Telegram...");
  if (bot.sendMessage(CHAT_ID, "EC200U Native Bot Online!", "")) {
    Serial.println("Startup message sent.");
  } else {
    Serial.println("Failed to send startup message.");
  }
}

void loop() {
  if (millis() - bot_lasttime > BOT_MTBS) {
    // Check for new updates
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    bot_lasttime = millis();
  }
}
