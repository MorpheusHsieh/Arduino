
const byte btnPin = A0;
const byte resPin = A1;
const byte motPin = A2;
const byte eyePin = A3;
const byte ledPin = 10;

int val = 0, avg = 0;
int min = 1024, max = 0;
const int avg_offset = 50;

void setup() 
{
  Serial.begin(9600); 
  pinMode(btnPin, INPUT); 
  pinMode(ledPin, OUTPUT);

}

void loop()
{
  Serial.println("Press button ...");

  if (analogRead(btnPin) > 0)
  {
    delay(20);
    min = 1024, max = 0; avg = 0;
    while (analogRead(btnPin))
    {
      int val = analogRead(btnPin);
      avg = 0.5 * avg + 0.5 * val;
      if (val < min) min = val;
      if (max < val) max = val;

    Serial.print("min: "); Serial.print(min);
    Serial.print(", max: "); Serial.print(max);
    Serial.print(", avg: "); Serial.print(avg);
    Serial.println();
    }
  }
  delay(100);
} 


