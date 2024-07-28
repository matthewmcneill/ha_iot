// ----[Dallas Temperature Sensor CONTROL MODULE]----- 

#pragma once

#include <OneWire.h>
#include <DallasTemperature.h>
#include "logStatus.h"

// Data wire is conntec to the Arduino digital pin 2
#define PIN_D2 2
#define ONE_WIRE_BUS PIN_D2 

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature tempSensors(&oneWire);

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
}

float getTempValueDegC() {
  tempSensors.requestTemperatures();
//  Serial.print("Celsius temperature: ");
//  Serial.println(tempSensors.getTempCByIndex(0)); 
  return (tempSensors.getTempCByIndex(0));
}

/*
void loopTempSensors(void){   
  if ((millis() - lastTempTime) > timeIntervalTemp) {
    lastTempTime = millis();

    // Call tempSensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
    tempSensors.requestTemperatures(); 

    Serial.print("Celsius temperature: ");
    // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
    Serial.print(tempSensors.getTempCByIndex(0)); 
    Serial.print(" - Fahrenheit temperature: ");
    Serial.println(tempSensors.getTempFByIndex(0));
  }
}
*/