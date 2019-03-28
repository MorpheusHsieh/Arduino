/*
AnalogButton_Combos
Version 0.1

Connection more then one button to a single analog pin. Utilizing
software debounce to prevent registering multiple button press
while allow for 2 button combos to be registered.  

The Circuit:
 Most other analog buttons circuits call for the resistors to be
 lined up in series from ground. The analog pin and each button
 connect off one of the resistors. My cuicuit requires that the
 resistors tie in from +5 to the buttons. The buttons all connect
 to the analog pin which is tied to ground threw a 1k resistor as
 seen in the diagram below. 

        Analog pin 5
           |
Ground--1K--|--------|--------|-------|-------|-------|
            |        |        |       |       |       |
           btn1     btn2     btn3    btn4    btn5    btn6
            |        |        |       |       |       |
         220 Ohm  390 Ohm  680 Ohm   2.2K    4.7K    10K
            |--------|--------|-------|-------|-------|---- +5V
           
Created By: Michael Pilcher
February 24, 2010
*/
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int j = 1; // integer used in scanning the array designating column number
//2-dimensional array for asigning the buttons and there high and low values

int Button[21][3] = {{1, 834, 835},  // button 1  *up
                     {2, 734, 738},  // button 2  *down
                     {3, 604, 611},  // button 3  *left
                     {4, 314, 319},  // button 4  *right
                     {5, 174, 179},  // button 5  *A
                     {6, 87, 92},    // button 6  *B
                     {7, 892, 897},  // button 1 + button 2
                     {8, 870, 878},  // button 1 + button 3
                     {9, 847, 852},  // button 1 + button 4
                     {10, 840, 845},  // button 1 + button 5
                     {11, 837, 838},  // button 1 + button 6
                     {12, 815, 822},  // button 2 + button 3
                     {13, 765, 770},  // button 2 + button 4
                     {14, 749, 754},  // button 2 + button 5
                     {15, 741, 746},  // button 2 + button 6
                     {16, 669, 675},  // button 3 + button 4
                     {17, 637, 642},  // button 3 + button 5
                     {18, 620, 624},  // button 3 + button 6
                     {19, 404, 409},  // button 4 + button 5
                     {20, 359, 364},  // button 4 + button 6
                     {21, 239, 243}}; // button 5 + button 6

int analogpin = 5;       // analog pin A5 to read the buttons 
int label = 0;           // for reporting the button label
int counter = 0;         // how many times we have seen new value
long time = 0;           // the last time the output pin was sampled
int debounce_count = 50; // number of millis/samples to consider before declaring a debounced input
int current_state = 0;   // the debounced input value
int ButtonVal;

void setup()
{
  Serial.begin(9600); 
}

void loop()
{
  // If we have gone on to the next millisecond
  if (millis() != time)
  {
    // check analog pin for the button value and save it to ButtonVal
    ButtonVal = analogRead(analogpin);
    if(ButtonVal == current_state && counter >0)
    { 
      counter--;
    }
   if(ButtonVal != current_state)
   {
     counter++;
   }
   // If ButtonVal has shown the same value for long enough let's switch it
   if (counter >= debounce_count)
   {
     counter = 0;
     current_state = ButtonVal;
     //Checks which button or button combo has been pressed
     if (ButtonVal > 0)
     {
       ButtonCheck();
     }
   }
   time = millis();
 }
}

void ButtonCheck()
{
 // loop for scanning the button array.
 for(int i = 0; i <= 21; i++)
 {
   // checks the ButtonVal against the high and low vales in the array
   if(ButtonVal >= Button[i][j] && ButtonVal <= Button[i][j+1])
   {
     // stores the button number to a variable
     label = Button[i][0];
     Action();      
   }
 }
}

void Action()
{
 if(label == 1)
 {
   Serial.println("Up Button");
 }
 if(label == 2)
 {
   Serial.println("Down Button");
 }
 if(label == 3)
 { 
   Serial.println("Left Button");
 }
 if(label == 4)
 {
   Serial.println("Right Button");
 }
 if(label == 5)
 {
   Serial.println("Action Button #1");
 }
 if(label == 6)
 {
   Serial.println("Action Button #2");
 }
 if(label == 8)
 {
   Serial.println("Left and Up Buttons");
 }
 if(label == 9)
 {
   Serial.println("Right and Up Buttons");
 }
 if(label == 12)
 {
   Serial.println("Left and Down Buttons");
 }
 if(label == 13)
 {
   Serial.println("Right and Down Buttons");
 }
 if(label == 21)
 {
   Serial.println("Action Buttons #1 and #2");
 }    
      
 //Serial.println("Button =:");
 //Serial.println(label);
 //delay(200);
}


