#include <QuectelEC200U.h>
#include <ArduinoJson.h>
#include <pgmspace.h>
#define FIRMWARE_VERSION "1.1"   
#define OTA_STATUS       "false" 

// --- MCU HARDWARE PINS ---
#define RX_PIN 18
#define TX_PIN 17
#define POWER_PIN 10 // Pin to trigger the EC200U-CN PWRKEY

// --- NETWORK & API CREDENTIALS ---
const char* APN_NAME  = "YOUR_APN_HERE"; 
const char* SEND_KEY  = "77e69be62f828f4c8d6f7613fc6f6961";
const char* BOARD_KEY = "b84fd282b400ef35";

// --- TIMING CONFIGURATION ---
const unsigned long SEND_INTERVAL_MS = 30000; // Send telemetry every 30 seconds


// ==============================================================================
// 2. PAYLOAD BUILDER CLASS
// ==============================================================================
class ConsentiumPayload {
  private:
    static const int MAX_PAIRS = 15; // Max number of sensors per payload
    String keys[MAX_PAIRS];
    String values[MAX_PAIRS];
    int count = 0;

  public:
    void clear() { count = 0; }
    
    bool add(String key, String value) {
      if (count >= MAX_PAIRS) {
        Serial.println("[WARNING] Payload full! Cannot add: " + key);
        return false;
      }
      keys[count] = key;
      values[count] = value;
      count++;
      return true;
    }

    bool add(String key, float value, int decimals = 2) {
      return add(key, String(value, decimals));
    }

    bool add(String key, int value) {
      return add(key, String(value));
    }

    String buildJSON(String fwVersion, String ota, String mac, int signal) {
      String json = "{\"sensors\":{\"sensorData\":[";
      
      for (int i = 0; i < count; i++) {
        json += "{\"info\":\"" + keys[i] + "\",\"data\":\"" + values[i] + "\"}";
        if (i < count - 1) json += ",";
      }
      
      json += "]},\"boardInfo\":{";
      json += "\"firmwareVersion\":\"" + fwVersion + "\",";
      json += "\"architecture\":\"EC200U-CN\",";
      json += "\"statusOTA\":" + ota + ",";
      json += "\"deviceMAC\":\"" + mac + "\","; 
      json += "\"signalStrength\":" + String(signal); 
      json += "}}";
      
      return json;
    }
};


// ==============================================================================
// 3. GLOBALS & CERTIFICATES
// ==============================================================================
HardwareSerial modemSerial(1);
ConsentiumPayload telemetry;

unsigned long lastSendTime = 0;
String deviceIMEI = "UNKNOWN"; 

// Cloudflare Root CA required for Consentium IoT
const char cloudflare_root_ca[] PROGMEM = R"CERT(
-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIICnzCCAiWgAwIBAgIQf/MZd5csIkp2FV0TttaF4zAKBggqhkjOPQQDAzBHMQsw
CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU
MBIGA1UEAxMLR1RTIFJvb3QgUjQwHhcNMjMxMjEzMDkwMDAwWhcNMjkwMjIwMTQw
MDAwWjA7MQswCQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMQwwCgYD
VQQDEwNXRTEwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARvzTr+Z1dHTCEDhUDC
[28-04-2026 17:10] Mugiwaraya LUFFY: R127WEcPQMFcF4XGGTfn1XzthkubgdnXGhOlCgP4mMTG6J7/EFmPLCaY9eYmJbsP
AvpWo4H+MIH7MA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYI
KwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUkHeSNWfE/6jM
qeZ72YB5e8yT+TgwHwYDVR0jBBgwFoAUgEzW63T/STaj1dj8tT7FavCUHYwwNAYI
KwYBBQUHAQEEKDAmMCQGCCsGAQUFBzAChhhodHRwOi8vaS5wa2kuZ29vZy9yNC5j
cnQwKwYDVR0fBCQwIjAgoB6gHIYaaHR0cDovL2MucGtpLmdvb2cvci9yNC5jcmww
EwYDVR0gBAwwCjAIBgZngQwBAgEwCgYIKoZIzj0EAwMDaAAwZQIxAOcCq1HW90OV
znX+0RGU1cxAQXomvtgM8zItPZCuFQ8jSBJSjz5keROv9aYsAm5VsQIwJonMaAFi
54mrfhfoFNZEfuNMSQ6/bIBiNLiyoX46FohQvKeIoJ99cx7sUkFN7uJW
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIDtjCCA1ygAwIBAgIQCRs7uF84N5wTBzGaBfmvoTAKBggqhkjOPQQDAjA7MQsw
CQYDVQQGEwJVUzEeMBwGA1UEChMVR29vZ2xlIFRydXN0IFNlcnZpY2VzMQwwCgYD
VQQDEwNXRTEwHhcNMjQxMTIwMDY1NDAzWhcNMjUwMjE4MDY1NDAyWjAcMRowGAYD
VQQDExFjb25zZW50aXVtaW90LmNvbTBZMBMGByqGSM49AgEGCCqGSM49AwEHA0IA
BFNRRpj/YV1iWpf9E+81MAs6ZpW425lXD4WsisaAlu4T51b4q6kmXFy73GSNSdxj
YitHJE4PLVB1Ze5R5TEu5v+jggJfMIICWzAOBgNVHQ8BAf8EBAMCB4AwEwYDVR0l
BAwwCgYIKwYBBQUHAwEwDAYDVR0TAQH/BAIwADAdBgNVHQ4EFgQU94g1UHZDjAG5
lCNBIX09BKgInfswHwYDVR0jBBgwFoAUkHeSNWfE/6jMqeZ72YB5e8yT+TgwXgYI
KwYBBQUHAQEEUjBQMCcGCCsGAQUFBzABhhtodHRwOi8vby5wa2kuZ29vZy9zL3dl
MS9DUnMwJQYIKwYBBQUHMAKGGWh0dHA6Ly9pLnBraS5nb29nL3dlMS5jcnQwMQYD
VR0RBCowKIIRY29uc2VudGl1bWlvdC5jb22CEyouY29uc2VudGl1bWlvdC5jb20w
EwYDVR0gBAwwCjAIBgZngQwBAgEwNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2Mu
cGtpLmdvb2cvd2UxL2c0eDZTYkh3MlFRLmNybDCCAQQGCisGAQQB1nkCBAIEgfUE
gfIA8AB2AE51oydcmhDDOFts1N8/Uusd8OCOG41pwLH6ZLFimjnfAAABk0iQZ5UA
AAQDAEcwRQIhALjXshLE53Sfs9a7F9GAbzxM4ycvS8/BsrdNJiJb7vzDAiBpyfXw
ruJeBH1OkezpWQC53OaqnNjhAlAJO02TgoNi0wB2AMz7D2qFcQll/pWbU87psnwi
6YVcDZeNtql+VMD+TA2wAAABk0iQZ8kAAAQDAEcwRQIhANNV9xwxCyv2hilz+f78
vArkjDYSgQtr4lU584TaqIxXAiBmfMveLddCQ+MKFjTNAHji7W46jamHyfkEtvFD
u4mdczAKBggqhkjOPQQDAgNIADBFAiEAoquDYR4Gp/RVekXtBTERdxCEIk/ff1VN
ACdaLCiM6dQCIEgy9lxXc2Sik9dFIje0e8plH7oobG7QBLGfCaKsVHhQ
-----END CERTIFICATE-----
)CERT";


// ==============================================================================
// 4. SETUP ROUTINE
// ==============================================================================
void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));
  
  pinMode(POWER_PIN, OUTPUT);
  Serial.println("\n[SYSTEM] Powering on Modem...");
  digitalWrite(POWER_PIN, LOW);
  delay(2500); 
  digitalWrite(POWER_PIN, HIGH);
  delay(3000); 
  
  modemSerial.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);

  deviceIMEI = getDeviceIMEI();
  Serial.println("[SYSTEM] Device ID (IMEI): " + deviceIMEI);

  uploadCertIfNeeded();

  Serial.println("[NETWORK] Connecting to Cellular Network...");
  sendATCommand("AT+QICSGP=1,1,\"" + String(APN_NAME) + "\",\"\",\"\",1", "OK", 2000);
  sendATCommand("AT+QIACT=1", "OK", 10000);

  Serial.println("[NETWORK] Configuring SSL Context...");
  sendATCommand("AT+QSSLCFG=\"sslversion\",1,4", "OK", 2000);     
  sendATCommand("AT+QSSLCFG=\"ciphersuite\",1,0xFFFF", "OK", 2000); 
  sendATCommand("AT+QSSLCFG=\"seclevel\",1,1", "OK", 2000);         
  sendATCommand("AT+QSSLCFG=\"cacert\",1,\"UFS:cloudflare.pem\"", "OK", 2000); 
  sendATCommand("AT+QSSLCFG=\"sni\",1,1", "OK", 2000);              
  sendATCommand("AT+QSSLCFG=\"ignorelocaltime\",1,1", "OK", 2000);  
  
  Serial.println("[SYSTEM] Setup Complete. Entering Main Loop...");
}

// ==============================================================================
// 5. MAIN LOOP
// ==============================================================================
void loop() {
  if (millis() - lastSendTime >= SEND_INTERVAL_MS || lastSendTime == 0) {
    lastSendTime = millis();
    sendTelemetry();
  }
}

// ==============================================================================
// 6. CORE TELEMETRY LOGIC
// ==============================================================================
void sendTelemetry() {
  Serial.println("\n--- Starting Data Transmission ---");

  int signal_dBm = getSignalStrength();
  Serial.print("[DATA] Current LTE Signal: "); Serial.print(signal_dBm); Serial.println(" dBm");

  telemetry.clear(); 
  
  // Replace these with your actual hardware sensor reading functions!
  telemetry.add("Temperature", random(200, 350) / 10.0, 1);
  telemetry.add("Humidity", random(400, 800) / 10.0, 1);
  telemetry.add("Tamper_Switch", digitalRead(0));


  // 2. Build JSON string dynamically
  String jsonPayload = telemetry.buildJSON(FIRMWARE_VERSION, OTA_STATUS, deviceIMEI, signal_dBm);

  // 3. Open Secure Socket
  Serial.println("[NETWORK] Opening Secure Socket...");
  sendATCommand("AT+QSSLOPEN=1,1,4,\"api.consentiumiot.com\",443,0", "+QSSLOPEN: 4,0", 150000);

  // 4. Construct HTTP POST Request
  String httpRequest = "POST /v2/updateData?sendKey=" + String(SEND_KEY) + "&boardKey=" + String(BOARD_KEY) + " HTTP/1.1\r\n" +
                       "Host: api.consentiumiot.com\r\n" +
                       "Content-Type: application/json\r\n" +
                       "Content-Length: " + String(jsonPayload.length()) + "\r\n" +
                       "Connection: close\r\n\r\n" + 
                       jsonPayload;

  // 5. Push Data
  Serial.println("[NETWORK] Pushing Payload...");
  String sendCmd = "AT+QSSLSEND=4," + String(httpRequest.length());
  
  modemSerial.println(sendCmd);
  if (waitForResponse(">", 5000)) {
    modemSerial.print(httpRequest);
    waitForResponse("SEND OK", 10000); 
  }

  // 6. Receive Server Response
  if (waitForResponse("+QSSLURC: \"recv\",4", 15000)) {
    Serial.println("[NETWORK] Response available, reading buffer...");
    sendATCommand("AT+QSSLRECV=4,1500", "OK", 5000); 
  }

  // 7. Clean Up
  Serial.println("[NETWORK] Closing Secure Socket...");
  sendATCommand("AT+QSSLCLOSE=4", "OK", 10000); 
  Serial.println("--- Transmission Complete ---");
}

// ==============================================================================
// 7. HELPER FUNCTIONS
// ==============================================================================

String getDeviceIMEI() {
  modemSerial.println("AT+GSN");
  String response = "";
  unsigned long start = millis();
  
  while (millis() - start < 2000) {
    while (modemSerial.available()) {
      response += (char)modemSerial.read();
    }
    if (response.indexOf("OK") != -1) break;
  }
  
  String imei = "";
  for(int i = 0; i < response.length(); i++) {
    if(isDigit(response[i])) {
      imei += response[i];
    }
  }
  
  if(imei.length() >= 15) {
    return imei.substring(0, 15);
  }
  
  return "00:00:00:00:00:00"; 
}

int getSignalStrength() {
  modemSerial.println("AT+CSQ");
  
  unsigned long start = millis();
  String response = "";
  int rssi = 99; 
  
  while (millis() - start < 3000) {
    while (modemSerial.available()) {
      response += (char)modemSerial.read();
    }
    if (response.indexOf("OK") != -1) break;
  }
  
  int index = response.indexOf("+CSQ: ");
  if (index != -1) {
    int commaIndex = response.indexOf(',', index);
    if (commaIndex != -1) {
      String rssiStr = response.substring(index + 6, commaIndex);
      rssi = rssiStr.toInt();
    }
  }
  
  if (rssi >= 0 && rssi <= 31) {
    return -113 + (rssi * 2);
  }
  return -999; 
}

void uploadCertIfNeeded() {
  Serial.println("[FILE] Checking modem filesystem for certificate...");
  
  modemSerial.println("AT+QFLST=\"UFS:cloudflare.pem\"");
  if (!waitForResponse("+QFLST: \"UFS:cloudflare.pem\"", 2000)) {
    Serial.println("[FILE] Certificate missing. Uploading...");
    
    int certLen = strlen(cloudflare_root_ca); 
    modemSerial.print("AT+QFUPL=\"UFS:cloudflare.pem\",");
    modemSerial.print(certLen);
    modemSerial.println(",100");
    
    if (waitForResponse("CONNECT", 5000)) {
      modemSerial.print(cloudflare_root_ca); 
      if (waitForResponse("OK", 15000)) {
        Serial.println("[FILE] Certificate uploaded!");
      } else {
        Serial.println("[ERROR] Upload failed.");
      }
    }
  } else {
    Serial.println("[FILE] Certificate already exists.");
    waitForResponse("OK", 1000); 
  }
}
 void sendATCommand(String command, String expectedResponse, unsigned long timeout) {
  modemSerial.println(command);
  waitForResponse(expectedResponse, timeout);
}

bool waitForResponse(String expected, unsigned long timeout) {
  unsigned long start = millis();
  String response = "";
  while (millis() - start < timeout) {
    while (modemSerial.available()) {
      char c = modemSerial.read();
      response += c;
      Serial.write(c); 
      if (response.endsWith(expected)) {
        return true;
      }
    }
  }
  return false;
}
