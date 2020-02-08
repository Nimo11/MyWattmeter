
#include "power.h"
#include "time.h"
#include "global.h"
#include "config.h"
#include "Tools.h"

void calcPower(int s, double value)
{

  if (weekday() == weekday(config.probe[s].lastChange))
  {
    calcPower2(s, config.probe[s].lastValue, value, config.probe[s].lastChange, now());
    return;
  }

  //we have change of day since last measure
  config.probe[s].power[weekday() - 1] = 0; //reset counter
  
calcPower2(s, config.probe[s].lastValue, value, config.probe[s].lastChange, now());
return;

  tmElements_t e;
  e.Day = day();
  e.Month = month();
  e.Year = year();
  time_t dayStart;
  dayStart = makeTime(e);

  float ratioT = ((float)(now() - dayStart)) / ((float)(now() - config.probe[s].lastChange));
  float midnightI = value + (float)(value - config.probe[s].lastValue) * ratioT;

  //lastday power
  calcPower2(s, config.probe[s].lastValue, midnightI, config.probe[s].lastChange, dayStart);
  //today power
  calcPower2(s, midnightI, value, dayStart, now());
}

void calcPower2(int s, float firstValue, float lastValue, time_t fromTime, time_t toTime)
{
  float deltaI = (float)(lastValue - firstValue);
  float deltaT = ((float)(toTime - fromTime));
  deltaI = deltaI / 2;
  float avgI = (float)lastValue - deltaI;

  config.probe[s].power[weekday(fromTime) - 1] += avgI * deltaT * 0.061111F; //*220/3600=0,0611

  //store power in data file
  char fileName[20];
  sprintf(fileName, powerFileName, s, weekday(fromTime) - 1);
  File powerDataFile = SPIFFS.open(fileName, "w");
  powerDataFile.print(config.probe[s].power[weekday(fromTime) - 1]);
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

  WiFiClient client = wm.server->client();
  sendHeader();

  //send labels list
  client.print("{\"labels\":[");
  client.print("\"" + dayName[dd[0]] + "\"");
  for (int d = 1; d < 7; d++)
    client.print(",\"" + dayName[dd[d]] + "\"");
  client.print("],");

  //send datas list
  client.print("\"datasets\":[");
  for (int s = 0; s < 4; s++)
  {
    if (s > 0)
      client.print(",");
    client.print("{\"label\":\"");
    client.print(config.probe[s].probe_name);
    client.print("\",");
    client.print("\"data\":[" + String(config.probe[s].power[dd[0]]));
    for (int d = 1; d < 7; d++)
      client.print("," + String(config.probe[s].power[dd[d]]));
    client.print("],\"backgroundColor\":\"");
    client.print(config.probe[s].color);
    client.print("\"}");
  }
  client.print("]}");

  delay(1);
  client.stop();

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
      sprintf(fileName, powerFileName, s, d);
      File file = SPIFFS.open(fileName, "r");

      if (!file)
      {
        Log.print(LogObject::DebugLevels::ErrorOnly, MSG_POWER_READ_FAILED);
        Log.println(LogObject::DebugLevels::ErrorOnly, fileName);
      }
      else
      {
        String v;
        while (file.available())
        {
          v += char(file.read());
        }

        config.probe[s].power[d] = v.toDouble();
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
      sprintf(fileName, powerFileName, s, d);
      SPIFFS.remove(fileName);
      config.probe[s].power[d] = 0;
    }
  }
}
