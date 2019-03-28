// Do not remove the include below
#include "ADXL345.h"
#include <Wire.h>    // I2C library, gyroscope

/*
 * Construct Method ------------------------------------------------------------
 */
ADXL345::ADXL345() {}

ADXL345::~ADXL345() {}

/*
 * Private Method --------------------------------------------------------------
 */
void ADXL345::writeToReg(REG reg_addr, byte value)
{
    Wire.beginTransmission(Module_ADDR);   // start transmission to device
    Wire.write(reg_addr);                        // send register address
    Wire.write(value);          // send value to write
    Wire.endTransmission();     // end transmission
}

int ADXL345::readFromReg(REG reg_addr, int nbytes, byte *buffer)
{
    int idx = 0;

    Wire.beginTransmission(Module_ADDR);
    Wire.write(reg_addr);
    Wire.endTransmission();

    // read value from address
    Wire.requestFrom(Module_ADDR, nbytes);
    while(Wire.available() && idx < nbytes) {
      buffer[idx++] = Wire.read();
    }

    return idx;
}

/*
 * Public Method ---------------------------------------------------------------
 */
void ADXL345::begin()
{
  Wire.begin();

  setDataFormat(FullRes_16G);

  setPowerCtl(Reset);
  setPowerCtl(AutoSleep);
  setPowerCtl(Measure);

  setBwRate(Rate_100_Hz);
}

// -----------------------------------------------------------------------------
bool ADXL345::testConnection()
{
  return (getDeviceID() == 0xE5);
}

byte ADXL345::getDeviceID()
{
  byte devid = 0;
  readFromReg(DEVID, 1 , &devid);
  return devid;
}

// -----------------------------------------------------------------------------
void ADXL345::readRawDatas(int *x, int *y, int *z)
{
  byte size = 6;
  byte buff[6];

  readFromReg(DATAX0, size, buff);

  *x = (((int)buff[1]) << 8) | buff[0];
  *y = (((int)buff[3]) << 8) | buff[2];
  *z = (((int)buff[5]) << 8) | buff[4];
}

void ADXL345::readGravitys(double *xg, double *yg, double *zg)
{
  int x, y, z;
  readRawDatas(&x, &y, &z);

  *xg = x * _GravityPerLsb;
  *yg = y * _GravityPerLsb;
  *zg = z * _GravityPerLsb;
}

// -----------------------------------------------------------------------------
void ADXL345::setPowerCtl(byte val)
{
  writeToReg(POWER_CTL, val);
}

byte ADXL345::getPowerCtl()
{
  byte buf[1] = { 0 };
  readFromReg(POWER_CTL, 1, buf);
  return buf[0];
}

// -----------------------------------------------------------------------------
void ADXL345::setDataFormat(byte val)
{
  writeToReg(DATA_FORMAT, val);
}

byte ADXL345::getDataFormat()
{
  byte data[1] = { 0 };
  readFromReg(DATA_FORMAT, 1, data);
  return data[0];
}

// -----------------------------------------------------------------------------
void ADXL345::setBwRate(byte val)
{
  writeToReg(BW_RATE, val);
}

byte ADXL345::getBwRate()
{
  byte data[1] = { 0 };
  readFromReg(BW_RATE, 1, data);
  return data[0];
}

// -----------------------------------------------------------------------------
void ADXL345::setOffsetX(byte val) { writeToReg(OFSX, val); }

void ADXL345::setOffsetY(byte val) { writeToReg(OFSY, val); }

void ADXL345::setOffsetZ(byte val) { writeToReg(OFSZ, val); }

void ADXL345::clearAllOffsets()
{
  setOffsetX(0x00); setOffsetY(0x00); setOffsetZ(0x00);
}

// -----------------------------------------------------------------------------
void ADXL345::setIntEnable()
{
  writeToReg(INT_ENABLE, DATA_READY);
}
