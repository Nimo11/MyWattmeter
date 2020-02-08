#ifndef ADS1015_h
#define ADS1015_h

#include <Arduino.h>


#define ADS1015_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)
#define ADS1015_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
#define ADS1015_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)    
#define ADS1015_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
#define ADS1015_REG_CONFIG_DR_860SPS   (0x00C0)  // 860 samples per second
#define ADS1015_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode     
#define ADS1015_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
#define ADS1015_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
#define ADS1015_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
#define ADS1015_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
#define ADS1015_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3
#define ADS1015_REG_POINTER_CONFIG      (0x01)
#define ADS1015_REG_POINTER_CONVERT     (0x00) 
#define ADS1015_ADDRESS                 (0x48)    // 1001 000 (ADDR = GND)

extern void setADSConfig();

extern uint16_t readADC_Continuous(uint8_t channel);

extern int16_t getLastConversionResults();

static uint16_t readRegister(uint8_t i2cAddress, uint8_t reg);

static void writeRegister(uint8_t i2cAddress, uint8_t reg, uint16_t value);

#endif