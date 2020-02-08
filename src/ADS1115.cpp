#include "ADS1115.h"
#include "Wire.h"

void setADSConfig()
{
  
}

uint16_t readADC_Continuous(uint8_t channel) {
  if (channel > 3)
  {
    return 0;
  }
  
  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_860SPS    | // 8600 samples per second (default)
                    ADS1015_REG_CONFIG_PGA_4_096V   | //Gain one
                    ADS1015_REG_CONFIG_MODE_CONTIN;  // Single-shot mode (default) JD Single

  // Set single-ended input channel
  switch (channel)
  {
    case (0):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
      break;
    case (1):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
      break;
    case (2):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
      break;
    case (3):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
      break;
  }

  // Write config register to the ADC
  writeRegister(ADS1015_ADDRESS, ADS1015_REG_POINTER_CONFIG, config);

  // Wait for the conversion to complete
  delay(1);

  // Read the conversion results
  // Shift 12-bit results right 4 bits for the ADS1015
  return readRegister(ADS1015_ADDRESS, ADS1015_REG_POINTER_CONVERT);  
}

int16_t getLastConversionResults()
{
  // Wait for the conversion to complete
  delay(1);

  // Read the conversion results
  uint16_t res = readRegister(ADS1015_ADDRESS, ADS1015_REG_POINTER_CONVERT);
  return res;

}

static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg) {
  Wire.beginTransmission(i2cAddress);
  Wire.write(ADS1015_REG_POINTER_CONVERT);
  Wire.endTransmission();
  Wire.requestFrom(i2cAddress, (uint8_t)2);
  return ((Wire.read() << 8) | Wire.read());  
}

static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value) {
  Wire.beginTransmission(i2cAddress);
  Wire.write((uint8_t)reg);
  Wire.write((uint8_t)(value>>8));
  Wire.write((uint8_t)(value & 0xFF));
  Wire.endTransmission();
}
