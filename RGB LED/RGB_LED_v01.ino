#define LEDR 9
#define LEDG 10
#define LEDB 11

char buffer[81];

const int potPin = 17; // A3 pin

int redVal = 0;
int grnVal = 0;
int bluVal = 0;

const int color_len = 170;
const int color_seg = 1024 / color_len;

void setup() 
{
  Serial.begin(9600);
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  test();
  delay(3000);
}

void loop() 
{
  int potVal = analogRead(potPin);   // read the potentiometer value at the input pin
  Serial.print(potVal); 

  if (potVal >= color_len*color_seg) potVal = color_len*color_seg - 1;
  int seg_num = potVal / color_len;
  Serial.print(", "); Serial.print(seg_num); 

  int col_num = potVal % color_len;
  int color_val = map(col_num, 0, color_len, 0, 255);

  switch(seg_num)
  {
    case 0:
      redVal = 255; grnVal = color_val; bluVal = 0; // 255, 0, 0 -> 255, 255, 0
      break;
    case 1:
      redVal = 255 - color_val; grnVal = 255; bluVal = 0; // 255, 255, 0 -> 0, 255, 0
      break;
    case 2:
      redVal = 0; grnVal = 255; bluVal = color_val; // 0, 255, 0 -> 0, 255, 255
      break;
    case 3:
      redVal = 0; grnVal = 255 - color_val; bluVal = 255; // 0, 255, 255 -> 0, 0, 255
      break;
    case 4:
      redVal = color_val; grnVal = 0; bluVal = 255; // 0, 0, 255 -> 255, 0, 255
      break;
    case 5:
      redVal = 255; grnVal = 0; bluVal = 255 - color_val; // 255, 0, 255 -> 255, 0, 0
      break;
    default:
      redVal = 255; grnVal = 127; bluVal = 255;
  }

  sprintf(buffer,", %d, %d, %d", redVal, grnVal, bluVal);
  Serial.println(buffer); 

  analogWrite(LEDR, redVal);
  analogWrite(LEDG, grnVal);
  analogWrite(LEDB, bluVal);  
  delay(30);
}

// 關閉LED
void turnoff() 
{
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, LOW);
}

void test()
{
  setColor(255, 0, 0);  // red
  delay(1000);
  setColor(0, 255, 0);  // green
  delay(1000);
  setColor(0, 0, 255);  // blue
  delay(1000);
  setColor(255, 255, 0);  // yellow
  delay(1000);  
  setColor(255, 0, 255);  // purple
  delay(1000);
  setColor(0, 255, 255);  // aqua
  delay(1000);
}

void setColor(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(LEDR, red);
  analogWrite(LEDG, green);
  analogWrite(LEDB, blue);  
}
