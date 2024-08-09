// ----[LOGGING AND DEBUG MODULE]----- 
// set of standard routines for logging and debugging
// Variadic macros used for debugging to print information in de-bugging mode from LarryD, Arduino forum

#pragma once

// un-comment this line to print the debugging statements
#define DEBUG
// uncomment this line to use the internal LED for debugging
#define DEBUG_LED

#ifdef DEBUG
  #define DPRINT(...)    Serial.print(__VA_ARGS__)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  // define blank line
  #define DPRINT(...)
  #define DPRINTLN(...)
#endif

void blinkLED(int duration, int numberOfTimes = 1) {
#ifdef DEBUG_LED
  bool state = (digitalRead(LED_BUILTIN) == HIGH);
  for (int i = 0; i < numberOfTimes; ++i) {
    digitalWrite(LED_BUILTIN, (!state ? HIGH : LOW));
    delay(duration);
    digitalWrite(LED_BUILTIN, (state ? HIGH : LOW));
    delay(duration);
  }
#endif
}

void setupLog() {
#ifdef DEBUG_LED
  pinMode(LED_BUILTIN, OUTPUT);
#endif
  Serial.begin(115200); 
  // Add a timeout to prevent infinite loop when no serial monitor is connected
  unsigned long startTime = millis();
  const unsigned long timeout = 5000; // 5 seconds timeout
  while (!Serial && (millis() - startTime < timeout)) {
    ; // Wait for serial port to connect, but with a timeout
  }
  if (Serial) {
    DPRINTLN("\nSerial port connected.");
  } else {
    DPRINTLN("\nNo serial port connected.");  // a bit pointless really.
  }
}

void logText(String msg) { 
  DPRINTLN(msg); 
}

void logStatus(String msg) {
  logText(msg);
  blinkLED(50);
}

void logError(String msg) {
  logText("Error: " + msg);
  blinkLED(100, 3);
}

void logSuspend(String msg) {
  logText("Execution suspended: " + msg);
  while (true) {
    // Stop execution.
#ifdef DEBUG_LED
    blinkLED(1000);
#endif
  }
}

void logByteArrayAsHex(const byte* byteArray, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    DPRINT(byteArray[i] >> 4, HEX); // Print high nibble
    DPRINT(byteArray[i] & 0xF, HEX); // Print low nibble
    if (i < len - 1) {
      DPRINT(" "); // Add space between bytes
    }
  }
  DPRINTLN();
}
