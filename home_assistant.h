// ----[HOME ASSISTANT CONTROL MODULE]----- 
// built for Arduino IoT 33 wifi board

#pragma once

// Turns on debug information of the ArduinoHA core (from <ArduinoHADefines.h>)
// Please note that you need to initialize serial interface 
// by calling Serial.begin([baudRate]) before initializing ArduinoHA.
// #define ARDUINOHA_DEBUG

#include <ArduinoHADefines.h>
#include <ArduinoHA.h>
#include <HADevice.h>
#include <HAMqtt.h>

#include "config.h"
#include "logStatus.h"
#include "wifi.h"

WiFiClient client;            // declare a wifi client object for the HA MQTT connection 
HADevice device;              // define a home assistant device object to control the device
HAMqtt mqtt(client, device);  // declare the MQTT object for the HADevice to communicat to the broker over wifi

// ====================================[ HA control plane definitions ]=======================================

// --------------- define control plane elements ---------------

HAButton buttonA("cbButtonA");
HAButton buttonB("cbButtonB");
HASwitch led("cbLed");

// ---------------- control plane event handlers ----------------

void onButtonCommand(HAButton* sender)
{
    bool state = (digitalRead(LED_BUILTIN) == HIGH);

    if (sender == &buttonA) {
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

    } else if (sender == &buttonB) {
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

// ------------------- configure the elements ------------------

void createControlPlane() {

  // set up the LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // LED switch
  led.setName("My LED"); // optional
  led.onCommand(onSwitchCommand); // switch callbacks

  // Button A
  buttonA.setIcon("mdi:fire"); // optional
  buttonA.setName("Flash LED 2x"); // optional
  buttonA.onCommand(onButtonCommand); // press callbacks
    
  // Button B
  buttonB.setIcon("mdi:home"); // optional
  buttonB.setName("Flash LED 3x"); // optional
  buttonB.onCommand(onButtonCommand); // press callbacks

}

// ====================================[ HA setup and connection ]=======================================
 
void setupHA() {

  // [1] -- set up the HA device --

  logStatus("Setting up HA Device.");
  // Retrieve MAC address from the wifi card and initialise the HA device with that as the unique ID

  byte macAddress[WL_MAC_ADDR_LENGTH]; // a byte string to containg the mac address we need to configure the HA device
  WiFi.macAddress(macAddress);

  // Initialize the HADevice object
  // device = HADevice(macAddress, sizeof(macAddress));
  device.setUniqueId(macAddress, sizeof(macAddress));
  device.setName(config.deviceID.c_str());
  device.setSoftwareVersion(config.deviceSoftwareVersion.c_str());
  device.setManufacturer(config.deviceManufacturer.c_str());
  device.setModel(config.deviceModel.c_str());
  // device.setConfigurationUrl("http://192.168.1.55:1234");
  
  // set up the availability options and LWT
  // This method enables availability for all device types registered on the device.
  // For example, if you have 5 sensors on the same device, you can enable
  // shared availability and change availability state of all sensors using
  // single method call "device.setAvailability(false|true)"
  device.enableSharedAvailability();

  // Optionally, you can enable MQTT LWT feature. If device will lose connection
  // to the broker, all device types related to it will be marked as offline in
  // the Home Assistant Panel.
  device.enableLastWill();

  // [2] -- set up the HA control plane --
  createControlPlane();
  
  // [3] -- connect to the WiFiClient and MQTT --
  
  logStatus("Connecting to MQTT Broker...");
  // Initialize the HAMqtt object

  while(!mqtt.begin(config.mqttBrokerAddress, config.secretMqttUser.c_str(), config.secretMqttPassword.c_str())) {
    logError("Retrying in 5 seconds...");
    delay(5000);
  }
  
  logStatus("Connected to MQTT Broker");

}

void loopHA() {
  mqtt.loop();
}