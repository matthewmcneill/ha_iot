// ----[HOME ASSISTANT CONTROL MODULE]----- 
// built for Arduino IoT 33 wifi board

#pragma once

// include system modules
#include "config.h"
#include "logStatus.h"
#include "wifi.h"

// include sensor sub-modules
#include "temp_sensor.h"

// --------------[ Thread objects ]-----------------

#include <Thread.h>             // simple threading library for time event driven updates
#include <ThreadController.h>   //  |

// ThreadController that will controll all threads
struct ThreadPointersType {
  ThreadController controller = ThreadController();
  // declare all threads here (i.e. in global vars to persist pointers)
  Thread updateTemperature = Thread();
} threads;

// ----------[ Home Assistant objects ]--------------

// Turns on debug information of the ArduinoHA core (from <ArduinoHADefines.h>)
// Please note that you need to initialize serial interface 
// by calling Serial.begin([baudRate]) before initializing ArduinoHA.
// #define ARDUINOHA_DEBUG
#include <ArduinoHADefines.h>   // HA libraries
#include <ArduinoHA.h>          //  |
#include <HADevice.h>           //  |
#include <HAMqtt.h>             //  |

WiFiClient networkClient;               // declare a wifi client object for the HA MQTT connection 

// a nice herlper class to organise all the HA objects into a neat collection 
// that makes them easier to navigate in the logic and collects all the configuration 
// together into a neat package.  Note that the order of construction is very precise
class HADataType {
public:
    HADevice device; // HADevice entity object
    HAMqtt mqtt;     // mqtt communication object
    // Constructor for HADataType
    HADataType(arduino::Client &netClient) :
        device(),
        mqtt(netClient, this->device),  // needs to be constructed here with the newly created device in this order
        entities()
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
            buttonA("cbButtonA02"), 
            buttonB("cbButtonB02"),
            led("cbLed02"),
            temperature("cbTemperature02", HASensorNumber::PrecisionP1) 
            {
              // LED switch
              led.setIcon("mdi:lightbulb");
              led.setName("My LED"); // optional
              // Button A
              buttonA.setIcon("mdi:numeric-2-circle"); // optional
              buttonA.setName("Flash LED 2x"); // optional
              // Button B
              buttonB.setIcon("mdi:numeric-3-circle"); // optional
              buttonB.setName("Flash LED 3x"); // optional
              // Temperature sensor
              temperature.setName("Temperature °C");
              temperature.setUnitOfMeasurement("°C");
              temperature.setIcon("mdi:thermometer-water"); // optional
            }

    };
    HAEntitiesType entities; // container object for entities
};
// Create instance of the HA controller object, and connect it to the network
HADataType ha(networkClient); 

// ====================================[ HA control plane event handlers ]=======================================

void onButtonCommand(HAButton* sender)
{
    bool state = (digitalRead(LED_BUILTIN) == HIGH);

    if (sender == &ha.entities.buttonA) {
        // button A was clicked, do your logic here
        logText("Button A Pressed");
        digitalWrite(LED_BUILTIN, (!state ? HIGH : LOW));
        delay(250);
        digitalWrite(LED_BUILTIN, (state ? HIGH : LOW));
        delay(250);
        digitalWrite(LED_BUILTIN, (!state ? HIGH : LOW));
        delay(250);
        digitalWrite(LED_BUILTIN, (state ? HIGH : LOW));
        delay(250);

    } else if (sender == &ha.entities.buttonB) {
        // button B was clicked, do your logic here
        logText("Button B Pressed");
        digitalWrite(LED_BUILTIN, (!state ? HIGH : LOW));
        delay(250);
        digitalWrite(LED_BUILTIN, (state ? HIGH : LOW));
        delay(250);
        digitalWrite(LED_BUILTIN, (!state ? HIGH : LOW));
        delay(250);
        digitalWrite(LED_BUILTIN, (state ? HIGH : LOW));
        delay(250);
        digitalWrite(LED_BUILTIN, (!state ? HIGH : LOW));
        delay(250);
        digitalWrite(LED_BUILTIN, (state ? HIGH : LOW));
        delay(250);
    }
}

void onSwitchCommand(bool state, HASwitch* sender)
{
    //Serial.println("Switch Command");
    digitalWrite(LED_BUILTIN, (state ? HIGH : LOW));
    sender->setState(state); // report state back to the Home Assistant
}

// ====================================[ Timer Events (Thread) Setup ]====================================

// ---------------- thread event handlers ----------------


void onTemperatureUpdateEvent() {
    // Update Temperature
    // logStatus("update temperature");
    float tempValue = getTempValueDegC(); // Get temperature reading from sensor
    ha.entities.temperature.setValue(tempValue); // Send to Home Assistant 
}

// --------------- configure the threads ------------------

void setupThreads() {
  // Thread for regular updates of the temperature sensor
	threads.updateTemperature.onRun(onTemperatureUpdateEvent);
	threads.updateTemperature.setInterval(5000); 
	// Add thread to the controller
	threads.controller.add(&threads.updateTemperature);
}

// ====================================[ HA setup and connection ]=======================================
 
void setupHA() {

  // [0] -- set up all the event timer threads
  logStatus("Setting up sensor event timers");
  setupThreads();

  // [1] -- set up the HA device --

  logStatus("Setting up HA Device.");
  // Retrieve MAC address from the wifi card and initialise the HA device with that as the unique ID

  byte macAddress[WL_MAC_ADDR_LENGTH]; // a byte string to containg the mac address we need to configure the HA device
  WiFi.macAddress(macAddress);

  // Initialize the HADevice object
  ha.device.setUniqueId(macAddress, sizeof(macAddress));
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
  ha.entities.led.onCommand(onSwitchCommand); // switch callbacks
  ha.entities.buttonA.onCommand(onButtonCommand); // press callbacks
  ha.entities.buttonB.onCommand(onButtonCommand); // press callbacks
    
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

void loopHA() {
  // run ThreadController and execute all scheduled timer events (threads)
  threads.controller.run();
  // check we still have the network up - its flakey on the Nano IoT 33
  connectToWiFi();
  // execute all the MQTT updates
  ha.mqtt.loop();
}