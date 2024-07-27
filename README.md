# ha_iot
 Comprehensive framework for Arduino Nano IoT 33 to work with Home Assistant

 Compile, upload and run through the IDE with the device connected to the serial console to go through the configuration flow.  Fill out the prompts to configure the device for the first time (setting up wifi, crypto, and MQTT credentials).  It is possible to force the device to rerun setup by uncommenting the directive in config.h.

 Home Assistant will need to be configured with an MQTT broker and MQTT addin to recognise the device.

 Add in control plane items in the home_assistant.h file and any associated logic.

