#define LEDPIN 0

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LEDPIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LEDPIN, HIGH);
  delay(2000);
  digitalWrite(LEDPIN, LOW);
  delay(2000);
}
