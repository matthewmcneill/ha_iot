// -----[MAIN MODULE]----- 
// main Arduino initialisation and control loops

#include "sys_config.h"         // installation configuration data
#include "sys_logStatus.h"      // library for logging and debugging
#include "sys_wifi.h"           // library to control the wifi connection
// #include "time.h"           // NTP time sync library for knowing actual time - uses ezTime library, which needs flags to be configured (see time.h for more info)
// #include "crypto.h"         // crypto control and configuration - don't need this, but may be useful if you are thinking abour TLS connections
#include "home_assistant.h"    // home assistant module

// the setup function runs once when you press reset or power the board
void setup() {
  // light up for startup
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  // [1] set up system modules
  setupLog(); 
  setupConfig();     
  setupWiFi();
//  setupTime();
//  setupCrypto();

  // [2] set up the home asistant config for device & entities and timers
  setupHA();   // setupHA will call setups for all subsystems

  // finish startup
  digitalWrite(LED_BUILTIN, LOW);
}

// the loop function runs over and over again forever
void loop() {
  connectToWiFi();    // check we still have the network up - its flakey on the Nano IoT 33
//  loopTime();         // do ezTime updates
  ha.loop();          // home assistant polling and event updates
}
