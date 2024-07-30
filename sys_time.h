// ----[LOGGING AND DEBUG MODULE]----- 
// set of standard routines for logging and debugging
// Variadic macros used for debugging to print information in de-bugging mode from LarryD, Arduino forum

#pragma once
// un-comment this line to print the debugging statements

#include "sys_logStatus.h"
#include "sys_wifi.h"

// Nano IoT 33 has NVS but no EEPROM so you need to edit ezTime.h header file with correct options. 
// I would have preferred that they are set here in the code.
// #undef EZTIME_CACHE_EEPROM
// #define EZTIME_CACHE_NVS 
#include <ezTime.h>

Timezone tzLondon;

void setupTime() {
  setupWiFi();           // ensure that wifi is set up and connected
  // set up the timer
  ezt::setInterval(60);  // set interval for NTP time sync polling
  logStatus("Syncing NTP...");
	ezt::waitForSync(5);    // ensure that the time is synced (5 sec timeout)

	// Provide official timezone names
	// https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
	tzLondon.setLocation(F(config.timeZone.c_str()));
  logStatus("IS8601 for Time Zone '" + config.timeZone + "' :" + tzLondon.dateTime(ISO8601));

/*
	Serial.println();
	Serial.println("Time in various internet standard formats ...");
	Serial.println();
	Serial.println("ATOM:        " + tzLondon.dateTime(ATOM));	
	Serial.println("COOKIE:      " + tzLondon.dateTime(COOKIE));
	Serial.println("IS8601:      " + tzLondon.dateTime(ISO8601));  // use this one I think
	Serial.println("RFC822:      " + tzLondon.dateTime(RFC822));
	Serial.println("RFC850:      " + tzLondon.dateTime(RFC850));
	Serial.println("RFC1036:     " + tzLondon.dateTime(RFC1036));
	Serial.println("RFC1123:     " + tzLondon.dateTime(RFC1123));
	Serial.println("RFC2822:     " + tzLondon.dateTime(RFC2822));
	Serial.println("RFC3339:     " + tzLondon.dateTime(RFC3339));
	Serial.println("RFC3339_EXT: " + tzLondon.dateTime(RFC3339_EXT));
	Serial.println("RSS:         " + tzLondon.dateTime(RSS));
	Serial.println("W3C:         " + tzLondon.dateTime(W3C));
	Serial.println();
	Serial.println(" ... and any other format, like \"" + tzLondon.dateTime("l ~t~h~e jS ~o~f F Y, g:i A") + "\"");
*/

}

void loopTime() { 
    // process the timer events
  ezt::events();
}