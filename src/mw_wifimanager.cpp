//-------------------------------------------------------------------------------------------
// Wifimanager
//--------------------------------------------------------------------------------------------
#include "mw_wifimanager.h"
#include "Global.h"
#include <WiFiManager.h>
#include "config.h"

void setWifiManagerMenu()
{
  std::vector<const char *> menu = {"wifi", "info", "param","test", "sep", "restart"};
  wm.setMenu(menu);
  wm.setDebugOutput(Log.level==LogObject::DebugLevels::Verbose);
  wm.setSaveParamsCallback(saveParam);
  wm.setConfigPortalBlocking(true);

  wm.addParameter(&dom_server_param);
  wm.addParameter(&dom_port_param);
  wm.addParameter(&wm_time);

  for (int i = 0; i < probe_count; i++) {
    //add to wifimanager engine
    wm.addParameter(&title_text[i]);
    wm.addParameter(&probe_name_param[i]);
    wm.addParameter(&resistor_param[i]);
    wm.addParameter(&treshold_param[i]);
    wm.addParameter(&idx_param[i]);
    wm.addParameter(&number_of_Samples_param[i]);
    wm.addParameter(&voltage_param[i]);
    wm.addParameter(&on_change_param[i]);
    wm.addParameter(&measure_text[i]);
  }

  wm.addParameter(&dc_offset);
  wm.addParameter(&btnGraph);
  wm.addParameter(&btnReset);
}
