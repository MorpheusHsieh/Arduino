// Do not remove the include below
#include "ADXL345_Basic.h"
#include "ADXL345.h"

bool isDebug = false;

const byte pinSDA = 4;
const byte pinSCL = 5;
const byte pinSW1 = 8;
const byte pinSW2 = 9;
const byte pinGreen = 10;
const byte pinRed = 11;

enum Events
{
    Normal
  , Calibration
};
Events _Event;

static const char *EventStr[] = { "Normal", "Calibration" };

struct RawData { int min = 0; int max = 0; int avg = 0; int calib = 0; };
RawData _Xout, _Yout, _Zout;

// Global Variable -------------------------------------------------------------
ADXL345 Accel;

const double _GFactor = 0.00390625;
const double _Alpha = 0.5;

char outstr[81];
char format_buf[81];
String format;

double Xg = 0, Yg = 0, Zg = 0;
char Xg_str[10], Yg_str[10], Zg_str[10];

double fXg = 0, fYg = 0, fZg = 0;
char fXg_str[10], fYg_str[10], fZg_str[10];

char roll_str[10], pitch_str[10];

// Methods ---------------------------------------------------------------------
bool getSwitchState(byte pinSW);
void NormalMode();
void CalibrationMode();

void collectData();

// Main Methods ----------------------------------------------------------------
//The setup function is called once at startup of the sketch
void setup()
{
  Serial.begin(9600);

  pinMode(pinSW1, INPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinRed, OUTPUT);

  digitalWrite(pinGreen, HIGH);  // Program Start

  Accel.begin();
  Accel.normalModeInit();

  byte devid = Accel.getDeviceID();

  if (isDebug)
  {

    String format = "\r\nDevice ID: 0x%02X\r\n";
    format.toCharArray(format_buf, 14);
    sprintf(outstr, format_buf, devid);
    Serial.print(outstr);
  }

  delay(500);
}

// The loop function is called in an endless loop
void loop()
{
  bool isPress = getSwitchState(pinSW1);

  Events old_event = _Event;
  if (isPress)
  {
    if (_Event == Normal)
      _Event = Calibration;
    else if (_Event == Calibration)
      _Event = Normal;

    String format = "\r\nisPress: %d, Mode: %s -> %s";
    format.toCharArray(format_buf, 45);
    sprintf(outstr, format_buf, isPress, EventStr[old_event], EventStr[_Event]);
    Serial.print(outstr);
  }

  switch(_Event)
  {
    case Calibration:
      digitalWrite(pinGreen, LOW);
      CalibrationMode();
      digitalWrite(pinGreen, HIGH);
      break;
    case Normal:
    default:
      NormalMode();

  }

  delay(500);
}

// -----------------------------------------------------------------------------
bool getSwitchState(byte pinSW)
{
  bool isPress = digitalRead(pinSW);

  unsigned long diff_time;
  if (isPress)
  {
    delay(20);
    isPress = digitalRead(pinSW);
  }

  return isPress;
}

// -----------------------------------------------------------------------------
void NormalMode()
{
  int x, y, z;
  Accel.readRawDatas(&x, &y, &z);

  format = "\r\nRead: %6d, %6d, %6d";
  format.toCharArray(format_buf, 81);
  sprintf(outstr, format_buf, x, y, z);
  Serial.print(outstr);

  Xg = x * _GFactor;
  dtostrf(Xg, 6, 2, Xg_str);

  Yg = y * _GFactor;
  dtostrf(Yg, 6, 2, Yg_str);

  Zg = z * _GFactor;
  dtostrf(Zg, 6, 2, Zg_str);

  //Low Pass Filter
  fXg = Xg * _Alpha + (fXg * (1.0 - _Alpha));
  dtostrf(fXg, 6, 2, fXg_str);

  fYg = Yg * _Alpha + (fYg * (1.0 - _Alpha));
  dtostrf(fYg, 6, 2, fYg_str);

  fZg = Zg * _Alpha + (fZg * (1.0 - _Alpha));
  dtostrf(fZg, 6, 2, fZg_str);

  format = "\r\nGains: %sg, %sg, %sg,\t\tFilter: %sg, %sg, %sg";
  format.toCharArray(format_buf, 81);
  sprintf(outstr, format_buf, Xg_str, Yg_str, Zg_str, fXg_str, fYg_str, fZg_str);
  Serial.print(outstr);

  // Roll & Pitch angles
  //double roll = (atan2(-y, z) * 180.0) / M_PI;
  double roll = 180.0 * atan(Xg / sqrt(Yg * Yg + Zg * Zg)) / M_PI;
  dtostrf(roll, 4, 2, roll_str);

  //double pitch = (atan2(x, sqrt(y * y + z * z)) * 180.0) / M_PI;
  double pitch = 180.0 * atan(Yg / sqrt(Xg * Xg + Zg * Zg)) / M_PI;
  dtostrf(pitch, 4, 2, pitch_str);

  format = "\r\nPitch: %s,\t\tRoll: %s";
  format.toCharArray(format_buf, 81);
  sprintf(outstr, format_buf, pitch_str, roll_str);
  Serial.println(outstr);


  delay(250);
}

// -----------------------------------------------------------------------------
void CalibrationMode()
{
  Accel.calibrationModeInit();
  Accel.clearAllOffset();

  collectData();

  format = "\r\nMin:%6d, %6d, %6d, \tMax:%6d, %6d, %6d, \tAvg:%6d, %6d, %6d";
  format.toCharArray(format_buf, 81);
  sprintf(outstr, format_buf, _Xout.min, _Yout.min, _Zout.min
                            , _Xout.max, _Yout.max, _Zout.max
                            , _Xout.avg, _Yout.avg, _Zout.avg);
  Serial.print(outstr);

//  double calib_x = 0, calib_y = 0, calib_z = 0;

  // Since OFXS, OFSY, and OFSZ registers are each eight bytes and
  // in two complement format with a scale factor of 15.6 mg/LSB.
  // ���b FULL_RES �Ҧ��U��Ʈ榡�� 3.9mg/LSB�A
  // �ҥH�~�t�ȥ������H 4�~��g�J OFSX, OFXY �M OFSZ �Ȧs��
  // �]��   15.6 / 3.9 = 4.
//  double calib_x = -(stddev[0] / 4);
//  double calib_y = -(stddev[1] / 4);
//  double calib_z = -(stddev[2] / 4);
//

  while (!getSwitchState(pinSW1))
  {
    digitalWrite(pinRed, HIGH);
    delay(500);
    digitalWrite(pinRed, LOW);
    delay(500);
    ;
  }

  Accel.normalModeInit();
//  Accel.setOffsetX(calib_x);
//  Accel.setOffsetY(calib_y);
//  Accel.setOffsetZ(calib_z);
  _Event = Normal;
}

// -----------------------------------------------------------------------------
void collectData()
{
  int x, y, z;
  int num_samples = 100;
  for (int i=0; i<num_samples; i++)
  {
    Accel.readRawDatas(&x, &y, &z);

    if (x < _Xout.min) _Xout.min = x;
    if (y < _Yout.min) _Yout.min = y;
    if (z < _Zout.min) _Zout.min = z;

    if (x > _Xout.max) _Xout.max = x;
    if (y > _Yout.max) _Yout.max = y;
    if (z > _Zout.max) _Zout.max = z;

    _Xout.avg += x; _Yout.avg += y; _Zout.avg += z;

    digitalWrite(pinRed, HIGH);
    delay(20);
    digitalWrite(pinRed, LOW);
    delay(20);
  }

  _Xout.avg = _Xout.avg / num_samples;
  _Yout.avg = _Yout.avg / num_samples;
  _Zout.avg = _Zout.avg / num_samples;
}
