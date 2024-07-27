#include "config.h"         // installation configuration data
#include "logStatus.h"      // library for logging and debugging
#include "wifi.h"           // library to control the wifi connection
#include "time.h"           // NTP time sync library for knowing actual time
#include "crypto.h"         // crypto control and configuration
#include "home_assistant.h" // home assistant module

// the setup function runs once when you press reset or power the board
void setup() {
  // light up for startup
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  setupLog(); 
  setupConfig();     
  setupWiFi();
  setupTime();
  setupCrypto();
  setupHA();
  
  // finish startup
  digitalWrite(LED_BUILTIN, LOW);
}

// the loop function runs over and over again forever
void loop() {

  // process modules
  loopTime();
  loopHA();

}
