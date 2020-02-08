//--------------------------------------------------------------------------------------
//  I2C
//--------------------------------------------------------------------------------------
#include "I2C.h"
#include "Wire.h"
#include "Global.h"

void setupI2C()
{
  Wire.setClock(400000);
  Wire.begin(4, 2);//sda,scl
}

void scanI2C()
{
  byte error, address;
  int nDevices;
  nDevices = 0;
  for (address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0)
    {
      Log.print(LogObject::DebugLevels::Normal,F("I2C device found at address 0x"));
      if (address < 16)
        Log.print(LogObject::DebugLevels::Normal,"0");
      Log.println(LogObject::DebugLevels::Normal,address, HEX);
      
      nDevices++;
    }
    else if (error == 4)
    {
      Log.print(LogObject::DebugLevels::ErrorOnly,F("Unknown error at address 0x"));
      if (address < 16)
        Log.print(LogObject::DebugLevels::ErrorOnly,"0");
      Log.println(LogObject::DebugLevels::ErrorOnly,address, HEX);
    }
  }
  if (nDevices == 0)
    Log.println(LogObject::DebugLevels::ErrorOnly,F("No I2C devices found\n"));
  else
    Log.println(LogObject::DebugLevels::Verbose,F("done\n"));
}
