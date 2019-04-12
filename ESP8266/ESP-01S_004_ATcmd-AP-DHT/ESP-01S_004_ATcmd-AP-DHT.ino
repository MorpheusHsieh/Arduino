#include "DHT.h" // package required from https://github.com/adafruit/DHT-sensor-library
#include <SoftwareSerial.h>

#define BAUD      9600
#define PIN_DHT   5
#define PIN_RX    3
#define PIN_TX    2

DHT dht(PIN_DHT, DHT11);            // digital pin5 connected to the DHT11
SoftwareSerial mySerial(PIN_RX, PIN_TX); // mySerial8266 mySerial-01: Tx, Rx

void setup() {
  dht.begin(); delay(2000);
  Serial.println("DHT11...Ok");

  Serial.begin(BAUD);
  Serial.println("Arduino...Ok");

  mySerial.begin(BAUD);
  Serial.println("mySerial8266...Ok");

  Serial.println("\nInitialize mySerial-01S as HTTP Server...");
  initmySerial();
  Serial.println("\r\nServer started at port 80 ...");
}

void initmySerial() {
  sendATcmd("AT+GMR\r\n",            1000);
  sendATcmd("AT+RST\r\n",            5000);
  sendATcmd("AT+CWMODE=2\r\n",       2000);
  sendATcmd("AT+CWMODE?\r\n",        1000);
  sendATcmd("AT+CIPMUX=1\r\n",       1000);
  sendATcmd("AT+CIPSERVER=1,80\r\n", 3000);
  sendATcmd("AT+CIFSR\r\n",          1000);
}

void sendATcmd(char *cmd, unsigned int delay)
{
  mySerial.print(cmd);
  unsigned long timeout = millis() + delay;
  while(millis() < timeout ) {} // NOP

  Serial.println();
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  };
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.print("% ");
  Serial.print("temperature = ");
  Serial.print(t);
  Serial.println("*C");
  delay(1000);
}
