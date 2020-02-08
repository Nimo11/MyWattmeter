//--------------------------------------------------------------------------------------
//  Measure
//--------------------------------------------------------------------------------------
#include "measure.h"
#include "Global.h"
#include "config.h"
#include "ADS1115.h"
#include "Time.h"
#include "Tools.h"
#include "power.h"

double calcIrms(int s, unsigned int nos)
{
  float const multiplier = 0.125F; /* ADS1115 @ +/- 4.096V/32767 (gain 1) (signed 16-bit results in mv) */
  readADC_Continuous(s);
  delay(8);
  float probeFactor = atof(config.probe[s].resistor);

  for (unsigned int n = 0; n < nos; n++)
  {
    sampleI = getLastConversionResults() * multiplier;

    // Digital low pass filter extracts the 1.65 V dc offset,
    offsetI = (offsetI + (sampleI - offsetI) / 1024);

    filteredI = (sampleI - offsetI);
    config.probe[s].profile[n] = filteredI;

    filteredI = filteredI * (probeFactor / 1000); //0.03F;//1v 30A / 1000 (mv->v)

    // Root-mean-square method current
    // 1) square current values
    sqI = filteredI * filteredI;
    // 2) sum
    sumI += sqI;
  }

  Irms = sqrt(sumI / nos);

  //Reset accumulators
  sumI = 0;

  Irms -= probeFactor * .00483F; //remove noise .145F

  if (Irms < 0)
    Irms = 0;

  return Irms;
}

void measure(int s)
{
  if (measureDebug)
    Log.printf(LogObject::DebugLevels::Verbose, "Start measure %d(%s) %s samples\n", s, config.probe[s].idx, config.probe[s].number_of_Samples);

  int nos = atoi(config.probe[s].number_of_Samples);
  double value = calcIrms(s, nos);

  if (config.probe[s].treshold && value < atof(config.probe[s].treshold)) value = 0;
  
  double power = value * atoi(config.probe[s].voltage);
  double delta = abs((value - config.probe[s].lastValue) * 5); //2 dixiéme* 10
  if (measureDebug)
    Log.printf(LogObject::DebugLevels::Verbose, "Measure done : %.2fA\n", value);

  if (!config.probe[s].send_on_change || delta > 1)
  {
    //calculate power
    if (config.probe[s].lastChange > 0) //if there is previous measure calculated power
      calcPower(s, value);

    //set config properties
    config.probe[s].lastChange = now();

    config.probe[s].lastValue = value;
    config.probe[s].ignoredCmpt = 0;

    //send last value with current time-1s
    if (config.probe[s].ignoredCmpt > 0)
      sendMeasure(s, config.probe[s].lastValue, false, 1);

    sendMeasure(s, value, true, 0);
  }
  else
  {
    config.probe[s].ignoredCmpt++;
  }

  sprintf(config.probe[s].txt_measure, MeasurePatern, value, power, config.probe[s].power[weekday() - 1]);
}
//send value for a given input using http
void sendMeasure(int s, double v, bool close, int delta)
{
  char cstr[16];
  itoa(v, cstr, 10);
  sendGetRequest(urlBuilder(config.probe[s].idx, cstr), close);

  if (s == 1)
  {
    char sv[20];
    sprintf(sv, "%.2f;%.2f", v * 220, config.probe[s].power[weekday() - 1]);
  }

  //store in data file
  char fileName[20];
  sprintf(fileName, measureFileName, s);
  File measureDataFile = SPIFFS.open(fileName, "a");

  measureDataFile.print(getTimeString(delta));
  measureDataFile.print("|");
  measureDataFile.print(v);
  measureDataFile.print("|");
  measureDataFile.close();

  if (measureDebug)
    Log.printf(LogObject::DebugLevels::Verbose, "%s: Sended input %d (%.2fA)\n", getTimeString(delta).c_str(), s, v);
}

void sendGetRequest(String url, bool close)
{
  //send data to server
  httpClient.begin(client, url);

  int httpCode = httpClient.GET(); //Send the request

  if (httpCode > 0)
  {                                          //Check the returning code
    String payload = httpClient.getString(); //Get the request response payload
  }

  if (close)
    httpClient.end();
}

bool sendProfileJson(String path)
{
  WiFiClient client = wm.server->client();
  sendHeader();
  client.print("{\"datasets\":[");

  for (int s = 0; s < probe_count; s++)
  {
    if (s > 0)
      client.print(",");
    client.print("{\"label\": \"" + String(config.probe[s].probe_name) + "\",\"lineTension\": 0,\"fill\":false,\"borderColor\": \"" + config.probe[s].color + "\",\"data\":[");
    client.print(String(config.probe[s].profile[0]));
    for (int c = 1; c < atoi(config.probe[s].number_of_Samples); c++)
    {
      client.print("," + String(config.probe[s].profile[c]));
    }
    client.print("]}");
  }
  client.print("]}");
  delay(1);
  client.stop();

  return true;
}

bool sendMeasureJson(String path)
{
  Log.println(LogObject::DebugLevels::Verbose, "Sending Measures");
  WiFiClient client = wm.server->client();
  sendHeader();
  client.print("{\"datasets\":[");

  for (int s = 0; s < probe_count; s++)
  {
    if (s > 0)
      client.print(",");
    getJsonFile(s);
  }

  client.print("]}");
  delay(1);
  client.stop();

  return true;
}

void getJsonFile(int s)
{
  WiFiClient client = wm.server->client();
  DataBlock block(1024, client);

  block.addArray("{\"label\": \"", 11);
  block.addArray(config.probe[s].probe_name, 100);
  block.addArray("\",\"lineTension\": 0,\"fill\":false,\"borderColor\": \"", 48);
  block.addArray(config.probe[s].color, 20);
  block.addArray("\",\"data\":[", 10);

  char fileName[20];
  sprintf(fileName, measureFileName, s);
  File dataFile = SPIFFS.open(fileName, "r");

  if (!dataFile)
  {
    Log.println(LogObject::DebugLevels::ErrorOnly, "Failed to open data file");
  }
  else
  {
    bool start = true;

    while (dataFile.available())
    {
      if (!start)
        block.addChar(',');

      block.addArray("{\"x\":\"", 6);
      printNextFileValue(dataFile, block);
      block.addArray("\",\"y\":\"", 6);
      printNextFileValue(dataFile, block);
      block.addChar('}');

      start = false;
    }

    if (!start)
      block.addChar(','); //complete with last value
    block.addArray("{\"x\":\"", 6);
    block.addString(getTimeString(0));
    block.addArray("\",\"y\":\"", 6);
    block.addString(String(config.probe[s].lastValue));
    block.addChar('}');
  }

  block.addArray("]}", 2);

  if (block.pos > 0)
    block.sendBuffer();
  block.pos = 0;
}

void printNextFileValue(File dataFile, DataBlock &block)
{
  char c;
  while (dataFile.available())
  {
    c = char(dataFile.read());
    if (c == '|')
      break;
    block.addChar(c);
  }
}

void initDataFile()
{
  for (int s = 0; s < probe_count; s++)
  {
    initDataFile(s);
    config.probe[s].ignoredCmpt = 10; //force to send first measure
  }
}
void initDataFile(int s)
{
  char fileName[20];
  sprintf(fileName, measureFileName, s);
  SPIFFS.remove(fileName);
  File initFile = SPIFFS.open(fileName, "w");
  initFile.print("");
  initFile.close();

  for (int s = 0; s < probe_count; s++)
  {
    for (int d = 0; d < 7; d++)
    {
      char fileName[20];
      sprintf(fileName, powerFileName, s, d);
      SPIFFS.remove(fileName);
    }
  }
}

bool sendProbeValue(String path)
{
  Log.println(LogObject::DebugLevels::Verbose, "Sending probe value");
  int s = (int)path[path.length() - 1];

  sendHeader();
  wm.server->client().print("{");
  sendJsonProperty("value", config.probe[s].lastValue);
  wm.server->client().print("}");
  delay(1);
  wm.server->client().stop();

  return true;
}

bool sendProbesValues(String path)
{
  Log.println(LogObject::DebugLevels::Verbose, "Sending probes values");
  sendHeader();
  wm.server->client().print("{\"values\":[");

  for (int s = 0; s < probe_count; s++)
  {
    if (s > 0)
      wm.server->client().print(",");
    wm.server->client().print(config.probe[s].lastValue);
  }

  wm.server->client().print("]}");
  delay(1);
  wm.server->client().stop();

  return true;
}
