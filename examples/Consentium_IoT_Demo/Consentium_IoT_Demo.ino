#include <QuectelEC200U.h>
#include <ArduinoJson.h>
#include "ca_cert.h"

// Set the EC200U modem RX and TX pins
#define EC200U_RX_PIN 16
#define EC200U_TX_PIN 17

// Set the EC200U modem power key pin
#define EC200U_PW_KEY_PIN 10

// Set the EC200U modem power status pin
#define EC200U_STATUS_PIN 2

#if defined(ARDUINO_ARCH_ESP32)
HardwareSerial& SerialAT = Serial2;
QuectelEC200U modem(SerialAT, 115200, EC200U_RX_PIN, EC200U_TX_PIN);

void EC200U_powerOn() {
  Serial.println("Setting up power pins...");
  pinMode(EC200U_PW_KEY_PIN, OUTPUT);
  pinMode(EC200U_STATUS_PIN, INPUT);

  // Check if the modem is already on
  int statusPin = digitalRead(EC200U_STATUS_PIN);
  Serial.print("Modem STATUS pin: ");
  Serial.println(statusPin);
  
  if (statusPin == LOW) {
    Serial.println("Powering on modem...");
    // Power on the modem
    digitalWrite(EC200U_PW_KEY_PIN, LOW);
    delay(500);
    digitalWrite(EC200U_PW_KEY_PIN, HIGH);
    delay(3000);
    Serial.println("Modem power-on sequence completed");
  } else {
    Serial.println("Modem appears to be already powered on");
  }
}
#else
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
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

const char* cert_path = "consentium_ca.pem";

void sendData() {
  // Construct the JSON payload dynamically using ArduinoJson
  Serial.println();
  Serial.println("Constructing JSON payload...");
  
  StaticJsonDocument<512> doc; // Adjust size as needed

  JsonArray sensorDataArray = doc["sensors"]["sensorData"].to<JsonArray>();

  int sensorCount = 0;
  if (String(sensor1_info) != "") {
    JsonObject sensor1 = sensorDataArray.add<JsonObject>();
    sensor1["info"] = sensor1_info;
    sensor1["data"] = sensor1_data;
    sensorCount++;
  }
  if (String(sensor2_info) != "") {
    JsonObject sensor2 = sensorDataArray.add<JsonObject>();
    sensor2["info"] = sensor2_info;
    sensor2["data"] = sensor2_data;
    sensorCount++;
  }
  if (String(sensor3_info) != "") {
    JsonObject sensor3 = sensorDataArray.add<JsonObject>();
    sensor3["info"] = sensor3_info;
    sensor3["data"] = sensor3_data;
    sensorCount++;
  }
  if (String(sensor4_info) != "") {
    JsonObject sensor4 = sensorDataArray.add<JsonObject>();
    sensor4["info"] = sensor4_info;
    sensor4["data"] = sensor4_data;
    sensorCount++;
  }
  if (String(sensor5_info) != "") {
    JsonObject sensor5 = sensorDataArray.add<JsonObject>();
    sensor5["info"] = sensor5_info;
    sensor5["data"] = sensor5_data;
    sensorCount++;
  }

  Serial.print("Added ");
  Serial.print(sensorCount);
  Serial.println(" sensors to payload");

  JsonObject boardInfo = doc["boardInfo"].to<JsonObject>();
  boardInfo["firmwareVersion"] = "1.0";
  boardInfo["architecture"] = "ESP32";
  boardInfo["statusOTA"] = false;
  
#if defined(ARDUINO_ARCH_ESP32)
  uint64_t chipid = ESP.getEfuseMac();
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           (uint8_t)(chipid >> 40), (uint8_t)(chipid >> 32),
           (uint8_t)(chipid >> 24), (uint8_t)(chipid >> 16),
           (uint8_t)(chipid >> 8), (uint8_t)chipid);
  boardInfo["deviceMAC"] = macStr;
#else
  boardInfo["deviceMAC"] = "AA:BB:CC:DD:EE:FF";
#endif
  
  int signal = modem.getSignalStrength();
  boardInfo["signalStrength"] = signal;
  
  Serial.print("Signal strength: ");
  Serial.println(signal);

  String jsonPayload;
  serializeJson(doc, jsonPayload);

  // Construct the URL
  String url = "https://api.consentiumiot.com/v2/updateData?sendKey=";
  url += sendKey;
  url += "&boardKey=";
  url += boardKey;

  Serial.println();
  Serial.println("======================================");
  Serial.println("Sending data to Consentium IoT...");
  Serial.println("======================================");
  Serial.print("URL: ");
  Serial.println(url);
  Serial.print("Payload: ");
  Serial.println(jsonPayload);
  Serial.println();
  
  String response;
  if (modem.httpsPost(url, jsonPayload, response)) {
    Serial.println("✓ Data sent successfully");
    Serial.println();
    Serial.println("Response:");
    Serial.println("--------------------------------------");
    Serial.println(response);
    Serial.println("--------------------------------------");
  } else {
    Serial.println("❌ Failed to send data");
    Serial.println("⚠️  Check:");
    Serial.println("   - Internet connectivity");
    Serial.println("   - API credentials (sendKey, boardKey)");
    Serial.println("   - API endpoint URL");
  }
}

void setup() {
  // Initialize Serial Monitor with delay for stability
  Serial.begin(115200);
  delay(2000);  // Give time for Serial Monitor to connect
  
  Serial.println();
  Serial.println("======================================");
  Serial.println("  Consentium IoT Demo - Starting");
  Serial.println("======================================");
  Serial.println();

#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#else
  Serial.println("Initializing SoftwareSerial...");
  SerialAT.begin(9600);
#endif

  Serial.println("Enabling modem debug output...");
  modem.enableDebug(Serial);

  Serial.println("Initializing modem (this may take 10-15 seconds)...");
  if (!modem.begin()) {
    Serial.println("❌ Failed to initialize modem!");
    Serial.println("⚠️  Possible issues:");
    Serial.println("   - Check wiring (RX/TX pins)");
    Serial.println("   - Check modem power");
    Serial.println("   - Check SIM card");
    Serial.println();
    while(1);
  }
  Serial.println("✓ Modem initialized successfully");

  Serial.println();
  Serial.println("Waiting for network registration...");
  if (!modem.waitForNetwork(30000)) {
    Serial.println("❌ Failed to connect to network");
    Serial.println("⚠️  Check:");
    Serial.println("   - SIM card is inserted");
    Serial.println("   - Network coverage in your area");
    Serial.println("   - SIM card has active service");
    Serial.println();
    while(1);
  }
  Serial.println("✓ Network registered");

  Serial.println();
  Serial.println("Attaching to data network...");
  if (!modem.attachData(apn)) {
    Serial.println("❌ Failed to attach to data network");
    Serial.println("⚠️  Check APN settings for your operator");
    Serial.println();
    while(1);
  }
  Serial.println("✓ Attached to data network");

  Serial.println();
  Serial.println("Activating PDP context...");
  if (!modem.activatePDP()) {
    Serial.println("❌ Failed to activate PDP context!");
    Serial.println();
    while(1);
  }
  Serial.println("✓ PDP context activated");

  // Upload the CA certificate to the module's filesystem
  Serial.println("Uploading CA certificate...");
  if (modem.fsUpload(cert_path, cloudflare_ca_cert)) {
    Serial.println("Certificate uploaded.");
  } else {
    Serial.println("Failed to upload certificate.");
  }

  // Before making an HTTPS request, you must configure the SSL context.
  if (modem.sslConfigure(1, cert_path)) {
    Serial.println("SSL context configured.");
  } else {
    Serial.println("Failed to configure SSL context.");
  }
  
  Serial.println();
  Serial.println("======================================");
  Serial.println("  Setup completed");
  Serial.println("======================================");
  Serial.println();
}

void loop() {
  // Send data every 60 seconds
  static unsigned long lastSendTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastSendTime >= 60000) {
    lastSendTime = currentTime;
    sendData();
  }
  
  delay(1000);  // Check every second
}