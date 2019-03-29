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

void sendHTML(byte connID,char* msg) 
{
  extern int connCount; 
  String html;
  char cipSend[128];
  char cipClose[128];

  float h = dht.readHumidity();   // Reading temperature or humidity takes about 250 milliseconds
  float t = dht.readTemperature(); // Read temperature as Celsius (the default)
   
  html += "<html>";
  html += "<head>";
  html += "  <meta http-equiv=\"refresh\" content=\"10\">";
  html += "  <title>From mySerial-01</title>";
  html += "</head>";
  html += "<body>\n\r";
  html += "  <p>ClientMsg: "; html += msg; html += "</p>\n\r";
  html += "  <p>Humi: "; html += h; html += "%</p>\n\r";
  html += "  <p>Temp: "; html += t; html += "*C</p>\n\r";
  html += "</body>";
  html += "</html>";
  
  Serial.println(html);
  sprintf(cipSend,"AT+CIPSEND=%d,%d\r\n",connID,html.length());
  sprintf(cipClose,"AT+CIPCLOSE=%d\r\n",connID);
  sendATcmd(cipSend,1000);
  sendATcmd(html.c_str(),1000);
  sendATcmd(cipClose,1000);
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

  
//  // send AT command to mySerial-01 form console (serial)
//  if ( Serial.available() ) {
//    mySerial.write( Serial.read() );
//  }
//  if ( mySerial.available() ) { // receive message from mySerial-01
//    if ( mySerial.find("+IPD,") ) { // detect the client's request
//      String msg="";
//      byte connID = mySerial.read()-48; // client's connection ID
//      while( mySerial.available() ) { // collect client's request from the web browser
//        msg += (char)mySerial.read();
//        delay(20); // the delay will let the message become more stable
//      }
//      sendHTML(connID,msg.c_str()); // send HTML message to client
//      Serial.flush();
//    }
//  }
}
