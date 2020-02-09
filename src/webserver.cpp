//--------------------------------------------------------------------------------------
//  Webserver
//--------------------------------------------------------------------------------------
#include "webserver.h"
#include "Tools.h"
#include "Global.h"
#include "config.h"
#include "measure.h"
#include "power.h"

void handleWebRequests() {
  if (loadFromSpiffs(wm.server->uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += wm.server->uri();
  message += "\nMethod: ";
  message += (wm.server->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += wm.server->args();
  message += "\n";
  for (uint8_t i = 0; i < wm.server->args(); i++) {
    message += " NAME:" + wm.server->argName(i) + "\n VALUE:" + wm.server->arg(i) + "\n";
  }
  wm.server->send(404, "text/plain", message);
  Log.println(LogObject::DebugLevels::ErrorOnly,message);
}
// get file from spiffs
bool loadFromSpiffs(String path) {
  String dataType = "text/plain";
  if (path.endsWith("/")) path += "index.htm";

  if (path=="/probesconfig") return sendProbesConfig(path);

  if (path=="/probesvalues") return sendProbesValues(path);
  if (path.startsWith("/probevalue")) return sendProbeValue(path);
  
  if (path=="/measures") return sendMeasureJson(path);
  if (path=="/power") return sendPowerJson(path);
  if (path=="/profiles") return sendProfileJson(path);
  if (path=="/cmdreset") return handleReset();

  wm.server->sendHeader(F("Cache-Control"), F("max-age=290304000, public"));

  if (path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if (path.endsWith(".html")||path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".htm")) dataType = "text/html";
  else if (path.endsWith(".css")) dataType = "text/css";
  else if (path.endsWith(".js")) dataType = "application/javascript";
  else if (path.endsWith(".png")) dataType = "image/png";
  else if (path.endsWith(".gif")) dataType = "image/gif";
  else if (path.endsWith(".jpg")) dataType = "image/jpeg";
  else if (path.endsWith(".ico")) dataType = "image/x-icon";
  else if (path.endsWith(".xml")) dataType = "text/xml";
  else if (path.endsWith(".pdf")) dataType = "application/pdf";
  else if (path.endsWith(".zip")) dataType = "application/zip";

  if (path.startsWith("/script.js")||path.startsWith("/favicon.ico"))
  {
    path+=".gz";
  };

  Log.print(LogObject::DebugLevels::Verbose,"Page request :");
  Log.println(LogObject::DebugLevels::Verbose,path.c_str());
  File dataFile = SPIFFS.open(path.c_str(), "r");

  if (wm.server->hasArg("download")) dataType = "application/octet-stream";
  if (wm.server->streamFile(dataFile, dataType) != dataFile.size()) {}

  dataFile.close();
  return true;
}

bool handleReset()
{
  initDataFile();
  initPowerFile();
  
  WiFiClient  client = wm.server->client();
  sendHeader();
  client.println(FPSTR(HTTP_STYLE));
  client.println("<div class='wrap'><div class='msg'>Saved<br/></div></div>");
  delay(1);
  client.stop();
  
  return true;
}
