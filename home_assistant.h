// ----[HOME ASSISTANT CONTROL MODULE]----- 
// built for Arduino IoT 33 wifi board

#pragma once

#include <Arduino.h>
#include <WiFi.h>

// include system modules
#include "sys_config.h"
#include "sys_logStatus.h"
#include "sys_wifi.h"

// ================================[ A cache of persistent strings for UIDs ]==================================
// this hacky work around is required because the homeassistant library does not make local copies of the strings 
// being used, but just keeps references to the pointers to the char arrays.  So it works fine with constants, 
// but if you are using more dynamic values, which we need for uniqueness, then you need fixed char array 
// memory address pointers.  Otherwise you will get all sorts of random data in your unique ids.
// don't use <vectors> because the pointers can change, don't use Strings because the pointers can change.
#include "sys_static_strings.h"

// Save a string in the array and return a pointer to the saved string
const char* newUid(const String& name, int instance = -1) {
    String s_uid = getUniqueChipID();  
    if (instance >= 0) {
        s_uid = s_uid + "_" + String(instance);
    }
    s_uid = s_uid + "_" + name;  // put the name last in case its very long

    return storeStaticString(s_uid);
}

// ====================================[ HA device + entity definition ]=======================================

// Turns on debug information of the ArduinoHA core (from <ArduinoHADefines.h>)
// Please note that you need to initialize serial interface 
// by calling Serial.begin([baudRate]) before initializing ArduinoHA.
// #define ARDUINOHA_DEBUG

#include <ArduinoHADefines.h>   // HA libraries
#include <ArduinoHA.h>          //  |
#include <HADevice.h>           //  |
#include <HAMqtt.h>             //  |
#include <Thread.h>             // simple threading library for time event driven updates
#include <ThreadController.h>   //  |

WiFiClient networkClient;               // declare a wifi client object for the HA MQTT connection 

// a nice herlper class to organise all the HA objects into a neat collection 
// that makes them easier to navigate in the logic and collects all the configuration 
// together into a neat package.  Note that the order of construction is very precise
class HADataType {
public:
    HADevice device; // HADevice entity object
    HAMqtt mqtt;     // mqtt communication object
    // Constructor for HADataType
    HADataType(Client& netClient) :
        device(),                       
        mqtt(netClient, this->device),  // needs to be constructed here with the newly created device in this order
        entities(),
        timers()
        {}; 

    // Nested class for defining the Entities on the device
    class HAEntitiesType {
    public:            
        HAButton buttonA;
        HAButton buttonB;
        HASwitch led;
        HASensorNumber temperature;
        // Set up all the entities (note that event handlers are added later)
        HAEntitiesType() : 
            buttonA(newUid("ButtonA")), 
            buttonB(newUid("ButtonB")),
            led(newUid("Led")),
            temperature(newUid("Temperature"), HASensorNumber::PrecisionP1) 
            {
              // LED switch
              led.setIcon("mdi:lightbulb");
              led.setName("Device LED"); 
              // Button A
              buttonA.setIcon("mdi:numeric-2-circle"); 
              buttonA.setName("Flash LED 2x"); 
              // Button B
              buttonB.setIcon("mdi:numeric-3-circle"); 
              buttonB.setName("Flash LED 3x"); 
              // Temperature sensor
              temperature.setName("Temperature °C");
              temperature.setUnitOfMeasurement("°C");
              temperature.setIcon("mdi:thermometer-water");
            }
    };
    HAEntitiesType entities; // container object for entities

    // Nested class for defining the events we want to run on a timer
    class ThreadTimersType {
    public: 
      ThreadController controller;  // ThreadController that will controll all threads
      Thread updateTemperature;     // timer thread for polling the temperature sensor
      ThreadTimersType() :
          controller(),
          updateTemperature() 
          {
              // add all the threads to the controller
              this->controller.add(&this->updateTemperature);
          }
    } timers;

    // Method to drive all the polling on the ha object and raise the events
    void loop() {
      this->timers.controller.run();    // poll all the timer events
      this->mqtt.loop();                // then propagate any status to MQTT and poll MQTT events
    }
};
// Create instance of the HA controller object, and connect it to the network
HADataType ha(networkClient); 

// ====================[ Include all the sensor and indicator control modules here ]=====================

// have to put them after the ha class definition and instantiation, so that they can reference it
// seems a bit hacky but that's the consequence of combining .h and .cpp and/or the use of a 
// global ha variable.  However the overall code is simpler

// include sensor sub-modules
#include "sensor_temperature.h"   // control module for the temperature sensor
#include "indicator_LED.h"        // control module for the indicator LED

// ====================================[ HA setup and connection ]=======================================
 
void setupHA() {

  // [1] -- set up the HA device --

  logStatus("Configuring the HA Device.");
  // Retrieve MAC address from the wifi card and initialise the HA device with that as the unique ID

  byte macAddress[WL_MAC_ADDR_LENGTH]; // a byte string to containg the mac address we need to configure the HA device
  WiFi.macAddress(macAddress);

  // Initialize the HADevice object
  ha.device.setUniqueId(macAddress, sizeof(macAddress));
  ha.device.enableExtendedUniqueIds();        // essential for code that is uploaded to multiple devices
  ha.device.setName(config.deviceID.c_str());
  ha.device.setSoftwareVersion(config.deviceSoftwareVersion.c_str());
  ha.device.setManufacturer(config.deviceManufacturer.c_str());
  ha.device.setModel(config.deviceModel.c_str());
  // ha_device.setConfigurationUrl("http://192.168.1.55:1234");
  
  // set up the availability options and LWT
  // This method enables availability for all device types registered on the device.
  // For example, if you have 5 sensors on the same device, you can enable
  // shared availability and change availability state of all sensors using
  // single method call "device.setAvailability(false|true)"
  ha.device.enableSharedAvailability();

  // Optionally, you can enable MQTT LWT feature. If device will lose connection
  // to the broker, all device types related to it will be marked as offline in
  // the Home Assistant Panel.
  ha.device.enableLastWill();

  // [2] -- set up the HA control plane --

  logStatus("Connecting HA control plane...");
  setupTempSensors();     
  setupLedIndicator();
    
  // [3] -- connect to the WiFiClient and MQTT --

  logStatus("Connecting to MQTT Broker...");
  // Initialize the HAMqtt object

  while(!ha.mqtt.begin(config.mqttBrokerAddress, config.secretMqttUser.c_str(), config.secretMqttPassword.c_str())) {
    logError("Retrying in 5 seconds...");
    delay(5000);
  }
  
  logStatus("Connected to MQTT Broker");
  ha.device.publishAvailability();

}