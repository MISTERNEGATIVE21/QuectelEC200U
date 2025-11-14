#include <QuectelEC200U.h>
#include "ca_cert.h" // Assuming ca_cert.h is in src/ and accessible

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
SoftwareSerial SerialAT(EC200U_RX_PIN, EC200U_TX_PIN);
QuectelEC200U modem(SerialAT);
#endif

const char* cert_path = "https_ca.pem"; // Path on the modem's filesystem

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial monitor to open

#if defined(ARDUINO_ARCH_ESP32)
  EC200U_powerOn();
#else
  SerialAT.begin(9600);
#endif

  Serial.println("Initializing modem...");
  if (!modem.modem_init()) {
    Serial.println("Failed to initialize modem!");
    while (true);
  }
  Serial.println("Modem initialized.");

  // Replace with your actual APN, username, and password
  const String apn = "JioNet";
  const String user = "";
  const String pass = "";

  Serial.println("Attaching to data network...");
  if (!modem.attachData(apn, user, pass)) {
    Serial.println("Failed to attach to data network!");
    while (true);
  }
  Serial.println("Attached to data network.");

  Serial.println("Activating PDP context...");
  if (!modem.activatePDP(1)) {
    Serial.println("Failed to activate PDP context!");
    while (true);
  }
  Serial.println("PDP context activated.");

  // --- HTTPS Setup ---
  // Upload the CA certificate to the module's filesystem
  // Note: You should use the correct CA certificate for your host
  Serial.println("Uploading CA certificate...");
  // The ca_cert.h file contains 'cloudflare_ca_cert'
  if (modem.fsUpload(cert_path, cloudflare_ca_cert)) {
    Serial.println("Certificate uploaded.");
  } else {
    Serial.println("Failed to upload certificate. Make sure ca_cert.h is correct and modem filesystem is writable.");
    // Continue without HTTPS if upload fails, or halt
  }

  // Configure the SSL context (context ID 1, using the uploaded cert)
  if (modem.sslConfigure(1, cert_path)) {
    Serial.println("SSL context configured.");
  } else {
    Serial.println("Failed to configure SSL context. HTTPS requests might fail.");
    // Continue without HTTPS if SSL config fails, or halt
  }

  // --- HTTP GET Request (Port 80 implicitly) ---
  Serial.println("\n--- Performing HTTP GET request ---");
  String httpResponse;
  String httpHeaders[] = {"User-Agent: Arduino-HTTP"};
  // Use a known HTTP-only URL for testing
  if (modem.httpGet("http://vsh.pp.ua/TinyGSM/logo.txt", httpResponse, httpHeaders, 1)) {
    Serial.println("HTTP GET successful. Response (first 500 chars):");
    Serial.println(httpResponse.substring(0, min((int)httpResponse.length(), 500)));
  } else {
    Serial.println("HTTP GET request failed.");
  }

  // --- HTTPS GET Request (Port 443 implicitly) ---
  Serial.println("\n--- Performing HTTPS GET request ---");
  String httpsResponse;
  String httpsHeaders[] = {"User-Agent: Arduino-HTTPS"};
  // Use a known HTTPS URL for testing
  if (modem.httpsGet("https://www.cloudflare.com/", httpsResponse, httpsHeaders, 1)) {
    Serial.println("HTTPS GET successful. Response (first 500 chars):");
    Serial.println(httpsResponse.substring(0, min((int)httpsResponse.length(), 500)));
  } else {
    Serial.println("HTTPS GET request failed. Check SSL configuration and network.");
  }
}

void loop() {
  // Keep the loop empty for now
}
