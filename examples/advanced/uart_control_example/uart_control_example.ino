/*
  UART Control Commands example for QuectelEC200U library
*/

#include <QuectelEC200U.h>

// Use Hardware Serial for communication with the modem


// Create a QuectelEC200U instance

#if defined(ARDUINO_ARCH_ESP32)
  
#else
  #include <SoftwareSerial.h>
  SoftwareSerial modemSerial(10, 11);
#endif

#if defined(ARDUINO_ARCH_ESP32)
  HardwareSerial modemSerial(1);
#else
  #include <SoftwareSerial.h>
  SoftwareSerial modemSerial(10, 11);
#endif
QuectelEC200U modem(modemSerial);

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

  // --- UART Control Example ---
  Serial.println("Setting UART flow control to RTS/CTS (2,2)...");
  if (modem.setUARTFlowControl(2, 2)) {
    Serial.println("UART flow control set!");
  } else {
    Serial.println("Failed to set UART flow control!");
  }
  
  // Note: Changing baud rate will require re-initializing the serial connection
  // with the new baud rate.
  // Serial.println("Setting UART baud rate to 9600...");
  // if (modem.setUARTBaudRate(9600)) {
  //   Serial.println("UART baud rate set! Please re-initialize serial at 9600.");
  // } else {
  //   Serial.println("Failed to set UART baud rate!");
  // }
}

void loop() {
  // Nothing to do here
}
