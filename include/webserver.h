//--------------------------------------------------------------------------------------
//  Webserver
//--------------------------------------------------------------------------------------
#ifndef webserver_h
#define webserver_h

#include <Arduino.h>

extern void handleWebRequests();

// get file from spiffs
extern bool loadFromSpiffs(String path);

extern bool handleReset();

#endif
