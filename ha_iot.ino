// -----[MAIN MODULE]----- 
// main Arduino initialisation and control loops

#include "config.h"         // installation configuration data
#include "logStatus.h"      // library for logging and debugging
#include "wifi.h"           // library to control the wifi connection
// #include "time.h"           // NTP time sync library for knowing actual time - uses ezTime library, which needs flags to be configured (see time.h for more info)
// #include "crypto.h"         // crypto control and configuration - don't need this, but may be useful if you are thinking abour TLS connections
#include "home_assistant.h" // home assistant module
#include "temp_sensor.h"    // temperature sensor setup

// the setup function runs once when you press reset or power the board
void setup() {
  // light up for startup
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  setupLog(); 
  setupConfig();     
  setupWiFi();
//  setupTime();
//  setupCrypto();
  setupTempSensors();
  setupHA();
  
  // finish startup
  digitalWrite(LED_BUILTIN, LOW);
}

// the loop function runs over and over again forever
void loop() {

  // process modules
//  loopTime();     // do ezTime updates
  loopHA();       // home assistant updates

}
