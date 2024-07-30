// ----[Internal LED indicator CONTROL MODULE]----- 

#pragma once

#include "sys_logStatus.h"
#include "home_assistant.h"

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


void setupLedIndicator(void)
{
  // light up for startup
  pinMode(LED_BUILTIN, OUTPUT);
  logStatus("Setting up internal LED controls");
  ha.entities.led.onCommand(onSwitchCommand);      // attach switch callback
  ha.entities.buttonA.onCommand(onButtonCommand); // attach button press callback
  ha.entities.buttonB.onCommand(onButtonCommand); // attach button press callback 
}


