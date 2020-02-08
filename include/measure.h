//--------------------------------------------------------------------------------------
//  Measure
//--------------------------------------------------------------------------------------
#ifndef measure_h
#define measure_h

#include <Arduino.h>
#include "FS.h"
#include "DataBlock.h"

const bool measureDebug=false;


double calcIrms(int s, unsigned int nos);

extern void measure(int s);

//send value for a given input using http
extern void sendMeasure(int s, double v, bool close, int delta);

extern void sendGetRequest(String url,bool close);

extern bool sendProfileJson(String path);

extern bool sendMeasureJson(String path);

extern void getJsonFile(int s);

extern void printNextFileValue(File dataFile,DataBlock& block);

extern void initDataFile();

extern void initDataFile(int s);

extern bool sendProbeValue(String path);

extern bool sendProbesValues(String path);
#endif