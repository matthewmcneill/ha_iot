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

#define CRYPTO_SLOT 0   // can be value 0 - 4

// interactive routine to configure the crypto of the device when connected to the serial port in the IDE.
// needed the first time when setting it up to configure the crypto
void configureCrypto() {

  if (!ECCX08.begin()) {
    logSuspend("No ECCX08 present!");
  }

  if (!ECCX08.locked()) {
    if (!promptAndReadYesNo("The ECCX08 on your board is not locked, would you like to PERMANENTLY configure and lock it now?", false) )
    {
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

  // CONFIGURE CRYPTO_SLOT IN CONSTANTS of the module header
  // Serial.println("Hi there, in order to generate a PEM public key for your board, we'll need the following information ...");
  // Serial.println();
  // String slot               = promptAndReadLine("What slot would you like to use? (0 - 4)", String(CRYPTO_SLOT));
  String slot = String(CRYPTO_SLOT);

  String publicKeyPem = ECCX08JWS.publicKey(CRYPTO_SLOT, false); 
  if (publicKeyPem == "") {
    Serial.println("Key missing at slot [" + slot + "]");
    // generate a new private key
    if promptAndReadYesNo("Would you like to generate a new private key?", true)  {
      Serial.println("Generating new key pair at slot [" + slot + "]...");
      publicKeyPem = ECCX08JWS.publicKey(slot.toInt(), true);
    }
  } else {
    Serial.println("Current public key PEM at slot [" + slot + "]:");
    Serial.println(publicKeyPem);
    // uncomment this to interactively request at serial console for new key creation
    // if promptAndReadYesNo("Would you like to generate a new private key?", true)  {
    //   Serial.println("Generating new key pair at slot [" + slot + "]...");
    //  publicKeyPem = ECCX08JWS.publicKey(slot.toInt(), true);
    //  Serial.println("Here's your public key PEM, at slot [" + slot + "]:");
    //  Serial.println();
    //  Serial.println(publicKeyPem);
    // }
  }

  if (!publicKeyPem || publicKeyPem == "") {
    logSuspend("Error generating public key!");
  }

}

void setupCrypto()
{

  // if the serial port is connected, run the interactive configuration checks for the crypto
  if (Serial) {
    configureCrypto();
  }

  // now run the checks for complete configuration
  if (!ECCX08.begin())
  {
    logSuspend("No ECCX08 present!");
  }
  if (!ECCX08.locked())
  {
    logSuspend("The ECCX08 on your board is not locked. Please configure the crypto.");
  }
  String publicKey = ECCX08JWS.publicKey(CRYPTO_SLOT, false);
  if (publicKey == "")
  {
    logSuspend("Key missing. Generate a new key pair. Please configure the crypto.");
  }
  
}

