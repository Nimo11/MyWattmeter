#include "Global.h"

LogObject Log;
HTTPClient httpClient;
WiFiClient client;
WiFiManager wm;
NtpStates ntpState;
WiFiUDP Udp;

double offsetI = 1650; //hold ref dc value in mv
char offset_txt[50];
char time_txt[50];
double filteredI;
double sqI, sumI;
int16_t sampleI;
double Irms;
int probe_count=4;

const int blockSize = 1024;

Config config;

const String send_on_change_patern = "<p>Send only on value change :<input type='checkbox' id='{n}' name='{n}' value='1' {c}></p>";
const char *DCPatern = "<p>DC offset:  %.2fmv</p>";
const char *MeasurePatern = "<p>Derni&egrave;re mesure:  %.3fA (%.2fW) - %.2fWh</p>";
const char *TimePatern = "<p><b>Time : %s</b></p>";
const String dayName[] = {"Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi"};

const char powerFileName[]="/data/energy/%02d/%02d.js";
const char measureFileName[]="/data/measure/%d.js";

WiFiManagerParameter dom_server_param("server", "Server IP", config.dom_server, 40);
WiFiManagerParameter dom_port_param("port", "Port", config.dom_port, 6);

WiFiManagerParameter btnGraph("<button onclick=\"window.location.href = '/energy.html';return false;\">Graph</button><br><br>");
WiFiManagerParameter btnReset("<button onclick=\"window.location.href = '/cmdreset';return false;\">Reset history</button>");

WiFiManagerParameter probe_name_param[4] = {
    WiFiManagerParameter("name1", "Name", config.probe[0].probe_name, 100),
    WiFiManagerParameter("name2", "Name", config.probe[1].probe_name, 100),
    WiFiManagerParameter("name3", "Name", config.probe[2].probe_name, 100),
    WiFiManagerParameter("name4", "Name", config.probe[3].probe_name, 100)};

WiFiManagerParameter resistor_param[4] = {
    WiFiManagerParameter("resistor1", "Resistor", config.probe[0].resistor, 6),
    WiFiManagerParameter("resistor2", "Resistor", config.probe[1].resistor, 6),
    WiFiManagerParameter("resistor3", "Resistor", config.probe[2].resistor, 6),
    WiFiManagerParameter("resistor4", "Resistor", config.probe[3].resistor, 6)};

WiFiManagerParameter idx_param[4] = {
    WiFiManagerParameter("idx1", "IDx", config.probe[0].idx, 6),
    WiFiManagerParameter("idx2", "IDx", config.probe[1].idx, 6),
    WiFiManagerParameter("idx3", "IDx", config.probe[2].idx, 6),
    WiFiManagerParameter("idx4", "IDx", config.probe[3].idx, 6)};

WiFiManagerParameter number_of_Samples_param[4] = {
    WiFiManagerParameter("number_of_Samples1", "Number of samples", config.probe[0].number_of_Samples, 6),
    WiFiManagerParameter("number_of_Samples2", "Number of samples", config.probe[1].number_of_Samples, 6),
    WiFiManagerParameter("number_of_Samples3", "Number of samples", config.probe[2].number_of_Samples, 6),
    WiFiManagerParameter("number_of_Samples4", "Number of samples", config.probe[3].number_of_Samples, 6)};

WiFiManagerParameter voltage_param[4] = {
    WiFiManagerParameter("voltage1", "Voltage", config.probe[0].voltage, 6),
    WiFiManagerParameter("voltage2", "Voltage", config.probe[1].voltage, 6),
    WiFiManagerParameter("voltage3", "Voltage", config.probe[2].voltage, 6),
    WiFiManagerParameter("voltage4", "Voltage", config.probe[3].voltage, 6)};

WiFiManagerParameter title_text[4] = {
    WiFiManagerParameter("<h1>Sonde 1:</h1>"),
    WiFiManagerParameter("<h1>Sonde 2:</h1>"),
    WiFiManagerParameter("<h1>Sonde 3:</h1>"),
    WiFiManagerParameter("<h1>Sonde 4:</h1>")};

WiFiManagerParameter on_change_param[4] = {
    WiFiManagerParameter(config.probe[0].send_on_change_input),
    WiFiManagerParameter(config.probe[1].send_on_change_input),
    WiFiManagerParameter(config.probe[2].send_on_change_input),
    WiFiManagerParameter(config.probe[3].send_on_change_input)};

WiFiManagerParameter measure_text[4] = {
    WiFiManagerParameter(config.probe[0].txt_measure),
    WiFiManagerParameter(config.probe[1].txt_measure),
    WiFiManagerParameter(config.probe[2].txt_measure),
    WiFiManagerParameter(config.probe[3].txt_measure)};

WiFiManagerParameter treshold_param[4] = {
    WiFiManagerParameter("treshold1", "Treshold", config.probe[0].treshold, 6),
    WiFiManagerParameter("treshold2", "Treshold", config.probe[1].treshold, 6),
    WiFiManagerParameter("treshold3", "Treshold", config.probe[2].treshold, 6),
    WiFiManagerParameter("treshold4", "Treshold", config.probe[3].treshold, 6)};

WiFiManagerParameter dc_offset = WiFiManagerParameter(offset_txt);
WiFiManagerParameter wm_time = WiFiManagerParameter(time_txt);


