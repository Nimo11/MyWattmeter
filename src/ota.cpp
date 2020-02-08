//--------------------------------------------------------------------------------------
//  OTA
//--------------------------------------------------------------------------------------
#include <Arduino.h>
#include <ArduinoOTA.h>
#include "Global.h"


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
