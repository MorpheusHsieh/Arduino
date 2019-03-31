// Do not remove the include below
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_SR.h>
#include "ADXL345.h"

bool isDebug = false;

const byte pinSDA = 4;       // I2C Data for ADXL345
const byte pinSCL = 5;       // I2C Clock for ADXL345
const byte pinLCD_CLK = 6;
const byte pinLCD_Data = 7;
const byte pinRedLED = 12;
const byte pinGreenLED = 13;
const byte pinSW = 14;

enum Events { Gravity, MinG, MaxG, Angles, MinPR, MaxPR, Calibration };
Events _Event = Gravity;

static const char *_EventStr[] =  
{ 
    "Gravity", "MinG", "MaxG"
  , "Angles", "MinPR", "MaxPR"
  , "Calibration" 
};

enum SwitchState { NoSwOn, SW1, SW2, TwoSWs };
SwitchState _SwState;
static const char *_SwStateStr[] =  { "NoSwOn", "SW1", "SW2", "TwoSws" };

struct GravityData { double val = 0.0; double minVal = 0.0; double maxVal = 0.0; };
GravityData _fXg, _fYg, _fZg;

struct AngleData { double val = 0.0; double minVal = 0.0; double maxVal = 0.0; };
AngleData _Roll, _Pitch;

struct CalibData { int x = 0.0; int y = 0.0; int z = 0.0; };
CalibData _Calib;

char _StrBuf[8];
String _OutStr;
const double _Alpha = 0.5;

ADXL345 _Accel;
LiquidCrystal_SR _LCD(pinLCD_Data, pinLCD_CLK, TWO_WIRE);


// Methods ---------------------------------------------------------------------
void showDeviceID();
//bool getSwitchState(byte pinNum);

// Main Methods ----------------------------------------------------------------
//The setup function is called once at startup of the sketch
void setup()
{
  Serial.begin(9600);
  
  pinMode(pinSW, INPUT);
  pinMode(pinGreenLED, OUTPUT);
  pinMode(pinRedLED, OUTPUT);

  digitalWrite(pinGreenLED, HIGH);  // Program Start

  _Accel.begin();    
  _Accel.clearAllOffsets();
  
  _LCD.begin(16, 2); // initialize the lcd 
  showDeviceID();

  digitalWrite(pinGreenLED, HIGH);
}

void loop()
{
  _SwState = NoSwOn;
  getSwitchState(pinSW);
  
  switch(_SwState)
  {
    case SW1:
           if ((_Event == Gravity) || (_Event == MinG) || (_Event == MaxG))  _Event = Angles;
      else if ((_Event == Angles) || (_Event == MinPR) || (_Event == MaxPR)) _Event = Gravity;
      break;
    case SW2:
      if (_Event == Gravity)     _Event = MinG;
      else if (_Event == MinG)   _Event = MaxG;
      else if (_Event == MaxG)   _Event = Gravity;
      else if (_Event == Angles) _Event = MinPR;
      else if (_Event == MinPR)  _Event = MaxPR;
      else if (_Event == MaxPR)  _Event = Angles;
      break;
    case TwoSWs: 
      // 當事件為 MinG, MaxG, MinPR, MaxPR 時，清除記錄的 min 或 max 值
      if (_Event == MinG) {
        _fXg.minVal = 0.0; _fYg.minVal = 0.0; _fZg.minVal = 0.0;
      }
     else if (_Event == MaxG) {
        _fXg.maxVal = 0.0; _fYg.maxVal = 0.0; _fZg.maxVal = 0.0;
     }
     else if (_Event == MinPR) {
       _Roll.minVal = 0.0; _Pitch.minVal = 0.0;
     }
     else if (_Event == MaxPR) {
       _Roll.maxVal = 0.0; _Pitch.maxVal = 0.0;
     }
     else // 剩餘事件則進入校正模式
      _Event = Calibration;
      break;
    case NoSwOn:
    default:
      break;
  }

  if (isDebug)
  {
    Serial.print("Sw state: ");
    Serial.print(_SwStateStr[_SwState]);
    Serial.print(", Event: ");
    Serial.println(_EventStr[_Event]);
  }

  switch(_Event)
  {
    case Gravity: 
      GravityEvent();         
      break;
    case MinG: 
      miniGravityEvent();
      break;
    case MaxG: 
      maxiGravityEvent();
      break;
    case Angles: 
      AnglesEvent();     
      break;
    case MinPR:
      miniAnglesEvent();
      break;
    case MaxPR:
      maxiAnglesEvent();
      break;
    case Calibration: 
      calibrationEvent(); 
      break;
    default: break;
  }

  delay(250);
}  

// -----------------------------------------------------------------------------
void showDeviceID()
{
  _LCD.clear();
  _LCD.home();
  _LCD.print("Starting ADXL345");

  _OutStr = "Device ID: 0x" + String(_Accel.getDeviceID(), HEX);
  _LCD.setCursor(0, 1);
  _LCD.print(_OutStr);
}

// -----------------------------------------------------------------------------
void getSwitchState(byte pinSW)
{
  int val = analogRead(pinSW);

  if (val > 0)
  {
    digitalWrite(pinRedLED, HIGH);

    delay(20);
    val = analogRead(pinSW);

    if (val == 0) _SwState = NoSwOn;
    else if ((val >= 500) && (val < 520)) _SwState = SW1;
    else if ((val >= 600) && (val < 620)) _SwState = SW2;
    else if ((val >= 750) && (val < 770)) _SwState = TwoSWs;

    digitalWrite(pinRedLED, LOW);
  }
}

// -----------------------------------------------------------------------------
void readFilterGravityDatas()
{
  double xg, yg, zg;
  _Accel.readGravitys(&xg, &yg, &zg);

  _fXg.val = xg * _Alpha + (_fXg.val * (1 - _Alpha));
  if (xg > _fXg.maxVal) _fXg.maxVal = xg;
  if (xg < _fXg.minVal) _fXg.minVal = xg;

  _fYg.val = yg * _Alpha + (_fYg.val * (1 - _Alpha));
  if (yg > _fYg.maxVal) _fYg.maxVal = yg;
  if (yg < _fYg.minVal) _fYg.minVal = yg;

  _fZg.val = zg * _Alpha + (_fZg.val * (1 - _Alpha));
  if (zg > _fZg.maxVal) _fZg.maxVal = yg;
  if (zg < _fZg.minVal) _fZg.minVal = yg;

  _Roll.val = ((atan2(-_fYg.val, _fZg.val) * 180.0) / M_PI);
  if (_Roll.val > _Roll.maxVal) _Roll.maxVal = _Roll.val;
  if (_Roll.val < _Roll.minVal) _Roll.minVal = _Roll.val;

  double fyg2 = _fYg.val * _fYg.val;
  double fzg2 = _fZg.val * _fZg.val;
  _Pitch.val = ((atan2(_fXg.val, sqrt(fyg2 + fzg2)) * 180.0) / M_PI);
  if (_Pitch.val > _Pitch.maxVal) _Pitch.maxVal = _Pitch.val;
  if (_Pitch.val < _Pitch.minVal) _Pitch.minVal = _Pitch.val;
}

// -----------------------------------------------------------------------------
void GravityEvent()
{
  readFilterGravityDatas();
  showGravityEvent("Gravity", _fXg.val, _fYg.val, _fZg.val);
}

void showGravityEvent(String title, double val_x, double val_y, double val_z)
{
  _LCD.clear();
  _LCD.home();
  _LCD.print(title);

  dtostrf(val_x, 7, 1, _StrBuf);
  _OutStr = String(_StrBuf) + "g";
  _LCD.setCursor(8, 0);
  _LCD.print(_OutStr);

  dtostrf(val_y, 7, 1, _StrBuf);
  _OutStr = String(_StrBuf) + "g";
  _LCD.setCursor(0, 1);
  _LCD.print(_OutStr);
  
  dtostrf(val_z, 7, 1, _StrBuf);
  _OutStr = String(_StrBuf) + "g";
  _LCD.setCursor(8, 1);
  _LCD.print(_OutStr);
}

// -----------------------------------------------------------------------------
void AnglesEvent()
{
  readFilterGravityDatas();
  showAnglesEvent("", _Roll.val, _Pitch.val);
}

void showAnglesEvent(String title, double roll, double pitch)
{
  _LCD.clear();
  _LCD.home();

  dtostrf(roll, 5, 1, _StrBuf);
  _OutStr = title + "Roll:  " + String(_StrBuf) + char(223);
  _LCD.print(_OutStr);

  dtostrf(pitch, 5, 1, _StrBuf);
  _OutStr = title + "Pitch: " + String(_StrBuf) + char(223);
  _LCD.setCursor(0, 1);
  _LCD.print(_OutStr);
}

// -----------------------------------------------------------------------------
void miniGravityEvent()
{
  showGravityEvent("MinG", _fXg.minVal, _fYg.minVal, _fZg.minVal);
}

// -----------------------------------------------------------------------------
void maxiGravityEvent()
{
  showGravityEvent("MaxG", _fXg.maxVal, _fYg.maxVal, _fZg.maxVal);
}

// -----------------------------------------------------------------------------
void miniAnglesEvent()
{
  showAnglesEvent("Min", _Roll.minVal, _Pitch.minVal);
}

void maxiAnglesEvent()
{
  showAnglesEvent("Max", _Roll.maxVal, _Pitch.maxVal);
}

// -----------------------------------------------------------------------------
void calibrationEvent()
{
  int x, y, z;
  int num_samples = 100;
  int flicker_time = 50;
  int tmpVal;
  
  digitalWrite(pinRedLED, HIGH);
  _LCD.clear();
  _LCD.home();
  _LCD.print("Place device at");
  _LCD.setCursor(0, 1);
  _LCD.print("0,0,1g (1=Y/2=N)");
  delay(500);

  _SwState = NoSwOn;
  while (_SwState == NoSwOn) { getSwitchState(pinSW); }

  if (_SwState == SW2) 
  {
    _Event = Gravity;
    return;
  }  
  
  _LCD.clear();
  _LCD.home();
  _LCD.print("Calculate OFSs..");

  // 再取樣 100份資料
  _Calib.x = 0; _Calib.y = 0; _Calib.z = 0;
  for (int i=0; i<num_samples; i++)
  {
    _Accel.readRawDatas(&x, &y, &z);

    _Calib.x += x;
    _Calib.y += y;
    _Calib.z += z;

    digitalWrite(pinRedLED, HIGH);
    delay(flicker_time);
    digitalWrite(pinRedLED, LOW);
    delay(flicker_time);
  }

  // 由於在 FULL_RES 模式下資料格式為 3.9 mg/LSB，
  // 所以讀取的數值必須乘以 4 才是真正的 mg
  _Calib.x = _Calib.x / num_samples;         // Z軸在 0g 值為 0
  _Calib.y = _Calib.y / num_samples;         // Y軸在 0g 值為 0
  _Calib.z = (_Calib.z / num_samples) - 256; // Z軸在 1g 值為 256

  _LCD.clear();
  _LCD.home();
  _LCD.print("OFSs:");

  _OutStr = String(_Calib.x * 4) + "mg";
  while (_OutStr.length() < 8) _OutStr = " " + _OutStr;
  _LCD.setCursor(8, 0);
  _LCD.print(_OutStr);

  _OutStr = String(_Calib.y * 4) + "mg";
  while (_OutStr.length() < 8) _OutStr = " " + _OutStr;
  _LCD.setCursor(0, 1);
  _LCD.print(_OutStr);

  _OutStr = String(_Calib.z * 4) + "mg";
  while (_OutStr.length() < 8) _OutStr = " " + _OutStr;
  _LCD.setCursor(8, 1);
  _LCD.print(_OutStr);
  delay(500);

  _SwState = NoSwOn;
  while (_SwState == NoSwOn) { getSwitchState(pinSW); }

  _LCD.clear();
  _LCD.home();
  _LCD.print("Write offsets ?");
  _LCD.setCursor(0, 1);
  _LCD.print("SW1=Y, SW2=N:");
  delay(500);
    
  _SwState = NoSwOn;
  while (_SwState == NoSwOn) { getSwitchState(pinSW); }

  if (_SwState == SW1)
  {
    // 由於 OFXS, OFSY 和 OFSZ 暫存器是以八位元二進為補數的方式表示
    // 且每一個單位表示 15.6 mg/LSB.
    _Accel.setOffsetX(_Calib.x / 4.0);
    _Accel.setOffsetY(_Calib.y / 4.0);
    _Accel.setOffsetZ(_Calib.z / 4.0);
    _Accel.begin();
  }

  _Event = Gravity;
}
