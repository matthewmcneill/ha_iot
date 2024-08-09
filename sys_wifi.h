// ----[WIFI CONTROL MODULE]----- 
// built for Arduino IoT 33 wifi board
// based on the tutorials by Alex Astrum
// https://medium.com/@alexastrum/getting-started-with-arduino-and-firebase-347ec6917da5

#pragma once

#include <WiFiNINA.h>
#include <utility/wifi_drv.h>
#include <Arduino.h>

#include "sys_config.h"
#include "sys_logStatus.h"

/**
 * Re-initialize the WiFi driver.
 * This is currently necessary to switch from BLE to WiFi.
 */
void resetWiFi() {
  wiFiDrv.wifiDriverDeinit();
  wiFiDrv.wifiDriverInit();
}

void connectToWiFi()
{
  int status = WiFi.status();
  if (status == WL_CONNECTED)
  {
    return;
  }
  logStatus("Connecting to WiFi...");
  WiFi.setHostname(config.deviceID.c_str());
  while(true) {
    status = WiFi.begin(config.secretWiFiSSID.c_str(), config.secretWiFiPassword.c_str());
    if (status == WL_CONNECTED) {
      break;
    }
    logError("Retrying in 5 seconds...");
    delay(5000);
    resetWiFi();
  }
  // log the mac address
  byte mac[6];
  WiFi.macAddress(mac);
  logStatus("MAC Address: ");
  logByteArrayAsHex(mac,6);
  // connection complete
  logStatus("Connected to WiFi.");
}

void setupWiFi()
{
  int status = WiFi.status();
  if (status == WL_NO_SHIELD)
  {
    logSuspend("WiFi shield missing!");
  }
  if (status == WL_NO_MODULE)
  {
    logSuspend("Communication with WiFi module failed!");
  }
  if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION)
  {
    logStatus("Please upgrade WiFi firmware!");
  }
  connectToWiFi();
}

// helper functions
String getWiFiMACAddressAsString(bool includeColons = true) {
  byte mac[6];
  WiFi.macAddress(mac); 

  String macAddress = "";
  for (int i = 0; i < 6; ++i) {
    macAddress += String(mac[i], HEX); // Convert each byte to hex and append
    if ((i < 5) && (includeColons)) {
      macAddress += ":"; // Add colon separator (except after the last byte)
    }
  }

  return macAddress;
}