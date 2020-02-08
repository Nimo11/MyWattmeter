#ifndef tools_h
#define tools_h

#include <Arduino.h>

extern String getTimeString(int delta);

extern String urlBuilder(char* idx,char* svalue);

extern String setCheckBox(bool f, int s);

extern void sendHeader();

extern void sendJsonProperty(const char* propertyName,const char* value);

extern void sendJsonProperty(const char* propertyName,const double value);

#endif