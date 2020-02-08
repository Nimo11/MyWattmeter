#include "Tools.h"
#include "Time.h"
#include "Global.h"

const char RESP_CODE_OK[]  ="HTTP/1.1 200 OK";
const char RESP_TYPE_HTML[]  ="Content-Type: text/html";
const char RESP_CLOSE[]  ="Connection: close";

const char TIME_PATERN[]  ="%02d-%02d-%02d %02d:%02d:%02d";
const char URL_JSON_DOM_PATERN[]  ="http://%s:%s/json.htm?type=command&param=udevice&nvalue=0&idx=%s&svalue=%s";

String getTimeString(int delta)
{
  char txt_time[20];
  sprintf(txt_time, TIME_PATERN, month(), day(), year(), hour(), minute(), second() - delta);
  return txt_time;
}

String urlBuilder(char* idx,char* svalue)
{
  char url[256];
  sprintf(url,URL_JSON_DOM_PATERN,config.dom_server,config.dom_port,idx,svalue);
  return url;
}

String setCheckBox(bool f, int s)
{
  String patern = send_on_change_patern;
  String name = "send_on_change" + String(s);
  patern.replace("{n}", name);
  patern.replace("{c}", f ? "checked" : "");
  return patern;
}

void sendHeader()
{
  wm.server->client().println(RESP_CODE_OK);
  wm.server->client().println(RESP_TYPE_HTML);
  wm.server->client().println(RESP_CLOSE);
  wm.server->client().println();
}

void sendJsonProperty(const char* propertyName,const char* value)
{
  wm.server->client().print("\"");
  wm.server->client().print(propertyName);
  wm.server->client().print("\":\"");
  wm.server->client().print(value);
  wm.server->client().print("\"");
}

void sendJsonProperty(const char* propertyName,const double value)
{
  wm.server->client().print("\"");
  wm.server->client().print(propertyName);
  wm.server->client().print("\":");
  wm.server->client().print(value);
}
