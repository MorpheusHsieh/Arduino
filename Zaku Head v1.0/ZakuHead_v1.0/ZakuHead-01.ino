#include <Servo.h>

Servo myservo; // 建立Servo物件，控制伺服馬達

char buffer[81];

const byte pinBtn = A0;
const byte pinRes = A1;
const byte eyeLED = 6;
const byte LED1 = 7;
const byte LED2 = 8;
const byte pinMot = 9;

const byte LED3 = 3;

const int minDegree = 45;
const int maxDegree = 140;
const int rorateDelay = 20;

const int minBrighht = 30;
const int maxBrighht = 250;
int brightness = minBrighht;
int fadeAmount = 5;
const int fadeDelay = 30; 

int Button[3][3] = {
  {1, 507, 514},  // button 1
  {2, 675, 680},  // button 2
  {3, 763, 767},  // button 1+2
};

void setup() 
{
  Serial.begin(9600); 
  
  pinMode(pinBtn, INPUT); 
  pinMode(pinRes, INPUT); 
  
  pinMode(eyeLED, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);

  myservo.attach(pinMot, 500, 2400); // 修正脈衝寬度範圍
  myservo.write(90);                 // 一開始先置中90度
  delay(1500);

  analogWrite(eyeLED, 50);
  for (int i=90; i>=minDegree; i--) { myservo.write(i); delay(rorateDelay); }
  delay(500);
  for (int i=minDegree; i<=maxDegree; i++) { myservo.write(i);  delay(rorateDelay); }
  delay(500);
  for (int i=maxDegree; i>=90; i--) { myservo.write(i);  delay(rorateDelay); }
  myservo.write(90);
  
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
}

void loop()
{
  //checkBtnValue(pinBtn);
  fadeMode();

  //Serial.println("Press button ...");
  int btn_num = ButtonCheck();
  if (btn_num == 0) return;

  switch(btn_num)
  {
    case 1:
      digitalWrite(LED1, HIGH);
      digitalWrite(LED2, LOW);
      autoRotate();
      break;
    case 2:
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, HIGH);
      manualRotate();
      break;
    default:
      digitalWrite(LED1, LOW);
      digitalWrite(LED2, LOW);
  }
  delay(100);
} 

int ButtonCheck()
{
  int pinVal = analogRead(pinBtn);
  int idx = 0;
  for (int i=0; i<3; i++)
  {
    if(pinVal >= Button[i][1] && pinVal <= Button[i][2]) {
      idx = Button[i][0];
      break;
    }
  }
  return idx;
}

void autoRotate()
{
  while (true)
  {
    for (int i=90; i<=maxDegree; i++)
    {
      myservo.write(i); // 使用write，傳入角度，從0度轉到180度
      delay(rorateDelay);
      
      if (ButtonCheck() == 2 || ButtonCheck() == 3) return;
    }
    delay(750);
    for (int i=maxDegree; i>=minDegree; i--)
    {
      myservo.write(i); // 使用write，傳入角度，從0度轉到180度
      delay(rorateDelay);
      if (ButtonCheck() == 2 || ButtonCheck() == 3) return;
    }

    delay(750);
    for (int i=minDegree; i<=90; i++)
    {
      myservo.write(i);
      delay(rorateDelay);
      if (ButtonCheck() == 2 || ButtonCheck() == 3) return;
    }
  }
}

void manualRotate()
{
  while (true)
  {
    int val = analogRead(pinRes);
    int degree = map(val, 0, 1023, 0, 180);
    if (degree < minDegree) degree = minDegree;
    if (degree > maxDegree) degree = maxDegree;
    myservo.write(degree);
    delay(rorateDelay);

    if (ButtonCheck() == 1 || ButtonCheck() == 3) return;
  }
}

void fadeMode()
{
  analogWrite(eyeLED, brightness);
  brightness = brightness + fadeAmount;
  if (brightness <= minBrighht || brightness >= maxBrighht) {
    fadeAmount = -fadeAmount;
  }
  delay(fadeDelay);
}

void checkBtnValue(int pinNum)
{
  Serial.println("Press button ...");

  if (analogRead(pinNum) > 0)
  {
    delay(20);
    int min = 1024, max = 0, avg = 0;
    while (analogRead(pinNum))
    {
      int val = analogRead(pinNum);
      avg = 0.5 * avg + 0.5 * val;
      if (val < min) min = val;
      if (max < val) max = val;

    Serial.print("min: "); Serial.print(min);
    Serial.print(", max: "); Serial.print(max);
    Serial.print(", avg: "); Serial.print(avg);
    }
  }
  delay(100);
}


