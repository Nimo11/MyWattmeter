//--------------------------------------------------------------------------------------
//  OTA
//--------------------------------------------------------------------------------------
#ifndef ota_h
#define ota_h

#include <Arduino.h>

extern void setOTA();

void checkForUpdates();

String getMAC();

#endif
