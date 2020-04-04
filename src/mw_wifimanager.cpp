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
  _wm.setMenu(menu);
  _wm.setDebugOutput(_Log.level==LogObject::DebugLevels::Verbose);
  _wm.setSaveParamsCallback(saveParam);
  _wm.setConfigPortalBlocking(true);

  _wm.addParameter(&dom_server_param);
  _wm.addParameter(&dom_port_param);
  _wm.addParameter(&wm_time);

  for (int i = 0; i < probe_count; i++) {
    //add to wifimanager engine
    _wm.addParameter(&title_text[i]);
    _wm.addParameter(&probe_name_param[i]);
    _wm.addParameter(&resistor_param[i]);
    _wm.addParameter(&treshold_param[i]);
    _wm.addParameter(&idx_param[i]);
    _wm.addParameter(&number_of_Samples_param[i]);
    _wm.addParameter(&voltage_param[i]);
    _wm.addParameter(&on_change_param[i]);
    _wm.addParameter(&measure_text[i]);
  }

  _wm.addParameter(&dc_offset);
  _wm.addParameter(&btnGraph);
  _wm.addParameter(&btnReset);
}
