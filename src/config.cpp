//--------------------------------------------------------------------------------------
//  Config manager
//--------------------------------------------------------------------------------------
#include "config.h"
#include <FS.h>
#include <LogObject.h>
#include "Global.h"
#include <ArduinoJson.h>
#include "Tools.h"



void startSPIFFS() { // Start the SPIFFS and list all contents
  if (SPIFFS.begin()){                             // Start the SPI Flash File System (SPIFFS)
  Log.print(LogObject::DebugLevels::Normal,MSG_SPIFF_STARTED);
  Log.println(LogObject::DebugLevels::Verbose,MSG_SPIFF_CONTENTS);
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      Log.printf(LogObject::DebugLevels::Verbose,MSG_SPIFF_FILE_TEMPLATE, fileName.c_str(), formatBytes(fileSize).c_str());
    }
    Log.print(LogObject::DebugLevels::Verbose,"\n");
  }
}
else
{
  Log.print(LogObject::DebugLevels::Normal,MSG_SPIFF_ERROR);
}

}

String formatBytes(size_t bytes) { // convert sizes in bytes to KB and MB
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }

  return String(bytes);
}

void saveParam()
{
  StaticJsonDocument<1024> doc;
  
  strcpy(config.dom_server, dom_server_param.getValue());//update config value from web form
  doc[CFG_JSON_NODE_DOM_SERVER] = config.dom_server;//build json
  strcpy(config.dom_port, dom_port_param.getValue());
  doc[CFG_JSON_NODE_DOM_PORT] = config.dom_port;  

  JsonArray jsonProbes = doc[CFG_JSON_NODE_PROBES_ARRAY].to<JsonArray>();

  for (int i = 0; i < probe_count; i++) {
    JsonObject jsonProbe  = jsonProbes.createNestedObject();
    strcpy(config.probe[i].probe_name, probe_name_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_NAME]=config.probe[i].probe_name;
    strcpy(config.probe[i].resistor, resistor_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_RESISTOR]=config.probe[i].resistor;
    strcpy(config.probe[i].treshold, treshold_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_TRESHOLD]=config.probe[i].treshold;
    strcpy(config.probe[i].idx, idx_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_IDX]=config.probe[i].idx;
    strcpy(config.probe[i].number_of_Samples, number_of_Samples_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_NBSAMPLE]=config.probe[i].number_of_Samples;
    strcpy(config.probe[i].voltage, voltage_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_VOLTAGE]=config.probe[i].voltage;
    config.probe[i].send_on_change = wm.server->arg(CFG_JSON_NODE_PROBE_SENDONCHANGE + String(i))[0] == '1';
    strcpy(config.probe[i].send_on_change_input, setCheckBox(config.probe[i].send_on_change, i).c_str());
    jsonProbe[CFG_JSON_NODE_PROBE_SENDONCHANGE]=config.probe[i].send_on_change;
  }
  //save value
  Log.println(LogObject::DebugLevels::Verbose,MSG_SPIFF_SAVE_CFG);

  File configFile = SPIFFS.open(MSG_SPIFF_PATH_CFG, "w");
  if (!configFile) {
    Log.println(LogObject::DebugLevels::ErrorOnly,MSG_SPIFF_SAVE_CFG_ERROR);
  }

  serializeJson(doc, configFile);
  configFile.close();
}

void loadConfig()
{  
  //read configuration from FS json
  File configFile = SPIFFS.open(MSG_SPIFF_PATH_CFG, "r");

  if (configFile) {
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);

    configFile.readBytes(buf.get(), size);
    DynamicJsonDocument doc(size);
    DeserializationError err = deserializeJson(doc, buf.get());

    configFile.close();

    if (err==DeserializationError::Ok) {
      strcpy(config.dom_server, doc[CFG_JSON_NODE_DOM_SERVER]);
      dom_server_param.setValue(config.dom_server,20);
      strcpy(config.dom_port, doc[CFG_JSON_NODE_DOM_PORT]);
      dom_port_param.setValue(config.dom_port,6);
      for (uint i = 0; i < doc[CFG_JSON_NODE_PROBES_ARRAY].size(); i++) {
        JsonObject jsonProbe=doc[CFG_JSON_NODE_PROBES_ARRAY][i];
        strcpy(config.probe[i].probe_name, jsonProbe[CFG_JSON_NODE_PROBE_NAME]);
        probe_name_param[i].setValue(config.probe[i].probe_name,150);
        
        strcpy(config.probe[i].resistor, jsonProbe[CFG_JSON_NODE_PROBE_RESISTOR]);
        resistor_param[i].setValue(config.probe[i].resistor,6);

        strcpy(config.probe[i].treshold, jsonProbe[CFG_JSON_NODE_PROBE_TRESHOLD]);
        treshold_param[i].setValue(config.probe[i].treshold,6);
        
        strcpy(config.probe[i].idx, jsonProbe[CFG_JSON_NODE_PROBE_IDX]);
        idx_param[i].setValue(config.probe[i].idx,6);
        
        strcpy(config.probe[i].number_of_Samples, jsonProbe[CFG_JSON_NODE_PROBE_NBSAMPLE]);
        number_of_Samples_param[i].setValue(config.probe[i].number_of_Samples,6);
        
        strcpy(config.probe[i].voltage, jsonProbe[CFG_JSON_NODE_PROBE_VOLTAGE]);
        voltage_param[i].setValue(config.probe[i].voltage,6);
        
        config.probe[i].send_on_change=jsonProbe[CFG_JSON_NODE_PROBE_SENDONCHANGE];
        strcpy(config.probe[i].send_on_change_input, setCheckBox(config.probe[i].send_on_change,i).c_str());
      }

      Log.println(LogObject::DebugLevels::Normal,MSG_SPIFF_LOAD_CFG);
    } else {
      Log.print(LogObject::DebugLevels::ErrorOnly,MSG_SPIFF_SAVE_CFG_DESERIALIZE_ERROR);
      Log.println(LogObject::DebugLevels::ErrorOnly,err.c_str());
    }
  }
}


bool sendProbesConfig(String path)
{
  Log.println(LogObject::DebugLevels::Normal,MSG_API_PROBES_CFG);
  
  sendHeader();
  wm.server->client().print("{\"probes\":[");

  for (int s = 0; s < probe_count; s++) {
    if (s > 0) wm.server->client().print(",");
    wm.server->client().print("{");
    sendJsonProperty("name",config.probe[s].probe_name);
    wm.server->client().print(",");
    sendJsonProperty("color",config.probe[s].color);
    wm.server->client().print("}");
  }

  wm.server->client().print("]}");
  delay(1);
  wm.server->client().stop();

  return true;
}

void TestSpiffs()
{
  //SPIFFS.format();
  File file = SPIFFS.open("/file.txt", "w");
 
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
 
  int bytesWritten = file.print("TEST SPIFFS");
if (bytesWritten > 0) {
    Serial.println("File was written");
    Serial.println(bytesWritten);
 
  } else {
    Serial.println("File write failed");
  }
 file.close();
     file = SPIFFS.open("/file.txt", "r");
    while (file.available()){
      Serial.write(file.read());
    }
  file.close();
}