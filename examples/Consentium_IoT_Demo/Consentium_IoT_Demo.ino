
#include <QuectelEC200U.h>

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 18
#define EC200U_TX_PIN 17

// Set the EC200U modem power key pin
#define EC200U_PW_KEY_PIN 10

// Set the EC200U modem power status pin
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial& SerialAT = Serial2;
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);

void EC200U_powerOn() {
  pinMode(EC200U_PW_KEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);

  // Check if the modem is already on
  if (digitalRead(EC200U_STATUS_PIN) == LOW) {
    // Power on the modem
    digitalWrite(EC200U_PW_KEY_PIN, LOW);
    delay(500);
    digitalWrite(EC200U_PW_KEY_PIN, HIGH);
    delay(3000);
  }
}
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(7, 8);
QuectelEC200U modem(SerialAT);
#endif

// Consentium IoT API credentials
const char* sendKey = "<YOUR-SEND-KEY>";
const char* boardKey = "<YOUR-BOARD-KEY>";
const char* apn = "your.apn"; // Your mobile network operator's APN

// Sensor data (up to 5 sensors)
// Leave info and data empty for unused sensors
const char* sensor1_info = "Temperature";
const char* sensor1_data = "25.50";
const char* sensor2_info = "Humidity";
const char* sensor2_data = "60.00";
const char* sensor3_info = "";
const char* sensor3_data = "";
const char* sensor4_info = "";
const char* sensor4_data = "";
const char* sensor5_info = "";
const char* sensor5_data = "";

void setup() {
  Serial.begin(115200);
  delay(200);

#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#else
  SerialAT.begin(9600);
#endif

  modem.begin();

  Serial.println("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println("Failed to connect to network");
    while (true);
  }

  Serial.println("Attaching to data network...");
  if (!modem.attachData(apn)) {
    Serial.println("Failed to attach to data network");
    while (true);
  }

  // Construct the JSON payload dynamically
  String jsonPayload = "{\\"sensors\\":{\\"sensorData\\":[";
  bool firstSensor = true;

  if (String(sensor1_info) != "") {
    if (!firstSensor) jsonPayload += ",";
    jsonPayload += "{\\"info\\":\\"" + String(sensor1_info) + "\\",\\"data\\":\\"" + String(sensor1_data) + "\\"}";
    firstSensor = false;
  }
  if (String(sensor2_info) != "") {
    if (!firstSensor) jsonPayload += ",";
    jsonPayload += "{\\"info\\":\\"" + String(sensor2_info) + "\\",\\"data\\":\\"" + String(sensor2_data) + "\\"}";
    firstSensor = false;
  }
  if (String(sensor3_info) != "") {
    if (!firstSensor) jsonPayload += ",";
    jsonPayload += "{\\"info\\":\\"" + String(sensor3_info) + "\\",\\"data\\":\\"" + String(sensor3_data) + "\\"}";
    firstSensor = false;
  }
  if (String(sensor4_info) != "") {
    if (!firstSensor) jsonPayload += ",";
    jsonPayload += "{\\"info\\":\\"" + String(sensor4_info) + "\\",\\"data\\":\\"" + String(sensor4_data) + "\\"}";
    firstSensor = false;
  }
  if (String(sensor5_info) != "") {
    if (!firstSensor) jsonPayload += ",";
    jsonPayload += "{\\"info\\":\\"" + String(sensor5_info) + "\\",\\"data\\":\\"" + String(sensor5_data) + "\\"}";
    firstSensor = false;
  }

  jsonPayload += "]},\\"boardInfo\\":{\\"firmwareVersion\\":\\"1.0\\",\\"architecture\\":\\"ESP32\\",\\"statusOTA\\":false,\\"deviceMAC\\":\\"AA:BB:CC:DD:EE:FF\\",\\"signalStrength\\":-50}}";

  // Construct the URL
  String url = "https://api.consentiumiot.com/v2/updateData?sendKey=";
  url += sendKey;
  url += "&boardKey=";
  url += boardKey;

  String response;
  Serial.println("Sending data to Consentium IoT...");
  Serial.println("Payload: " + jsonPayload);
  if (modem.httpPost(url, jsonPayload, response, "application/json")) {
    Serial.println("Data sent successfully");
    Serial.println("Response:");
    Serial.println(response);
  } else {
    Serial.println("Failed to send data");
  }
}

void loop() {
  // You can add code here to send data periodically
  delay(60000); // Send data every 60 seconds
  // Re-run setup to send data again
  setup();
}
