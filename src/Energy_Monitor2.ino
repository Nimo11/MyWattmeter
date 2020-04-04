#include "Global.h"
#include <TimeLib.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#include <ArduinoOTA.h>
#include <Ethernet.h>
#include "config.h"
#include "ntp.h"
#include "ota.h"
#include "Tools.h"
#include "DataBlock.h"
#include "ADS1115.h"
#include "power.h"
#include "measure.h"
#include "I2C.h"
#include "webserver.h"
#include "mw_wifimanager.h"
#include "LogObject.h"

void setup()
{

  strcpy(_config.probe[1].color, "rgba(255,99,132)");
  strcpy(_config.probe[2].color, "rgba(99,255,132)");
  strcpy(_config.probe[3].color, "rgba(99,99,132)");

  Serial.begin(115200);
  delay(10);
  Serial.println("");

  setupI2C();

  startSPIFFS();

  _Log.level=LogObject::DebugLevels::Verbose;
  _Log.fileName="/debug.log";
  _Log.initLogFile();
  _Log.debugType=LogObject::DebugType::FileType;

  loadConfig();

  loadPowerCount();

  setWifiManagerMenu();

  if (_wm.autoConnect("My_Wattmeter"))
  {
    _Log.print(LogObject::DebugLevels::Normal, F("IP Address "));
    _Log.println(LogObject::DebugLevels::Normal, WiFi.localIP().toString());
    WiFi.mode(WIFI_STA);
    WiFi.hostname(F("My_Wattmetre"));

    setNTP();

    setOTA();

    _wm.startWebPortal();

    _wm.server->onNotFound(handleWebRequests);

    scanI2C();

    //checkForUpdates();
  }
  else
  {
    _Log.println(LogObject::DebugLevels::Normal, F("Config portal running!"));
  }
}

void loop()
{
  unsigned long time_ref = millis() + 1000;

  while (millis() < time_ref)
  {
    _wm.process();
    ArduinoOTA.handle();
  }

  for (int i = 0; i < probe_count; i++)
  {
    _wm.process();
    ArduinoOTA.handle();
    if (ntpState != NtpStates::waiting && atoi(_config.probe[i].idx) != 0 && strlen(_config.probe[i].idx) > 0)
      measure(i);
  }
  
  sprintf(offset_txt,  (const char*)FPSTR(DCPatern), offsetI);
  sprintf(time_txt, (const char*)FPSTR(TimePatern), LogObject::getTimeString(0).c_str());
}