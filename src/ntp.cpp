//--------------------------------------------------------------------------------------
//NTP
//--------------------------------------------------------------------------------------
#include "ntp.h"
#include <Time.h>
#include "Global.h"

void setNTP()
{
  ntpState = NtpStates::waiting;
  _Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(_Udp.localPort());
  Serial.println("waiting for sync");

  setSyncProvider(getNtpTime);
  setSyncInterval(3600);
}

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (_Udp.parsePacket() > 0); // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  int err = WiFi.hostByName(ntpServerName, ntpServerIP);
  if (err == 1)
  {
    Serial.print(ntpServerName);
    Serial.print(": ");
    Serial.println(ntpServerIP);
    sendNTPpacket(ntpServerIP);
    uint32_t beginWait = millis();
    while (millis() - beginWait < 1500)
    {
      int size = _Udp.parsePacket();
      if (size >= NTP_PACKET_SIZE)
      {
        Serial.println("Receive NTP Response");
        
        _Udp.read(packetBuffer, NTP_PACKET_SIZE); // read packet into the buffer
        unsigned long secsSince1900;
        // convert four bytes starting at location 40 to a long integer
        secsSince1900 = (unsigned long)packetBuffer[40] << 24;
        secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
        secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
        secsSince1900 |= (unsigned long)packetBuffer[43];

        ntpState=NtpStates::Ok;
        return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
      }
    }
    Serial.println("No NTP Response :-(");
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(err);
  }
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  _Udp.beginPacket(address, 123); //NTP requests are to port 123
  _Udp.write(packetBuffer, NTP_PACKET_SIZE);
  _Udp.endPacket();
}
