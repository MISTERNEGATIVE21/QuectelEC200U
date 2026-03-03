import re

with open('src/QuectelEC200U.h', 'r') as f:
    h_content = f.read()

# Add macro below includes
macro = """
// Cross-platform Serial setup macro to avoid repetitive code in examples
#if defined(ARDUINO_ARCH_ESP32)
  #define QUECTEL_SERIAL_PORT HardwareSerial
  #define QUECTEL_SERIAL_INIT(name, rx, tx) HardwareSerial name(1)
  #define QUECTEL_SERIAL_BEGIN(name, baud, rx, tx) name.begin(baud, SERIAL_8N1, rx, tx)
#elif defined(ARDUINO_ARCH_ZEPHYR)
  #define QUECTEL_SERIAL_PORT HardwareSerial
  #define QUECTEL_SERIAL_INIT(name, rx, tx) HardwareSerial& name = Serial1
  #define QUECTEL_SERIAL_BEGIN(name, baud, rx, tx) name.begin(baud)
#else
  #include <SoftwareSerial.h>
  #define QUECTEL_SERIAL_PORT SoftwareSerial
  #define QUECTEL_SERIAL_INIT(name, rx, tx) SoftwareSerial name(rx, tx)
  #define QUECTEL_SERIAL_BEGIN(name, baud, rx, tx) name.begin(9600)
#endif

// Define if HardwareSerial is available
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ZEPHYR) || defined(HAVE_HWSERIAL0) || defined(HAVE_HWSERIAL1) || defined(HAVE_HWSERIAL2) || defined(HAVE_HWSERIAL3) || defined(UBRRH) || defined(UBRR0H) || defined(USBCON)
#define QUECTEL_HAS_HARDWARE_SERIAL
#endif
"""
if "QUECTEL_SERIAL_INIT" not in h_content:
    h_content = h_content.replace('#include <ArduinoJson.h>\n', '#include <ArduinoJson.h>\n' + macro)

h_content = re.sub(r'(QuectelEC200U\(HardwareSerial\s*&serial,\s*uint32_t\s*baud\s*=\s*115200,\s*int8_t\s*rxPin\s*=\s*-1,\s*int8_t\s*txPin\s*=\s*-1\);)',
                   r'#if defined(QUECTEL_HAS_HARDWARE_SERIAL)\n    \1\n#endif', h_content)

with open('src/QuectelEC200U.h', 'w') as f:
    f.write(h_content)

with open('src/QuectelEC200U.cpp', 'r') as f:
    cpp_content = f.read()

cpp_macro = """
#if defined(QUECTEL_HAS_HARDWARE_SERIAL)
QuectelEC200U::QuectelEC200U(HardwareSerial &serial, uint32_t baud,
                             int8_t rxPin, int8_t txPin) {
  _serial = &serial;
  _hwSerial = &serial;
  _debugSerial = nullptr;
  _baud = baud;
  _rxPin = rxPin;
  _txPin = txPin;
  _state = MODEM_UNINITIALIZED;
  _initialized = false;
  _echoDisabled = false;
  _simChecked = false;
  _networkRegistered = false;
  _historyCount = 0;
  _historyIndex = 0;
}
#endif
"""

cpp_content = re.sub(
    r'QuectelEC200U::QuectelEC200U\(HardwareSerial.*?_historyIndex = 0;\n}',
    cpp_macro.strip(), cpp_content, flags=re.DOTALL
)

with open('src/QuectelEC200U.cpp', 'w') as f:
    f.write(cpp_content)

