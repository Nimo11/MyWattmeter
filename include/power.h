
#ifndef power_h
#define power_h

#include <Arduino.h>

const char MSG_POWER_READ_FAILED[]  ="Failed to open power file";

extern void calcPower(int s,double value);

extern void calcPower2(int s,float firstValue,float lastValue,time_t fromTime,time_t toTime);

//
// Send enregy json file using wificlient
//
extern bool sendPowerJson(String path);

//
//load energy value from file
//
extern void loadPowerCount();

extern void initPowerFile();

#endif