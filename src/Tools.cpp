#include "Tools.h"
#include "Global.h"

const char *RESP_CODE_OK  PROGMEM="HTTP/1.1 200 OK";
const char *RESP_TYPE_HTML  PROGMEM="Content-Type: text/html";
const char *RESP_CLOSE  PROGMEM="Connection: close";

const char *URL_JSON_DOM_PATERN  PROGMEM="http://%s:%s/json.htm?type=command&param=udevice&nvalue=0&idx=%s&svalue=%s";



String urlBuilder(char* idx,char* svalue)
{
  char url[256];
  sprintf(url,(const char*)FPSTR(URL_JSON_DOM_PATERN),_config.dom_server,_config.dom_port,idx,svalue);
  return url;
}
///
/// return html checkbox input text
///
String setCheckBox(bool f, int s)
{
  String patern = FPSTR(send_on_change_patern);
  String name = "send_on_change" + String(s);
  patern.replace("{n}", name);
  patern.replace("{c}", f ? "checked" : "");
  return patern;
}

void sendHeader()
{
  _wm.server->client().println(FPSTR(RESP_CODE_OK));
  _wm.server->client().println(FPSTR(RESP_TYPE_HTML));
  _wm.server->client().println(FPSTR(RESP_CLOSE));
  _wm.server->client().println();
}

void sendJsonProperty(const char* propertyName,const char* value)
{
  _wm.server->client().print("\"");
  _wm.server->client().print(propertyName);
  _wm.server->client().print("\":\"");
  _wm.server->client().print(value);
  _wm.server->client().print("\"");
}

void sendJsonProperty(const char* propertyName,const double value)
{
  _wm.server->client().print("\"");
  _wm.server->client().print(propertyName);
  _wm.server->client().print("\":");
  _wm.server->client().print(value);
}
