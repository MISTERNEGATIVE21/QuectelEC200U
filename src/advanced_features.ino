/*
  Advanced features example for QuectelEC200U_Adv library
*/

#include <QuectelEC200U_Adv.h>

// Use Hardware Serial for communication with the modem
HardwareSerial& modemSerial = Serial1;

// Create a QuectelEC200U_Adv instance
QuectelEC200U_Adv modem(modemSerial);

void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Set up modem serial communication
  modemSerial.begin(115200);

  // Enable debug output
  modem.enableDebug(Serial);

  // Initialize the modem
  if (!modem.begin()) {
    Serial.println("Failed to initialize modem!");
    while (1);
  }

  // Wait for network registration
  if (!modem.waitForNetwork()) {
    Serial.println("Failed to register on the network!");
    while (1);
  }

  // --- Ping Example ---
  Serial.println("Pinging google.com...");
  if (modem.ping("google.com")) {
    Serial.println("Ping successful!");
  } else {
    Serial.println("Ping failed!");
  }

  // --- NTP Sync Example ---
  Serial.println("Synchronizing time with NTP server...");
  if (modem.ntpSync("pool.ntp.org")) {
    Serial.println("NTP sync successful!");
    String clock = modem.getClock();
    Serial.println("Current time: " + clock);
  } else {
    Serial.println("NTP sync failed!");
  }

  // --- DNS Example ---
  Serial.println("Setting DNS servers...");
  if (modem.setDNS("8.8.8.8", "8.8.4.4")) {
    Serial.println("DNS servers set successfully!");
  } else {
    Serial.println("Failed to set DNS servers!");
  }
  
  Serial.println("Getting IP address for google.com...");
  String ip = modem.getIpByHostName("google.com");
  if (ip.length() > 0) {
    Serial.println("IP address for google.com: " + ip);
  } else {
    Serial.println("Failed to get IP address for google.com!");
  }

  // --- ADC Example ---
  Serial.println("Reading ADC value...");
  int adcValue = modem.readADC();
  if (adcValue != -1) {
    Serial.println("ADC value: " + String(adcValue));
  } else {
    Serial.println("Failed to read ADC value!");
  }

  // --- Packet Domain Example ---
  Serial.println("Getting packet data counter...");
  String dataCounter = modem.getPacketDataCounter();
  Serial.println("Packet data counter: " + dataCounter);

  Serial.println("Reading dynamic PDN parameters...");
  String pdnParams = modem.readDynamicPDNParameters();
  Serial.println("Dynamic PDN parameters: " + pdnParams);

  // --- PDP Context Example ---
  Serial.println("Getting PDP Context (CID 1)...");
  QuectelEC200U_Adv::PDPContext pdp_ctx = modem.getPDPContext(1);
  if (pdp_ctx.cid != -1) {
    Serial.println("PDP Context CID: " + String(pdp_ctx.cid));
    Serial.println("PDP Type: " + pdp_ctx.pdp_type);
    Serial.println("APN: " + pdp_ctx.apn);
    Serial.println("Primary Address: " + pdp_ctx.p_addr);
  } else {
    Serial.println("Failed to get PDP Context!");
  }

  // --- Audio Example ---
  Serial.println("Setting speaker volume to 50...");
  if (modem.setSpeakerVolume(50)) {
    Serial.println("Speaker volume set!");
  } else {
    Serial.println("Failed to set speaker volume!");
  }

  Serial.println("Enabling audio loopback...");
  if (modem.audioLoopback(true)) {
    Serial.println("Audio loopback enabled!");
  } else {
    Serial.println("Failed to enable audio loopback!");
  }

  // --- Hardware Example ---
  Serial.println("Getting battery charge status...");
  String batteryStatus = modem.getBatteryCharge();
  Serial.println("Battery Status: " + batteryStatus);

  Serial.println("Performing Wi-Fi scan...");
  String wifiScan = modem.getWifiScan();
  Serial.println("Wi-Fi Scan Results: " + wifiScan);

  // --- Advanced TCP/IP Example ---
  // Note: This example assumes a TCP connection (connectID 0) is already open and in buffer access mode.
  // Real-world usage would involve opening a TCP connection first.
  Serial.println("Switching data access mode to direct push (mode 1) for connection 0...");
  if (modem.switchDataAccessMode(0, 1)) {
    Serial.println("Data access mode switched to direct push!");
  } else {
    Serial.println("Failed to switch data access mode!");
  }

  Serial.println("Enabling data echo for QISEND...");
  if (modem.echoSendData(true)) {
    Serial.println("Data echo enabled!");
  } else {
    Serial.println("Failed to enable data echo!");
  }

  // --- QCFG - Extended Settings Example ---
  Serial.println("Setting network scan mode to LTE only (mode 3)...");
  if (modem.setNetworkScanMode(3)) {
    Serial.println("Network scan mode set to LTE only!");
  } else {
    Serial.println("Failed to set network scan mode!");
  }

  Serial.println("Setting band for GSM (0) and LTE (1)...");
  if (modem.setBand("0", "1")) { // Example: GSM all bands (0), LTE Band 1 (1) - check module specific band masks
    Serial.println("Band settings applied!");
  } else {
    Serial.println("Failed to set band settings!");
  }

  // --- Modem Identification Example ---
  Serial.println("Getting Manufacturer Identification...");
  String manufacturer = modem.getManufacturerIdentification();
  Serial.println("Manufacturer: " + manufacturer);

  Serial.println("Getting Model Identification...");
  String model = modem.getModelIdentification();
  Serial.println("Model: " + model);

  Serial.println("Getting Firmware Revision...");
  String firmware = modem.getFirmwareRevision();
  Serial.println("Firmware Revision: " + firmware);

  // --- Advanced Error Reporting and SIM Example ---
  Serial.println("Getting Extended Error Reports...");
  String errorReports = modem.getExtendedErrorReports();
  Serial.println("Extended Error Reports: " + errorReports);

  Serial.println("Getting SIM Status...");
  String simStatus = modem.getSIMStatus();
  Serial.println("SIM Status: " + simStatus);

  // --- General Commands Example ---
  Serial.println("Setting command echo OFF...");
  modem.setCommandEcho(false);
  delay(1000); // Give modem time to process

  Serial.println("Setting command echo ON...");
  modem.setCommandEcho(true);
  delay(1000);

  Serial.println("Setting verbose error messages...");
  modem.setErrorMessageFormat(2);
  delay(1000);

  // Note: restoreFactoryDefaults will reset the modem's settings
  // Serial.println("Restoring factory defaults...");
  // modem.restoreFactoryDefaults();
  // delay(5000); // Wait for modem to reset

  // --- UART Control Example ---
  Serial.println("Setting UART flow control to RTS/CTS (2,2)...");
  if (modem.setUARTFlowControl(2, 2)) {
    Serial.println("UART flow control set!");
  } else {
    Serial.println("Failed to set UART flow control!");
  }

  // Add more audio examples here as needed
}

void loop() {
  // Nothing to do here
}
