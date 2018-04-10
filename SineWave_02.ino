int redPin = 9; 
int grnPin = 10; 
int bluPin = 11; 


void setup() 
{
  Serial.begin(9600);
  pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);
  pinMode(bluPin, OUTPUT);
}

void loop() 
{
  float x = millis() / 100.0;
  float y = sin(x * 2.0 * PI);
  Serial.print(x); Serial.print(", ");  Serial.println(y);
  int value = 128 + 128 * y;
  
  //write value to pin
  analogWrite(redPin, value);
  analogWrite(grnPin, 0);
  analogWrite(bluPin, 0);
  delay(10);
}




void sin_pwm(const int pin_number, const unsigned long timer, const unsigned int len)
{
  //PWM modulation
  unsigned long mod = timer % len;
  //0.5 * (1 + sin(2*pi*x - pi/2))
  float f = 0.5f * (1.0f + sin(   2.0f * PI * (mod/(float)len) - PI * 0.5f));
  analogWrite(pin_number, (unsigned char) (f * 255.0f));
}
