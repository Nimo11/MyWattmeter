//--------------------------------------------------------------------------------------
//NTP
//--------------------------------------------------------------------------------------
#ifndef ntp_h
#define ntp_h

#include <Arduino.h>
#include <WiFiUdp.h>

static const char ntpServerName[] = "pool.ntp.org";

static const int timeZone = 1; // Central European Time

static const unsigned int localPort=8888; // local port to listen for UDP packets

const int NTP_PACKET_SIZE = 48;     // NTP time is in the first 48 bytes of message
static byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets


extern void setNTP();

extern time_t getNtpTime();

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address);

#endif