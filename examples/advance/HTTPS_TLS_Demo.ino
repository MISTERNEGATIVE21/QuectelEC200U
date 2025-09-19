#include <QuectelEC200U.h>

// Advanced HTTPS + TLS demo: uploads a CA cert (optional), configures SSL ctx, performs HTTPS GET

QuectelEC200U modem(Serial1, 115200);

// Replace with your APN credentials
const char* APN = "your.apn"; // e.g., "internet"

// Optional: place a real PEM CA here, or set verify=false below
const char* CA_PEM = "-----BEGIN CERTIFICATE-----\n...your CA PEM here...\n-----END CERTIFICATE-----\n";

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("HTTPS TLS demo");

  modem.begin();

  // Basic bring-up
  if (!modem.isSimReady()) Serial.println("SIM not ready (continuing)...");
  if (!modem.waitForNetwork(60000)) Serial.println("Network not available (continuing)...");
  modem.attachData(APN);
  modem.activatePDP(1);

  // Upload CA into UFS and configure SSL context 1
  bool uploaded = modem.fsUpload("ca.pem", CA_PEM);
  Serial.print("CA upload: "); Serial.println(uploaded ? "OK" : "SKIP/FAIL");
  // If you don't have a valid CA, set verify=false below
  bool sslOk = modem.sslConfigure(1, String("UFS:") + "ca.pem", true /*verify*/);
  Serial.print("SSL cfg: "); Serial.println(sslOk ? "OK" : "FAIL");

  // Perform HTTPS GET
  String resp;
  if (modem.httpsGet("https://example.com", resp)) {
    Serial.println("HTTPS OK");
    Serial.println(resp);
  } else {
    Serial.println("HTTPS failed");
  }
}

void loop() {
  // no-op
}
