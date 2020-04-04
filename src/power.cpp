
#include "power.h"
#include "time.h"
#include "global.h"
#include "config.h"
#include "Tools.h"

void calcPower(int s, double value)
{

  if (weekday() == weekday(_config.probe[s].lastChange))
  {
    calcPower2(s, _config.probe[s].lastValue, value, _config.probe[s].lastChange, now());
    return;
  }

  //we have change of day since last measure
  _config.probe[s].power[weekday() - 1] = 0; //reset counter
  
calcPower2(s, _config.probe[s].lastValue, value, _config.probe[s].lastChange, now());
return;

  tmElements_t e;
  e.Day = day();
  e.Month = month();
  e.Year = year();
  time_t dayStart;
  dayStart = makeTime(e);

  float ratioT = ((float)(now() - dayStart)) / ((float)(now() - _config.probe[s].lastChange));
  float midnightI = value + (float)(value - _config.probe[s].lastValue) * ratioT;

  //lastday power
  calcPower2(s, _config.probe[s].lastValue, midnightI, _config.probe[s].lastChange, dayStart);
  //today power
  calcPower2(s, midnightI, value, dayStart, now());
}

void calcPower2(int s, float firstValue, float lastValue, time_t fromTime, time_t toTime)
{
  float deltaI = (float)(lastValue - firstValue);
  float deltaT = ((float)(toTime - fromTime));
  deltaI = deltaI / 2;
  float avgI = (float)lastValue - deltaI;

  _config.probe[s].power[weekday(fromTime) - 1] += avgI * deltaT * 0.061111F; //*220/3600=0,0611

  //store power in data file
  char fileName[20];
  sprintf(fileName, (const char*)FPSTR(powerFileName), s, weekday(fromTime) - 1);
  File powerDataFile = SPIFFS.open(fileName, "w");
  powerDataFile.print(_config.probe[s].power[weekday(fromTime) - 1]);
  powerDataFile.close();
}

//
// Send enregy json file using wificlient
//
bool sendPowerJson(String path)
{
  //build display list
  int dd[7];
  int i = 6;

  int d = weekday();

  while (i > -1)
  {
    d--;
    if (d == -1)
      d = 6;
    dd[i] = d;
    i--;
  }

  WiFiClient _client = _wm.server->client();
  sendHeader();

  //send labels list
  _client.print("{\"labels\":[");
  _client.print("\"" + dayName[dd[0]] + "\"");
  for (int d = 1; d < 7; d++)
    _client.print(",\"" + dayName[dd[d]] + "\"");
  _client.print("],");

  //send datas list
  _client.print("\"datasets\":[");
  for (int s = 0; s < 4; s++)
  {
    if (s > 0)
      _client.print(",");
    _client.print("{\"label\":\"");
    _client.print(_config.probe[s].probe_name);
    _client.print("\",");
    _client.print("\"data\":[" + String(_config.probe[s].power[dd[0]]));
    for (int d = 1; d < 7; d++)
      _client.print("," + String(_config.probe[s].power[dd[d]]));
    _client.print("],\"backgroundColor\":\"");
    _client.print(_config.probe[s].color);
    _client.print("\"}");
  }
  _client.print("]}");

  delay(1);
  _client.stop();

  return true;
}

//
//restaure power value from file
//
void loadPowerCount()
{
  for (int s = 0; s < probe_count; s++)
  {
    for (int d = 0; d < 7; d++)
    {
      char fileName[20];
      sprintf(fileName, (const char*)FPSTR(powerFileName), s, d);
      File file = SPIFFS.open(fileName, "r");

      if (!file)
      {
        _Log.print(LogObject::DebugLevels::ErrorOnly, MSG_POWER_READ_FAILED);
        _Log.println(LogObject::DebugLevels::ErrorOnly, fileName);
      }
      else
      {
        String v;
        while (file.available())
        {
          v += char(file.read());
        }

        _config.probe[s].power[d] = v.toDouble();
      }
    }
  }
}

void initPowerFile()
{
  for (int s = 0; s < probe_count; s++)
  {
    for (int d = 0; d < 7; d++)
    {
      char fileName[20];
      sprintf(fileName, (const char*)FPSTR(powerFileName), s, d);
      SPIFFS.remove(fileName);
      _config.probe[s].power[d] = 0;
    }
  }
}
