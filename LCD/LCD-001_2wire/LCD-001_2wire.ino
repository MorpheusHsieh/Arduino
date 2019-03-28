#include <Wire.h>
#include <LiquidCrystal_SR.h>

// (Data/Enable Pin, Clock Pin, TWO_WIRE)
LiquidCrystal_SR lcd(7, 6, TWO_WIRE);

void setup()
{
  lcd.begin(16,2);               // initialize the lcd
  lcd.home ();                   // go home
  lcd.print("Hello World!!!");
}

void loop()
{
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
}
