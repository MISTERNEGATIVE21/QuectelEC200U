// src/QuectelTelegramBot.h

#ifndef QUECTEL_TELEGRAM_BOT_H
#define QUECTEL_TELEGRAM_BOT_H

#include "QuectelEC200U.h"
#include <Arduino.h>
#include <ArduinoJson.h>

struct telegramMessage {
  String text;
  String chat_id;
  String from_name;
  String date;
  long update_id;
};

class QuectelTelegramBot {
public:
  QuectelTelegramBot(const String &token, QuectelEC200U &modem);

  String getToken() const { return _token; }

  // Fetch updates from Telegram API
  // Provide an offset to acknowledge previous messages
  int getUpdates(long offset);

  // Send a message to a specific chat ID
  bool sendMessage(const String &chat_id, const String &text,
                   const String &parse_mode = "");

  // To buffer incoming messages (up to 5 at a time to save memory)
  telegramMessage messages[5];

  // Holds the highest update_id received so far
  long last_message_received;

private:
  String _token;
  QuectelEC200U &_modem;
  String _apiUrl;

  bool _sendPost(const String &endpoint, const JsonDocument &payload,
                 String &response);
  bool _sendGet(const String &endpoint, String &response);
};

#endif
