/*
 * For NodeMCU
 */

#define LED_ESP   2
#define LED_MCU  16


void setup() 
{
  pinMode(LED_ESP, OUTPUT);
  pinMode(LED_MCU, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_MCU, LOW);   // Turn the MCU LED on
  digitalWrite(LED_ESP, HIGH);  // Turn the ESP LED of

  delay(1000);                  // Wait for a second
  digitalWrite(LED_MCU, HIGH);  // Turn the MCU LED off
  digitalWrite(LED_ESP, LOW);   // Turn the ESP LED on
  delay(1000);                  
}
