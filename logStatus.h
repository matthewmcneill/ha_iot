// ----[LOGGING AND DEBUG MODULE]----- 
// set of standard routines for logging and debugging
// Variadic macros used for debugging to print information in de-bugging mode from LarryD, Arduino forum

#pragma once

// un-comment this line to print the debugging statements
#define DEBUG

#ifdef DEBUG
  #define DPRINT(...)    Serial.print(__VA_ARGS__)
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  // define blank line
  #define DPRINT(...)
  #define DPRINTLN(...)
#endif

// uncomment this line to use the internal LED for debugging
// #define DEBUG_LED

void blinkLED(int duration) {
#ifdef DEBUG_LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(duration);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
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
  blinkLED(100);
}

void logError(String msg) {
  logText("Error: " + msg);
  blinkLED(100);
  blinkLED(100);
  blinkLED(100);
}

void logSuspend(String msg) {
  logText("Execution suspended: " + msg);
#ifdef DEBUG_LED
  digitalWrite(LED_BUILTIN, HIGH); // Turn on LED.
#endif
  while (true) {
    // Stop execution.
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
