#include "Global.h"

const int _FW_VERSION = 1.0;
const char* _fwUrlBase = "https://raw.githubusercontent.com/Nimo11/MyWattmeter/master/.pio/build/esp12e/";


LogObject _Log;
HTTPClient _httpClient;
WiFiClient _client;
WiFiManager _wm;
NtpStates ntpState;
WiFiUDP _Udp;

double offsetI = 1650; //hold ref dc value in mv
char offset_txt[50];
char time_txt[50];
double filteredI;
double sqI, sumI;
int16_t sampleI;
double Irms;
int probe_count=4;

const int blockSize = 1024;

Config _config;

const char *send_on_change_patern PROGMEM= "<p>Send only on value change :<input type='checkbox' id='{n}' name='{n}' value='1' {c}></p>";
const char *DCPatern PROGMEM= "<p>DC offset:  %.2fmv</p>";
const char *MeasurePatern PROGMEM= "<p>Derni&egrave;re mesure:  %.3fA (%.2fW) - %.2fWh</p>";
const char *TimePatern PROGMEM= "<p><b>Time : %s</b></p>";
const String dayName[] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};

const char *WM_INPUT_SERVER_LABEL = "Server IP";
const char *WM_INPUT_PORT_LABEL = "Port";
const char *WM_INPUT_NAME_LABEL = "Name";
const char *WM_INPUT_RESISTOR_LABEL = "Resistor";
const char *WM_INPUT_ID_LABEL = "IDx";
const char *WM_INPUT_SAMPLE_LABEL = "Number of samples";
const char *WM_INPUT_VOLTAGE_LABEL = "Voltage";
const char *WM_INPUT_TRESHOLD_LABEL = "Treshold";

const char *WM_INPUT_POWER_BTN PROGMEM= "<button onclick=\"window.location.href = '/power.html';return false;\">Graph</button><br><br>";
const char *WM_INPUT_RESET_BTN PROGMEM= "<button onclick=\"window.location.href = '/cmdreset';return false;\">Reset history</button>";

const char *powerFileName PROGMEM="/data/energy/%02d/%02d.js";
const char *measureFileName PROGMEM="/data/measure/%d.js";

WiFiManagerParameter dom_server_param("server",WM_INPUT_SERVER_LABEL , _config.dom_server, 40);
WiFiManagerParameter dom_port_param("port",WM_INPUT_PORT_LABEL , _config.dom_port, 6);

WiFiManagerParameter btnGraph((const char*)FPSTR(WM_INPUT_POWER_BTN));
WiFiManagerParameter btnReset((const char*)FPSTR(WM_INPUT_RESET_BTN));

WiFiManagerParameter probe_name_param[4] = {
    WiFiManagerParameter("name1", WM_INPUT_NAME_LABEL, _config.probe[0].probe_name, 100),
    WiFiManagerParameter("name2", WM_INPUT_NAME_LABEL, _config.probe[1].probe_name, 100),
    WiFiManagerParameter("name3", WM_INPUT_NAME_LABEL, _config.probe[2].probe_name, 100),
    WiFiManagerParameter("name4", WM_INPUT_NAME_LABEL, _config.probe[3].probe_name, 100)};

WiFiManagerParameter resistor_param[4] = {
    WiFiManagerParameter("resistor1", WM_INPUT_RESISTOR_LABEL, _config.probe[0].resistor, 6),
    WiFiManagerParameter("resistor2", WM_INPUT_RESISTOR_LABEL, _config.probe[1].resistor, 6),
    WiFiManagerParameter("resistor3", WM_INPUT_RESISTOR_LABEL, _config.probe[2].resistor, 6),
    WiFiManagerParameter("resistor4", WM_INPUT_RESISTOR_LABEL, _config.probe[3].resistor, 6)};

WiFiManagerParameter idx_param[4] = {
    WiFiManagerParameter("idx1", WM_INPUT_ID_LABEL, _config.probe[0].idx, 6),
    WiFiManagerParameter("idx2", WM_INPUT_ID_LABEL, _config.probe[1].idx, 6),
    WiFiManagerParameter("idx3", WM_INPUT_ID_LABEL, _config.probe[2].idx, 6),
    WiFiManagerParameter("idx4", WM_INPUT_ID_LABEL, _config.probe[3].idx, 6)};

WiFiManagerParameter number_of_Samples_param[4] = {
    WiFiManagerParameter("number_of_Samples1", WM_INPUT_SAMPLE_LABEL, _config.probe[0].number_of_Samples, 6),
    WiFiManagerParameter("number_of_Samples2", WM_INPUT_SAMPLE_LABEL, _config.probe[1].number_of_Samples, 6),
    WiFiManagerParameter("number_of_Samples3", WM_INPUT_SAMPLE_LABEL, _config.probe[2].number_of_Samples, 6),
    WiFiManagerParameter("number_of_Samples4", WM_INPUT_SAMPLE_LABEL, _config.probe[3].number_of_Samples, 6)};

WiFiManagerParameter voltage_param[4] = {
    WiFiManagerParameter("voltage1", WM_INPUT_VOLTAGE_LABEL, _config.probe[0].voltage, 6),
    WiFiManagerParameter("voltage2", WM_INPUT_VOLTAGE_LABEL, _config.probe[1].voltage, 6),
    WiFiManagerParameter("voltage3", WM_INPUT_VOLTAGE_LABEL, _config.probe[2].voltage, 6),
    WiFiManagerParameter("voltage4", WM_INPUT_VOLTAGE_LABEL, _config.probe[3].voltage, 6)};

WiFiManagerParameter title_text[4] = {
    WiFiManagerParameter("<h1>Sonde 1:</h1>"),
    WiFiManagerParameter("<h1>Sonde 2:</h1>"),
    WiFiManagerParameter("<h1>Sonde 3:</h1>"),
    WiFiManagerParameter("<h1>Sonde 4:</h1>")};

WiFiManagerParameter on_change_param[4] = {
    WiFiManagerParameter(_config.probe[0].send_on_change_input),
    WiFiManagerParameter(_config.probe[1].send_on_change_input),
    WiFiManagerParameter(_config.probe[2].send_on_change_input),
    WiFiManagerParameter(_config.probe[3].send_on_change_input)};

WiFiManagerParameter measure_text[4] = {
    WiFiManagerParameter(_config.probe[0].txt_measure),
    WiFiManagerParameter(_config.probe[1].txt_measure),
    WiFiManagerParameter(_config.probe[2].txt_measure),
    WiFiManagerParameter(_config.probe[3].txt_measure)};

WiFiManagerParameter treshold_param[4] = {
    WiFiManagerParameter("treshold1", WM_INPUT_TRESHOLD_LABEL, _config.probe[0].treshold, 6),
    WiFiManagerParameter("treshold2", WM_INPUT_TRESHOLD_LABEL, _config.probe[1].treshold, 6),
    WiFiManagerParameter("treshold3", WM_INPUT_TRESHOLD_LABEL, _config.probe[2].treshold, 6),
    WiFiManagerParameter("treshold4", WM_INPUT_TRESHOLD_LABEL, _config.probe[3].treshold, 6)};

WiFiManagerParameter dc_offset = WiFiManagerParameter(offset_txt);
WiFiManagerParameter wm_time = WiFiManagerParameter(time_txt);


