#include <QuectelEC200U.h>

QuectelEC200U modem(Serial1, 115200);

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("GNSS demo");

  modem.begin();

  Serial.println("Starting GNSS...");
  if (!modem.startGNSS()) {
    Serial.println("Failed to start GNSS");
    return;
  }

  // Optionally configure GNSS (uncomment as needed)
  // modem.setGNSSConfig("nmeasrc", "1"); // output over AT channel

  // Wait up to 60s for a fix using getGNSSLocation(wait)
  String loc = modem.getGNSSLocation(60000);
  if (loc.length() == 0) {
    Serial.println("No fix within timeout");
  } else {
    Serial.println("GNSS Location:");
    Serial.println(loc);
  }

  // Read a few NMEA sentences
  Serial.println("NMEA RMC:");
  Serial.println(modem.getNMEASentence("RMC"));
  Serial.println("NMEA GGA:");
  Serial.println(modem.getNMEASentence("GGA"));

  // Stop GNSS to save power
  modem.stopGNSS();
}

void loop() {
}
