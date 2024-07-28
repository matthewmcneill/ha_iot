# ha_iot
 Comprehensive framework for Arduino Nano IoT 33 to work with Home Assistant

 Compile, upload and run through the IDE with the device connected to the serial console to go through the configuration flow.  Fill out the prompts to configure the device for the first time (setting up wifi, crypto, and MQTT credentials).  It is possible to force the device to rerun setup by uncommenting the directive in config.h.

 Home Assistant will need to be configured with an MQTT broker and MQTT addin to recognise the device.

 Add in control plane items in the home_assistant.h file and any associated logic.

# Libraries Used

<Preferences.h> https://github.com/vshymanskyy/Preferences
<ArduinoECCX08.h> https://github.com/arduino-libraries/ArduinoECCX08
<WiFiNINA.h> https://www.arduino.cc/reference/en/libraries/wifinina/
<ArduinoJson.h> https://arduinojson.org/
<ArduinoHA.h> https://github.com/dawidchyrzynski/arduino-home-assistant
<OneWire.h> https://www.pjrc.com/teensy/td_libs_OneWire.html
<DallasTemperature.h> https://github.com/milesburton/Arduino-Temperature-Control-Library
<ezTime> https://github.com/ropg/ezTime