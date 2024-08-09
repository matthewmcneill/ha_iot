// ----[CONFIG MODULE]----- 
// software configurations and definitions
#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include "sys_logStatus.h"
#include "sys_serial_utils.h"

// Nano ESP 32 - for chip UID
#ifdef ARDUINO_ARCH_ESP32
  #include <esp_system.h>
#endif

// to help with debugging, if you want to disable the interactive Serial config options
// set this to true and it will not force interactive
#define NO_RECONFIGURE false

Preferences preferences;

struct ConfigurationStructType {
  // general configuration items, defaults can be specified here, leave blank if you want to use interactive setup
  String deviceID               = "";                               // used for WiFi and Home Assistant device IDs
  String deviceSoftwareVersion  = "1.0.0";                          // used by Home Assistant
  String deviceManufacturer     = "Arduino";                        // used by Home Assistant
#ifdef ARDUINO_ARCH_SAMD
  String deviceModel            = "Nano 33 IoT";                    // used by Home Assistant
#endif
#ifdef ARDUINO_ARCH_ESP32
  String deviceModel            = "Nano ESP32 S3";                    // used by Home Assistant
#endif
  String timeZone               = "Europe/London";                  // used by NTP Time Libraries
  IPAddress mqttBrokerAddress   = IPAddress(0,0,0,0);               // used by Home Assistant for MQTT broker

  // Secret Items (really should NOT have defaults specificed here in the source code)
  String secretWiFiSSID         = "";                               // used by WiFi
  String secretWiFiPassword     = "";                               // used by WiFi
  String secretMqttUser         = "";                               // used by Home Assistant for MQTT broker
  String secretMqttPassword     = "";                               // used by Home Assistant for MQTT broker
} config;


String loadConfig(String key, String defaultValue = "", String prompt = "", bool mandatory = false, bool force = false) {
  // assumes a preferences namespace has been opened
  String value = preferences.getString(key.c_str(), defaultValue);

  if (force) {
    // unset the value, and make the current value the default to force a prompt
    defaultValue = value;
    value = "";
  }

  // no value found
  while (value == "") {
    //logStatus("No value found for key: " + key);
    value = promptAndReadLine(prompt.c_str(), defaultValue.c_str());
    // if mandatory, keep asking
    if (mandatory && (value == "")) {
      logStatus("A value is required for this key to proceed.");
    } else {
      // ok, let's save it
      break;
    }
  }

  // save the value (putString checks for nugatory writes)
  preferences.putString(key.c_str(), value);

  // uncomment this line to get a serial readout of your configuraiton on startup
  // logText("Config: " + key + " = " + value);

  return value;

}


void setupConfig() {
  bool doReconfigure;

  if (Serial && !NO_RECONFIGURE) {
    // if we have a serial port connection to the IDE terminal ask for forced reconfiguration
    // note: interactive reconfiguration will always occur if mandatory items are not set and there are no defaults
    doReconfigure = promptAndReadYesNo("Do you want to configure the device?", false);
  } else {
    doReconfigure = false;
  }

  preferences.begin("config");

    config.deviceID = loadConfig(
      "dvc_id", 
      config.deviceID,
      "Enter a unique network device ID that is used when connecting to the WifI and Home Assistant: ",
      true,
      doReconfigure
    );

    config.deviceManufacturer = loadConfig(
      "dvc_manuf", 
      config.deviceManufacturer,
      "Device Manufacturer: this should not need to be configured: ",
      false,
      false
    );

    config.deviceModel = loadConfig(
      "dvc_model", 
      config.deviceModel,
      "Device Model: this should not need to be configured: ",
      false,
      false
    );

    config.timeZone = loadConfig(
      "tz", 
      config.timeZone,
      "Enter a standard TimeZone for your device to configure local time. A full list is available here https://en.wikipedia.org/wiki/List_of_tz_database_time_zones : ",
      true,
      doReconfigure
    );

    while (!config.mqttBrokerAddress.fromString( 
      loadConfig(
        "mqtt_broker_ip", 
        config.mqttBrokerAddress.toString(),
        "Please enter a valid IP address for the MQTT broker: ",
        true,
        doReconfigure 
      ).c_str() )) 
    {
      logStatus("Could not parse IP Address, or IP address is unconfigured value 0.0.0.0, please try again.");
    }
    
  preferences.end();

  preferences.begin("secrets");

    config.secretWiFiSSID = loadConfig(
      "s_wifi_ssid", 
      config.secretWiFiSSID,
      "Enter your WiFi network SSID: ",
      true,
      doReconfigure
    );

    config.secretWiFiPassword = loadConfig(
      "s_wifi_pwd", 
      config.secretWiFiPassword,
      "Enter your WiFi password: ",
      true,
      doReconfigure
    );

    config.secretMqttUser = loadConfig(
      "s_mqtt_user", 
      config.secretMqttUser,
      "Enter your MQTT broker user name: ",
      true,
      doReconfigure
    );

    config.secretMqttPassword = loadConfig(
      "s_mqtt_pwd", 
      config.secretMqttPassword,
      "Enter your MQTT broker password: ",
      true,
      doReconfigure
    );

  preferences.end();

}

String getUniqueChipID() {

#ifdef ARDUINO_ARCH_SAMD
  // Get the unique device ID (for SAMD-based boards)
  uint32_t uniqueID = *(uint32_t*)0x0080A00C;

  // Convert the ID to a string (hexadecimal representation)
  char uniqueIDStr[9]; // 8 hex digits + null terminator
  sprintf(uniqueIDStr, "%08X", uniqueID);

  return String(uniqueIDStr);
#endif

#ifdef ARDUINO_ARCH_ESP32
  uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);

  // Convert the ID to a string (hexadecimal representation)
  char uniqueIDStr[9]; // 8 hex digits + null terminator
  sprintf(uniqueIDStr, "%08X", chip);

  return String(uniqueIDStr);
#endif

}