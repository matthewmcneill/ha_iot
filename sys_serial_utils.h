// ----[SERIAL PORT UTILITIES]----- 
// serial port interaction utilities from 
// https://github.com/arduino-libraries/ArduinoECCX08/blob/master/examples/Tools/ECCX08JWSPublicKey/ECCX08JWSPublicKey.ino

#pragma once

String readLine() {
  String line;

  while (1) {
    if (Serial.available()) {
      char c = Serial.read();

      if (c == '\r') {
        // ignore
        continue;
      } else if (c == '\n') {
        break;
      }

      line += c;
    }
  }

  return line;
}

String promptAndReadLine(const char* prompt, const char* defaultValue) {
  Serial.print(prompt);
  Serial.print(" [");
  Serial.print(defaultValue);
  Serial.print("]: ");

  String s = readLine();

  if (s.length() == 0) {
    s = defaultValue;
  }

  Serial.println(s);

  return s;
}

bool promptAndReadYesNo(String prompt, bool defaultValue) {
  prompt = prompt + (defaultValue ? " (Y/n)" : " (y/N)" );
  String yesno = promptAndReadLine(prompt.c_str(), defaultValue ? "Y" : "N");
  yesno.toLowerCase();
  return (yesno.startsWith("y"));
}