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
  _Log.print(LogObject::DebugLevels::Normal,MSG_SPIFF_STARTED);
  _Log.println(LogObject::DebugLevels::Verbose,MSG_SPIFF_CONTENTS);
  {
    Dir dir = SPIFFS.openDir("/");
    while (dir.next()) {                      // List the file system contents
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      _Log.printf(LogObject::DebugLevels::Verbose,MSG_SPIFF_FILE_TEMPLATE, fileName.c_str(), formatBytes(fileSize).c_str());
    }
    _Log.print(LogObject::DebugLevels::Verbose,"\n");
  }
}
else
{
  _Log.print(LogObject::DebugLevels::Normal,MSG_SPIFF_ERROR);
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
  
  strcpy(_config.dom_server, dom_server_param.getValue());//update config value from web form
  doc[CFG_JSON_NODE_DOM_SERVER] = _config.dom_server;//build json
  strcpy(_config.dom_port, dom_port_param.getValue());
  doc[CFG_JSON_NODE_DOM_PORT] = _config.dom_port;  

  JsonArray jsonProbes = doc[CFG_JSON_NODE_PROBES_ARRAY].to<JsonArray>();

  for (int i = 0; i < probe_count; i++) {
    JsonObject jsonProbe  = jsonProbes.createNestedObject();
    strcpy(_config.probe[i].probe_name, probe_name_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_NAME]=_config.probe[i].probe_name;
    strcpy(_config.probe[i].resistor, resistor_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_RESISTOR]=_config.probe[i].resistor;
    strcpy(_config.probe[i].treshold, treshold_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_TRESHOLD]=_config.probe[i].treshold;
    strcpy(_config.probe[i].idx, idx_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_IDX]=_config.probe[i].idx;
    strcpy(_config.probe[i].number_of_Samples, number_of_Samples_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_NBSAMPLE]=_config.probe[i].number_of_Samples;
    strcpy(_config.probe[i].voltage, voltage_param[i].getValue());
    jsonProbe[CFG_JSON_NODE_PROBE_VOLTAGE]=_config.probe[i].voltage;
    _config.probe[i].send_on_change = _wm.server->arg(CFG_JSON_NODE_PROBE_SENDONCHANGE + String(i))[0] == '1';
    strcpy(_config.probe[i].send_on_change_input, setCheckBox(_config.probe[i].send_on_change, i).c_str());
    jsonProbe[CFG_JSON_NODE_PROBE_SENDONCHANGE]=_config.probe[i].send_on_change;
  }
  //save value
  _Log.println(LogObject::DebugLevels::Verbose,MSG_SPIFF_SAVE_CFG);

  File configFile = SPIFFS.open(MSG_SPIFF_PATH_CFG, "w");
  if (!configFile) {
    _Log.println(LogObject::DebugLevels::ErrorOnly,MSG_SPIFF_SAVE_CFG_ERROR);
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
      strcpy(_config.dom_server, doc[CFG_JSON_NODE_DOM_SERVER]);
      dom_server_param.setValue(_config.dom_server,20);
      strcpy(_config.dom_port, doc[CFG_JSON_NODE_DOM_PORT]);
      dom_port_param.setValue(_config.dom_port,6);
      for (uint i = 0; i < doc[CFG_JSON_NODE_PROBES_ARRAY].size(); i++) {
        JsonObject jsonProbe=doc[CFG_JSON_NODE_PROBES_ARRAY][i];
        strcpy(_config.probe[i].probe_name, jsonProbe[CFG_JSON_NODE_PROBE_NAME]);
        probe_name_param[i].setValue(_config.probe[i].probe_name,150);
        
        strcpy(_config.probe[i].resistor, jsonProbe[CFG_JSON_NODE_PROBE_RESISTOR]);
        resistor_param[i].setValue(_config.probe[i].resistor,6);

        strcpy(_config.probe[i].treshold, jsonProbe[CFG_JSON_NODE_PROBE_TRESHOLD]);
        treshold_param[i].setValue(_config.probe[i].treshold,6);
        
        strcpy(_config.probe[i].idx, jsonProbe[CFG_JSON_NODE_PROBE_IDX]);
        idx_param[i].setValue(_config.probe[i].idx,6);
        
        strcpy(_config.probe[i].number_of_Samples, jsonProbe[CFG_JSON_NODE_PROBE_NBSAMPLE]);
        number_of_Samples_param[i].setValue(_config.probe[i].number_of_Samples,6);
        
        strcpy(_config.probe[i].voltage, jsonProbe[CFG_JSON_NODE_PROBE_VOLTAGE]);
        voltage_param[i].setValue(_config.probe[i].voltage,6);
        
        _config.probe[i].send_on_change=jsonProbe[CFG_JSON_NODE_PROBE_SENDONCHANGE];
        strcpy(_config.probe[i].send_on_change_input, setCheckBox(_config.probe[i].send_on_change,i).c_str());
      }

      _Log.println(LogObject::DebugLevels::Normal,MSG_SPIFF_LOAD_CFG);
    } else {
      _Log.print(LogObject::DebugLevels::ErrorOnly,MSG_SPIFF_SAVE_CFG_DESERIALIZE_ERROR);
      _Log.println(LogObject::DebugLevels::ErrorOnly,err.c_str());
    }
  }
}


bool sendProbesConfig(String path)
{
  _Log.println(LogObject::DebugLevels::Normal,MSG_API_PROBES_CFG);
  
  sendHeader();
  _wm.server->client().print("{\"probes\":[");

  for (int s = 0; s < probe_count; s++) {
    if (s > 0) _wm.server->client().print(",");
    _wm.server->client().print("{");
    sendJsonProperty("name",_config.probe[s].probe_name);
    _wm.server->client().print(",");
    sendJsonProperty("color",_config.probe[s].color);
    _wm.server->client().print("}");
  }

  _wm.server->client().print("]}");
  delay(1);
  _wm.server->client().stop();

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