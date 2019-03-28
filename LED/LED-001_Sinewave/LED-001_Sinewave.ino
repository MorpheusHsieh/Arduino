#define SINSIZE 255
#define TWOPIOVERSINSIZE    (PI*2)/(float)SINSIZE

#define LEDPIN 10
unsigned char sins_of_our_fathers[SINSIZE];

void setup()
{
 Serial.begin(9600);
 pinMode(10, HIGH);

 Serial.print("Precomputing sine wave.");
 for(unsigned char i=0;i<SINSIZE;i++) 
 {
   sins_of_our_fathers[i] = (128.0*sin( (float)i*TWOPIOVERSINSIZE) ) + 127;
   Serial.print(".");
 }
 Serial.println("Done.");
}

void loop()
{
 for(unsigned char i=0;i<SINSIZE;i++)
 {
   analogWrite(LEDPIN, sins_of_our_fathers[i]);
   delay(10);
   Serial.print("SIN: ");
   Serial.println( sins_of_our_fathers[i] , DEC);
 }
 
 for(unsigned char i=0;i<255;i++)
 {
   analogWrite(LEDPIN, i);
   delay(10);
   Serial.print("NOTSIN: ");
   Serial.println( i , DEC);
 }
 
}
