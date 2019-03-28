// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _ADXL345_H_
#define _ADXL345_H_

#include "Arduino.h"

#define Module_ADDR (0x53) // Device Address

enum REG
{
    DEVID          = 0x00  // Device ID
  , THRESH_TAP     = 0x1D  // Tap threshold
  , OFSX           = 0x1E  // X-axis offset
  , OFSY           = 0x1F  // Y-axis offset
  , OFSZ           = 0x20  // Z-axis offset
  , DUR            = 0x21  // Tap duration
  , Latent         = 0x22  // Tap latency
  , Window         = 0x23  // Tap window
  , THRESH_ACT     = 0x24  // Activity threshold
  , THRESH_INACT   = 0x25  // Inactivity threshold
  , TIME_INACT     = 0x26  // Inactivity time
  , ACT_INACT_CTL  = 0x27  // Axis enable control for activity and inactivity detection
  , THRESH_FF      = 0x28  // Free-fall threshold
  , TIME_FF        = 0x29  // Free-fall time
  , TAP_AXES       = 0x2A  // Axis control for single/double tap
  , ACT_TAP_STATUS = 0x2B  // Source for single/double tap
  , BW_RATE        = 0x2C  // Data rate and power mode control
  , POWER_CTL      = 0x2D  // Power-saving features control
  , INT_ENABLE     = 0x2E  // Interrupt enable control
  , INT_MAP        = 0x2F  // Interrupt mapping control
  , INT_SOURCE     = 0x30  // Source of interrupts
  , DATA_FORMAT    = 0x31  // Data format control
  , DATAX0         = 0x32  // X-axis data 0
  , DATAX1         = 0x33  // X-axis data 1
  , DATAY0         = 0x34  // Y-axis data 0
  , DATAY1         = 0x35  // Y-axis data 1
  , DATAZ0         = 0x36  // Z-axis data 0
  , DATAZ1         = 0x37  // Z-axis data 1
  , FIFO_CTL       = 0x38  // FIFO control
  , FIFO_STATUS    = 0x39  // FIFO status
};

enum BwRateData // 0x2C
{
  // Normal power mode
    Rate_3200_Hz = 0x0F
  , Rate_1600_Hz = 0x0E
  , Rate_800_Hz  = 0x0D
  , Rate_400_Hz  = 0x0C
  , Rate_200_Hz  = 0x0B
  , Rate_100_Hz  = 0x0A // default value
  , Rate_50_Hz   = 0x09
  , Rate_25_Hz   = 0x08
  , Rate_12_5_Hz = 0x07
  , Rate_6_25_Hz = 0x06
};

enum PowerCtlData  // 0x2D
{
    Reset     = 0x00
  , Measure   = 0x08 // default value
  , AutoSleep = 0x10
};

enum IntEnableData  // 0x2E
{
  DATA_READY = 0x80
};

enum DataFormatData  // 0x31
{
  SelfTest_FullRes_16G = 0x8B
, FullRes_2G           = 0x08 // default value
, FullRes_4G           = 0x09
, FullRes_8G           = 0x0A
, FullRes_16G          = 0x0B
};

// ----------------------------------------------------------------------------
class ADXL345
{
  private:
    double _GravityPerLsb = 0.00390625;
    double _Alpha = 0.5;
    double __fXg = 0, _fYg = 0, _fZg = 0;

    void writeToReg(REG reg_addr, byte value);
    int readFromReg(REG reg_addr, int nbytes, byte*buffer);

  public:
    ADXL345();
    virtual ~ADXL345();

    void begin();
    void clearAllOffset();

    // If ID of connected device is 0xE5, then it is ADXL345
    bool testConnection();
    byte getDeviceID(); // Get the device ID

    void readRawDatas(int *x, int *y, int *z);
    void readGravitys(double *xg, double *yg, double *zg);

    // Setting and getting Data Format data
    void setDataFormat(byte val);
    byte getDataFormat();

    // Setting and getting Power Control data
    void setPowerCtl(byte val);
    byte getPowerCtl();

    // Setting and getting bandwidth rate data
    void setBwRate(byte val);
    byte getBwRate();

    // Setting Interrupt Mode
    void setIntEnable();

    // Setting Offset
    void setOffsetX(byte val);
    void setOffsetY(byte val);
    void setOffsetZ(byte val);
    void clearAllOffsets();
};
#endif /* _ADXL345_H_ */
