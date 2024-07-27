// ----[CONFIG MODULE]----- 
// software configurations and definitions
#pragma once
#include <Preferences.h>
#include "logStatus.h"
#include "serial_utils.h"

// uncomment this to force a reconfiguration of the parameters
// this requires the device to be connected to the serial console for setup
#define FORCE_RECONFIGURE false

Preferences preferences;

struct ConfigurationStructType {
  // general configuration items, defaults can be specified here, leave blank if you want to force setup
  String deviceID               = "";
  String deviceSoftwareVersion  = "1.0.0";
  String deviceManufacturer     = "Arduino";
  String deviceModel            = "Nano 33 IoT"; 
  String timeZone               = "Europe/London";
  IPAddress mqttBrokerAddress   = IPAddress(0,0,0,0);   

  // Secret Items (really should NOT have defaults specificed here in the source code)
  String secretWiFiSSID         = ""; 
  String secretWiFiPassword     = ""; 
  String secretMqttUser         = "";
  String secretMqttPassword     = "";    
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

  // logText("Config: " + key + " = " + value);

  return value;

}


void setupConfig() {

  preferences.begin("config");

    config.deviceID = loadConfig(
      "dvc_id", 
      config.deviceID,
      "Enter a unique network device ID that is used when connecting to the WifI and Home Assistant: ",
      true,
      FORCE_RECONFIGURE
    );

    config.deviceSoftwareVersion = loadConfig(
      "dvc_sw_ver", 
      config.deviceSoftwareVersion,
      "Software Version: this should not need to be configured: ",
      true,
      false
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
      FORCE_RECONFIGURE
    );

    while ((!config.mqttBrokerAddress.fromString( 
      loadConfig(
        "mqtt_broker_ip", 
        config.mqttBrokerAddress.toString(),
        "Please enter a valid IP address for the MQTT broker: ",
        true,
        FORCE_RECONFIGURE
      ).c_str() 
    )) || (config.mqttBrokerAddress == IPAddress(0,0,0,0)) ) {
      logStatus("Could not parse IP Address, please try again.");
    }

    String ipAddress = config.mqttBrokerAddress.toString();
    
  preferences.end();

  preferences.begin("secrets");

    config.secretWiFiSSID = loadConfig(
      "s_wifi_ssid", 
      config.secretWiFiSSID,
      "Enter your WiFi network SSID: ",
      true,
      FORCE_RECONFIGURE
    );

    config.secretWiFiPassword = loadConfig(
      "s_wifi_pwd", 
      config.secretWiFiPassword,
      "Enter your WiFi password: ",
      true,
      FORCE_RECONFIGURE
    );

    config.secretMqttUser = loadConfig(
      "s_mqtt_user", 
      config.secretMqttUser,
      "Enter your MQTT broker user name: ",
      true,
      FORCE_RECONFIGURE
    );

    config.secretMqttPassword = loadConfig(
      "s_mqtt_pwd", 
      config.secretMqttPassword,
      "Enter your MQTT broker password: ",
      true,
      FORCE_RECONFIGURE
    );

  preferences.end();

}