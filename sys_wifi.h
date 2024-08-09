// ----[WIFI CONTROL MODULE]----- 
// built for Arduino IoT 33 wifi board
// based on the tutorials by Alex Astrum
// https://medium.com/@alexastrum/getting-started-with-arduino-and-firebase-347ec6917da5

#pragma once

#include <Arduino.h>

//Nano IOT 33
#ifdef ARDUINO_ARCH_SAMD
  #include <WiFiNINA.h>
  #include <utility/wifi_drv.h>
#endif

// Nano ESP 32
#ifdef ARDUINO_ARCH_ESP32
  #include <WiFi.h>
  #include <esp_wifi.h>
  #define WL_MAC_ADDR_LENGTH 6
#endif

#include "sys_config.h"
#include "sys_logStatus.h"

/**
 * Re-initialize the WiFi driver.
 * This is currently necessary to switch from BLE to WiFi.
 */
void resetWiFi() {

  #ifdef ARDUINO_ARCH_SAMD
    wiFiDrv.wifiDriverDeinit();
    wiFiDrv.wifiDriverInit();
  #endif

  #ifdef ARDUINO_ARCH_ESP32
// seems to permanently kill the wifi
//    esp_wifi_disconnect();
//    esp_wifi_stop();
//    esp_wifi_deinit();
//    esp_wifi_init(NULL); // Reinitialize the Wi-Fi driver
//    delay(1000);
  #endif

}

void connectToWiFi()
{
  int status = WiFi.status();
  if (status == WL_CONNECTED)
  {
    return;
  }

  // connect to the wifi
  logStatus("Connecting to WiFi...");
  while(true) {
    WiFi.setHostname(config.deviceID.c_str());
    status = WiFi.begin(config.secretWiFiSSID.c_str(), config.secretWiFiPassword.c_str());
#ifdef ARDUINO_ARCH_ESP32
    status = WiFi.waitForConnectResult();  // needed for ESP32
#endif
    if (status == WL_CONNECTED) {
      break;
    } else {
      // evaluate failure mode   
      switch (status) {
      case WL_CONNECT_FAILED:
        logText("Connection failed. Check SSID and password.");
        break;
      case WL_NO_SSID_AVAIL:
        logText("SSID not found. Check if the network is available.");
        break;
      case WL_CONNECTION_LOST:
        logText("Connection lost. Check network stability.");
        break;
      case WL_DISCONNECTED:
        logText("Connection disconnected. Double-check that you've entered the correct SSID and password.");
        break; 
      default:
        logText("Unknown error [" + String(status) + "] occurred.");
      }
    }
    logError("Retrying in 5 seconds...");
    resetWiFi();
    delay(5000);
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

  #ifdef ARDUINO_ARCH_SAMD
    if (status == WL_NO_MODULE)
    {
      logSuspend("Communication with WiFi module failed!");
    }
    if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION)
    {
      logStatus("Please upgrade WiFi firmware!");
    }
  #endif

  resetWiFi();
  connectToWiFi();
}

// helper functions
String getWiFiMACAddressAsString(bool includeColons = true) {
  byte mac[WL_MAC_ADDR_LENGTH];
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