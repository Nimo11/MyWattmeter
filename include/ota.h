//--------------------------------------------------------------------------------------
//  OTA
//--------------------------------------------------------------------------------------
#ifndef ota_h
#define ota_h

#include <Arduino.h>

extern void setOTA();

extern void checkForUpdates();

String getMAC();

#endif
