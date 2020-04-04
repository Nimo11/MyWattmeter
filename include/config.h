//--------------------------------------------------------------------------------------
//  Config manager
//--------------------------------------------------------------------------------------
#ifndef config_h
#define config_h

#include <Arduino.h>
#include <ArduinoJson.h>

const char MSG_SPIFF_STARTED[]  ="SPIFFS started.";
const char MSG_SPIFF_CONTENTS[]  ="Contents:";
const char MSG_SPIFF_FILE_TEMPLATE[]  ="\tFS File: %s, size: %s\r\n";
const char MSG_SPIFF_ERROR[]  ="SPIFFS error.";
const char MSG_SPIFF_SAVE_CFG[]  ="Saving config";
const char MSG_SPIFF_LOAD_CFG[]  ="Loading config";
const char MSG_SPIFF_SAVE_CFG_ERROR[]  ="Failed to save config file for writing";
const char MSG_SPIFF_SAVE_CFG_DESERIALIZE_ERROR[]  ="Deserialize failed with code :";
const char MSG_SPIFF_PATH_CFG[]  ="/config.json";

const char MSG_API_PROBES_CFG[]  ="Sending probes config";

const char CFG_JSON_NODE_DOM_SERVER[]  ="dom_server";
const char CFG_JSON_NODE_DOM_PORT[]  ="dom_port";
const char CFG_JSON_NODE_PROBES_ARRAY[]  ="probes";
const char CFG_JSON_NODE_PROBE_NAME[]  ="probe_name";
const char CFG_JSON_NODE_PROBE_RESISTOR[]  ="resistor";
const char CFG_JSON_NODE_PROBE_TRESHOLD[]  ="treshold";
const char CFG_JSON_NODE_PROBE_IDX[]  ="idx";
const char CFG_JSON_NODE_PROBE_NBSAMPLE[]  ="number_of_Samples";
const char CFG_JSON_NODE_PROBE_VOLTAGE[]  ="voltage";
const char CFG_JSON_NODE_PROBE_SENDONCHANGE[]  ="send_on_change";


struct Probe
{
  char probe_name[100] = "My Probe";
  int16_t profile[1024];
  char color[20] = "rgba(54,162,235,1)";
  char idx[6] = "0";
  char number_of_Samples[6] = "512";
  char voltage[6] = "220";
  char resistor[6] = "70";
  char treshold[6]; //do not measure hunder this value
  bool send_on_change = true;

  double lastValue = -1;
  int lastChangeDay; //day of last measure
  time_t lastChange; //last measure millis time

  double power[7] = {0, 0, 0, 0, 0, 0, 0};

  int ignoredCmpt = 10; //force first send
  char txt_measure[100] = "<p>Pas de mesure disponible...</p>";
  char send_on_change_input[150] = "-";
};

struct Config
{
  char dom_server[20] = "0.0.0.0";
  char dom_port[6] = "8084";
  Probe probe[4];
};

extern void startSPIFFS();

String formatBytes(size_t bytes);

extern void saveParam();

extern void loadConfig();

extern bool sendProbesConfig(String path);

template <typename T>
void setFromJson(T obj,JsonObject jsonObj,const char propertyName[]);

void TestSpiffs();

#endif