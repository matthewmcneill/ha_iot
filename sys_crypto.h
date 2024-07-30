// ----[CRYPTO CONTROL MODULE]----- 
// built for Arduino IoT 33 wifi board
// based on the tutorials by Alex Astrum
// https://medium.com/@alexastrum/getting-started-with-arduino-and-firebase-347ec6917da5
//
// crypto configuration routines taken directly from 
// https://github.com/arduino-libraries/ArduinoECCX08/blob/master/examples/Tools/ECCX08JWSPublicKey/ECCX08JWSPublicKey.ino
//
// ATECCx08 crypto chips only support ES256:
// https://github.com/MicrochipTech/cryptoauthlib/blob/master/lib/jwt/atca_jwt.c
// therefore will need to hand-crank to signature verification code in Google Apps Script
// which only supports RS256 and HS256.  ES256 is receommended as stronger.

#pragma once

#include <ArduinoECCX08.h>
#include <utility/ECCX08JWS.h>
#include <utility/ECCX08DefaultTLSConfig.h> // needed to set up the PEM for the first time
#include <ArduinoJson.h>
#include "sys_logStatus.h"
#include "sys_serial_utils.h"

#define CRYPTO_SLOT 0

// interactive routine to configure the crypto of the device when connected to the serial port in the IDE.
// needed the first time when setting it up to configure the crypto
void configureCrypto() {

  if (!ECCX08.begin()) {
    logSuspend("No ECCX08 present!");
  }

  if (!ECCX08.locked()) {
    String lock = promptAndReadLine("The ECCX08 on your board is not locked, would you like to PERMANENTLY configure and lock it now? (y/N)", "N");
    lock.toLowerCase();

    if (!lock.startsWith("y")) {
      logSuspend("Unfortunately you can't proceed without locking it :(");
    }

    if (!ECCX08.writeConfiguration(ECCX08_DEFAULT_TLS_CONFIG)) {
      logSuspend("Writing ECCX08 configuration failed!");
    }

    if (!ECCX08.lock()) {
      logSuspend("Locking ECCX08 configuration failed!");
    }

    logStatus("ECCX08 locked successfully");
  }

  // don't need this in this application
  // Serial.println("Hi there, in order to generate a PEM public key for your board, we'll need the following information ...");
  // Serial.println();
  // String slot               = promptAndReadLine("What slot would you like to use? (0 - 4)", "0");
  String slot = String(CRYPTO_SLOT);

  String generateNewKey     = promptAndReadLine("Would you like to generate a new private key? (Y/n)", "Y");
  Serial.println();
  generateNewKey.toLowerCase();

  String publicKeyPem = ECCX08JWS.publicKey(slot.toInt(), generateNewKey.startsWith("y"));

  if (!publicKeyPem || publicKeyPem == "") {
    logSuspend("Error generating public key!");
  }

  Serial.println("Here's your public key PEM, enjoy!");
  Serial.println();
  Serial.println(publicKeyPem);
}

void setupCrypto()
{
  if (!ECCX08.begin())
  {
    logSuspend("No ECCX08 present!");
  }
  if (!ECCX08.locked())
  {
    logStatus("The ECCX08 on your board is not locked!");
    configureCrypto();
  }
  String publicKey = ECCX08JWS.publicKey(CRYPTO_SLOT, false);
  if (publicKey == "")
  {
    logStatus("Key missing. Generate a new key pair!");
    configureCrypto();
  }

  logStatus("Crypto Configured");
  logText(publicKey);

  // uncomment this line if you want to change the key on the device
  // REMEMBER TO COMMENT OUT AQAIN BEFORE RUNNING ALONE
  //configureCrypto();
  
}

