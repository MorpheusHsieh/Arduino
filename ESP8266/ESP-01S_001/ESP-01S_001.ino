#include <SoftwareSerial.h>

#define BAUD    115200
#define TIMEOUT 5000    // mS

SoftwareSerial mySerial(3,2);  // mySerial-01S: Tx, Rx

void setup() {
  Serial.begin(BAUD);
  Serial.println("Arduino...OK");
    
  mySerial.begin(BAUD);

  Serial.println("\r\nESp8266 version:");
  SendCommand("AT+GMR", "Ready");

  Serial.println("\r\nESP8266 reseting...");
  SendCommand("AT+RST", "Ready");
  delay(3000);

  SendCommand("AT+CWMODE?","OK");
  SendCommand("AT+CIFSR", "OK");
//  SendCommand("AT+CIPMUX=1","OK");
//  SendCommand("AT+CIPSERVER=1,80","OK");    

  Serial.println("\r\nESP8266 Ready...");
}

void loop() {
    if (mySerial.available()) {
        Serial.write(mySerial.read());
    }
    if (Serial.available()) {
        mySerial.write(Serial.read());
    }
}

//void setDelay(unsigned int delay) {
//    unsigned long timeout = delay + millis();
//    while(millis() < timeout ) {} // NOP
//}

//void sendATcmd(char* cmd, unsigned int delay) {
//    mySerial.print( cmd ); // send AT command to mySerial-01
//    setDelay(delay);
//}

boolean SendCommand(String cmd, String ack){
  mySerial.println(cmd);    // Send "AT+" command to module
  if (!echoFind(ack))       // timed out waiting for ack string
    return true;            // ack blank or ack found
}
 
boolean echoFind(String keyword)
{
  byte current_char = 0;
  byte keyword_length = keyword.length();
  long deadline = millis() + TIMEOUT;
  while(millis() < deadline) 
  {
    if (mySerial.available()) 
    {
      char ch = mySerial.read();
      Serial.write(ch);
      if (ch == keyword[current_char])
      if (++current_char == keyword_length)
      {
        Serial.println();
        return true;
      }
    }
  }
  return false; // Timed out
}
