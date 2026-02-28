// src/QuectelTelegramBot.cpp

#include "QuectelTelegramBot.h"

QuectelTelegramBot::QuectelTelegramBot(const String &token,
                                       QuectelEC200U &modem)
    : _token(token), _modem(modem), last_message_received(0) {
  _apiUrl = "https://api.telegram.org/bot" + token + "/";
}

bool QuectelTelegramBot::_sendPost(const String &endpoint,
                                   const JsonDocument &payload,
                                   String &response) {
  String url = _apiUrl + endpoint;
  String headers[] = {"Content-Type: application/json"};
  return _modem.httpsPost(url.c_str(), payload, response, headers, 1);
}

bool QuectelTelegramBot::_sendGet(const String &endpoint, String &response) {
  String url = _apiUrl + endpoint;
  return _modem.httpsGet(url.c_str(), response);
}

int QuectelTelegramBot::getUpdates(long offset) {
  String endpoint = "getUpdates?limit=5&allowed_updates=[\"message\"]";
  if (offset > 0) {
    endpoint += "&offset=" + String(offset);
  }

  String response;
  if (!_sendGet(endpoint, response)) {
    return 0;
  }

  // Parse response
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, response);
  if (err) {
    return 0; // JSON parse error
  }

  bool ok = doc["ok"] | false;
  if (!ok)
    return 0;

  JsonArray result = doc["result"].as<JsonArray>();
  int numMessages = 0;

  for (JsonObject update : result) {
    if (numMessages >= 5)
      break;

    long update_id = update["update_id"];
    if (update.containsKey("message")) {
      JsonObject msg = update["message"];
      messages[numMessages].update_id = update_id;
      messages[numMessages].text = msg["text"] | "";
      messages[numMessages].chat_id = msg["chat"]["id"].as<String>();
      messages[numMessages].from_name = msg["from"]["first_name"] | "";
      messages[numMessages].date = msg["date"].as<String>();
      numMessages++;

      if (update_id >= last_message_received) {
        last_message_received = update_id;
      }
    }
  }

  return numMessages;
}

bool QuectelTelegramBot::sendMessage(const String &chat_id, const String &text,
                                     const String &parse_mode) {
  JsonDocument payload;
  payload["chat_id"] = chat_id;
  payload["text"] = text;
  if (parse_mode.length() > 0) {
    payload["parse_mode"] = parse_mode;
  }

  String response;
  bool success = _sendPost("sendMessage", payload, response);

  if (success) {
    // Double check API response wrapper just in case
    JsonDocument resDoc;
    if (!deserializeJson(resDoc, response)) {
      return resDoc["ok"] | false;
    }
  }
  return success;
}
