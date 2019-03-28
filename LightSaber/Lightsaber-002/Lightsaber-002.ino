boolean isDebug = false;

const int LED_Segs[6] = { 11, 10, 9, 6, 5, 3 };
const int pinBtn = 14;  // A0

const int LoopDelay = 0;
const int OpenDelay = 100;
const int CloseDelay = 100;
const int BrightDelay = 200;

const int OpenDuration = 500;
const int CloseDuration = 50;

enum SystemEvent { Open, Close, Brighting };
SystemEvent SysState = Close;

const int max_brightness = 220;
const int min_brightness = 120;

int fadeAmount = 5;
int brightness = max_brightness;

SystemEvent readButton();
void LightOn();
void LightOut();
void LightBrighting();

void setup() 
{
  Serial.begin(9600);
  pinMode(pinBtn, INPUT);

  for (int i=0; i<6; i++) {
    pinMode(LED_Segs[i], OUTPUT);
  }
}

void loop() 
{
  int press_time = readButton();

  switch (SysState)
  {
    case Open:
      LightOn();
      break;
    case Brighting:
      LightBrighting();
      break;
    case Close:
      LightOut();
      break;
  }
  delay(LoopDelay);

  if (isDebug) Serial.println(SysState);
}

SystemEvent readButton()
{
  int press_time = -1;
  if (analogRead(pinBtn) > 0)
  {
    unsigned long start_time = millis();
    delay(20);
    if (analogRead(pinBtn) > 0)
    {
      while (analogRead(pinBtn)) ;
      press_time = millis() - start_time;
      if (isDebug) {
        Serial.print("Press time: ");
        Serial.println(press_time);
      }
    }

    if ((SysState == Close) && (press_time >= OpenDuration)) {
      SysState = Open;
    } else if ((SysState != Close) && (press_time >= CloseDuration) && (press_time < OpenDuration))
      SysState = Close;
  }
  
  return SysState;
}

void LightOn()
{
  for (int i=0; i<6; i++)
  {
    analogWrite(LED_Segs[i], brightness);
    delay(OpenDelay);
  }

  SysState = Brighting;
}

void LightOut()
{
  for (int i=5; i>=0; i--)
  {
    analogWrite(LED_Segs[i], LOW);
    delay(CloseDelay);
  }
}

void LightBrighting()
{
  if (brightness <= min_brightness || brightness >= max_brightness) {
    fadeAmount = -fadeAmount ;
  }   
  brightness += fadeAmount;
  if (isDebug) Serial.println(brightness);

  for (int i=0; i<6; i++) {
    analogWrite(LED_Segs[i], brightness);
  }
  delay(BrightDelay);
}



