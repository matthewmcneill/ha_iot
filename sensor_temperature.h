// ----[Dallas Temperature Sensor CONTROL MODULE]----- 

#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include "sys_logStatus.h"
#include "home_assistant.h"

// Data wire is conntec to the Arduino digital pin 2
#define PIN_D2 2
#define ONE_WIRE_BUS PIN_D2 

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature tempSensors(&oneWire);

void onTemperatureUpdateEvent() {
    // Update Temperature
    tempSensors.requestTemperatures();
//    Serial.print("Celsius temperature: ");
//    Serial.println(tempSensors.getTempCByIndex(0)); 
    float tempValue = tempSensors.getTempCByIndex(0); // Get temperature reading from sensor
    ha.entities.temperature.setValue(tempValue); // Send to Home Assistant 
}

void setupTempSensors(void)
{
  logStatus("Dallas Temperature IC Controller");

  // Start up the library
  tempSensors.begin();

  // locate devices on the bus
  logStatus("Locating devices...");
  tempSensors.begin();
  logStatus("Found " + String(tempSensors.getDeviceCount()) + " devices.");

  // report parasite power requirements
  logStatus("Parasite power is: "); 
  if (tempSensors.isParasitePowerMode()) logText("ON");
  else logText("OFF");

  // set up the timer thread
  ha.timers.updateTemperature.onRun(onTemperatureUpdateEvent);
	ha.timers.updateTemperature.setInterval(5000);  // poll every 5 seconds
  ha.timers.updateTemperature.enabled = true;     // ensure that the thread is enabled

}


