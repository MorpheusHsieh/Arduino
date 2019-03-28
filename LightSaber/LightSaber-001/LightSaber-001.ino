
const int LED_Segs[6] = { 3, 5, 6, 9, 10, 11 };
const int pinBtn = 14;  // A0
const int pinAudio = 4;

const int LoopDelay = 50;
const int LedShowsDelay = 100;
const int OpenDuration = 180;
const int CloseDuration = 30;

enum SystemEvent { Open, Close, Brighting };
String SysStateStr[3] = { "Open", "Close", "Brightness" };
SystemEvent SysState = Close;

const int max_brightness = 200;
const int min_brightness = 80
;
int brightness = 180;
int fadeAmount = 5;
int BrightingDelay = 10;

void setup() 
{
  Serial.begin(9600);
  pinMode(pinBtn, INPUT);
  pinMode(pinAudio, OUTPUT);
  
  for (int i=0; i<6; i++) {
    pinMode(LED_Segs[i], OUTPUT);
  }
}

void loop() 
{
  int press_time = readButton();

  if (press_time > 0)
  {
    if ((SysState == Close) && (press_time > OpenDuration)) 
    {
      LightOn();
      SysState = Brighting;
    }
    else if ((SysState != Close) && (press_time > CloseDuration)) 
    {
      LightOut();
    }
  }

  if (SysState == Brighting) LightBrighting();
  
  delay(LoopDelay);
}

int readButton()
{
  int press_time = -1;
  
  if (analogRead(pinBtn) > 0)
  {
    delay(20);
    if (analogRead(pinBtn) > 0)
    {
      unsigned long start_time = millis();
      while (analogRead(pinBtn)) ;
      press_time = millis() - start_time;
    }
  }
  
  return press_time;
}

void LightOn()
{
  SysState = Open;
  digitalWrite(pinAudio, HIGH);
  
  for (int i=0; i<6; i++)
  {
    analogWrite(LED_Segs[i], 127);
    delay(LedShowsDelay);
  }
}

void LightOut()
{
  SysState = Close;
  digitalWrite(pinAudio, LOW);

  for (int i=5; i>=0; i--)
  {
    analogWrite(LED_Segs[i], 0);
    delay(LedShowsDelay);
  }
}

void LightBrighting()
{
  for (int i=0; i<6; i++) {
    analogWrite(LED_Segs[i], brightness);
  }
  
  brightness = brightness + fadeAmount;
  if (brightness <= min_brightness || brightness >= max_brightness) {
    fadeAmount = -fadeAmount ;
  }   
  delay(BrightingDelay);
}


