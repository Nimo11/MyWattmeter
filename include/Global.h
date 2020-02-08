#ifndef Global_h
#define Global_h

#include "LogObject.h"
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include "config.h"

enum NtpStates
{
  waiting,
  Ok,
  Error
};

extern NtpStates ntpState;
extern LogObject Log;
extern HTTPClient httpClient;
extern WiFiClient client;
extern WiFiManager wm;
extern WiFiUDP Udp;

extern int probe_count;
extern double offsetI; //hold ref dc value in mv
extern char offset_txt[50];
extern char time_txt[50];
extern double filteredI;
extern double sqI, sumI;
extern int16_t sampleI;
extern double Irms;


extern const int blockSize;

extern Config config;

extern const String send_on_change_patern ;
extern const char *DCPatern ;
extern const char *TimePatern ;
extern const char *MeasurePatern ;

extern WiFiManagerParameter dom_server_param;
extern WiFiManagerParameter dom_port_param;

extern WiFiManagerParameter btnGraph;
extern WiFiManagerParameter btnReset;

extern WiFiManagerParameter probe_name_param[4];
extern WiFiManagerParameter resistor_param[4] ;
extern WiFiManagerParameter treshold_param[4] ;
extern WiFiManagerParameter idx_param[4] ;
extern WiFiManagerParameter number_of_Samples_param[4] ;
extern WiFiManagerParameter voltage_param[4] ;

extern WiFiManagerParameter title_text[4];

extern WiFiManagerParameter on_change_param[4];

extern WiFiManagerParameter measure_text[4];

extern WiFiManagerParameter dc_offset;
extern WiFiManagerParameter wm_time;

extern const String dayName[];

extern const char powerFileName[];
extern const char measureFileName[];

#endif