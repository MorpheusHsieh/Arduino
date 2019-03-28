#define LEDR 9
#define LEDG 10
#define LEDB 11

char buffer[81];

int wave[8][6] = {
  { 1, 0, 0,  5, 150, 100 },   // red
  { 1, 1, 0,  5, 100,  50 },   // yellow
  { 0, 0, 1,  5,  50,  25 },   // blue
  { 1, 1, 1,  5,   0,   0 },   // white
  { 1, 0, 1,  1,  50,  25 },   // purple
  { 1, 0, 1,  2, 100,  50 },   // purple
  { 1, 0, 1,  4, 200, 100 },   // purple
  { 0, 0, 0,  5,   0,   0 },   // DARK
};

void setup() 
{
  Serial.begin(9600);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);
}

void loop() 
{
  for (int i=0; i<8; i++)
  {
    int factor = (i <= 3) ? (i+1) : (7-i);
    int red = wave[i][0] * factor * 50;
    Serial.println(red);
    
    int grn = wave[i][1] * factor * 50;
    int blu  = wave[i][2] * factor * 50;
    int time_len = wave[i][3];
    int delay1 = wave[i][4];
    int delay2 = wave[i][5];
    unsigned long now = millis(); 
    delay(10);
    while (millis() - now < time_len * 1000)
    {
      analogWrite(LEDR, red);
      analogWrite(LEDG, grn);
      analogWrite(LEDB, blu);  
      delay(delay1);
      analogWrite(LEDR, 0);
      analogWrite(LEDG, 0);
      analogWrite(LEDB, 0);  
      delay(delay2);
    }
  }
}

// 關閉LED
void turnoff() 
{
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
}
