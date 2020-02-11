//--------------------------------------------------------------------------------------
//  OTA
//--------------------------------------------------------------------------------------
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "Global.h"
#include <ESP8266httpUpdate.h>
#include "ota.h"

void setOTA() {
  ArduinoOTA.setHostname("MyWattmeter");//undercscore not allowed
  ArduinoOTA.setPassword("wattmetre");

  ArduinoOTA.onStart([]() {
    Log.println(LogObject::DebugLevels::Normal,"Start OTA update");
  });
  ArduinoOTA.onEnd([]() {
    Log.println(LogObject::DebugLevels::Verbose,"\nEnd of OTA");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Log.printf(LogObject::DebugLevels::Verbose,"Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Log.printf(LogObject::DebugLevels::ErrorOnly,"Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Log.println(LogObject::DebugLevels::ErrorOnly,"Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Log.println(LogObject::DebugLevels::ErrorOnly,"Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Log.println(LogObject::DebugLevels::ErrorOnly,"Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Log.println(LogObject::DebugLevels::ErrorOnly,"Receive Failed");
    else if (error == OTA_END_ERROR) Log.println(LogObject::DebugLevels::ErrorOnly,"End Failed");
  });
  ArduinoOTA.begin();
  Log.println(LogObject::DebugLevels::Normal,"OTA ready");
}

void checkForUpdates() {
  //String mac = getMAC();
  String fwURL = String( fwUrlBase );
  //fwURL.concat( mac );
  String fwVersionURL = fwURL;
  fwVersionURL.concat( "current.ver" );

  Serial.println( "Checking for firmware updates." );
  //Serial.print( "MAC address: " );
  //Serial.println( mac );
  Serial.print( "Firmware version URL: " );
  Serial.println( fwVersionURL );

  HTTPClient httpClient;
  httpClient.setReuse(true);
  httpClient.begin( fwVersionURL );
  int httpCode = httpClient.GET();
  if( httpCode == 200 ) {
    String newFWVersion = httpClient.getString();

    Serial.print( "Current firmware version: " );
    Serial.println( FW_VERSION );
    Serial.print( "Available firmware version: " );
    Serial.println( newFWVersion );

    int newVersion = newFWVersion.toInt();

    if( newVersion > FW_VERSION ) {
      Serial.println( "Preparing to update" );

      String fwImageURL = fwURL;
      fwImageURL.concat( "firmware.bin" );
      t_httpUpdate_return ret = ESPhttpUpdate.update( fwImageURL );

      switch(ret) {
        case HTTP_UPDATE_FAILED:
          Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
          break;

        case HTTP_UPDATE_NO_UPDATES:
          Serial.println("HTTP_UPDATE_NO_UPDATES");
          break;
      }
    }
    else {
      Serial.println( "Already on latest version" );
    }
  }
  else {
    Serial.print( "Firmware version check failed, got HTTP response code " );
    Serial.println( httpCode );
  }
  httpClient.end();
}

String getMAC()
{
  uint8_t mac[6];
  char result[14];

 snprintf( result, sizeof( result ), "%02x%02x%02x%02x%02x%02x", mac[ 0 ], mac[ 1 ], mac[ 2 ], mac[ 3 ], mac[ 4 ], mac[ 5 ] );

  return String( result );
}