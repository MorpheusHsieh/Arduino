#include "DHT.h"
#include "LiquidCrystal_I2C.h"
#include <SoftwareSerial.h>
#include <Wire.h>

#define BAUD_SERIAL   9600
#define BAUD_ESP8266  9600

#define PRINT_DHT_SERIAL    false
#define PRINT_DHT_LCD       true
#define PRINT_WiFi_SERIAL   true
#define PRINT_WiFi_LCD      true

#define WIFI_SSID "DADA9631"
#define WIFI_PSWD "22453975"

// 設定 LCD I2C 位址
LiquidCrystal_I2C LCD1602(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define PIN_DHT   5     // D5 connected to the DHT11
#define DHTTYPE   DHT11
DHT DHT_SENSOR(PIN_DHT, DHTTYPE);

#define PIN_RX    3     // D3 -> ESP TX, 
#define PIN_TX    2     // D2 -> ESP RX, 
SoftwareSerial ESP8266(PIN_RX, PIN_TX);

void setup() 
{
  String mesg = "";
  
  lcd_setting();
  LCD1602.clear();

  mesg = "Arduino...Ok";
  Serial.begin(BAUD_SERIAL);
  Serial.println(mesg);
  LCD1602.setCursor(0, 0); LCD1602.print(mesg);
  delay(1000);

  mesg = "DHT Sensor...Ok";
  DHT_SENSOR.begin(); 
  Serial.println("mesg");
  LCD1602.setCursor(0, 0); LCD1602.print(mesg);
  delay(1000);

  wifi_setting();
}

void loop() 
{
  float h = DHT_SENSOR.readHumidity();
  float t = DHT_SENSOR.readTemperature();
  if (PRINT_DHT_SERIAL) printMonitor(h, t);
  if (PRINT_DHT_LCD)    printLCD(h, t);
  delay(1000);

  
//  // send AT command to ESP8266-01 form console (serial)
//  if ( Serial.available() ) {
//    ESP8266.write( Serial.read() );
//  }
//  if ( ESP8266.available() ) { // receive message from ESP8266-01
//    if ( ESP8266.find("+IPD,") ) { // detect the client's request
//      String msg="";
//      byte connID = ESP8266.read()-48; // client's connection ID
//      while( ESP8266.available() ) { // collect client's request from the web browser
//        msg += (char)ESP8266.read();
//        delay(20); // the delay will let the message become more stable
//      }
//      sendHTML(connID,msg.c_str()); // send HTML message to client
//      Serial.flush();
//    }
//  }
}

void lcd_setting()
{
  // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
  LCD1602.begin(16, 2);      

  // 閃爍三次
  for(int i = 0; i < 3; i++) {
    LCD1602.backlight(); // 開啟背光
    delay(250);
    LCD1602.noBacklight(); // 關閉背光
    delay(250);
  }
  LCD1602.backlight();

  // 輸出初始化文字
  LCD1602.setCursor(0, 0); // 設定游標位置在第一行行首
  LCD1602.print("LCD is ready ...");
  delay(1000);
}

void wifi_setting() 
{
  Serial.println("\nESP8266 setup start ...");

  ESP8266.begin(BAUD_ESP8266);
  sendATcmd("AT+GMR\r\n",            1000);
  sendATcmd("AT+RST\r\n",            5000);
  sendATcmd("AT+CWMODE=2\r\n",       2000);
  sendATcmd("AT+CWMODE?\r\n",        1000);
  sendATcmd("AT+CIPMUX=1\r\n",       1000);
  sendATcmd("AT+CIPSERVER=1,80\r\n", 3000);
  sendATcmd("AT+CIFSR\r\n",          1000);

  Serial.println("\r\nESP8266 setup finished ...");
  delay(1000);
}

void printMonitor(float h, float t)
{
  String outstr = "Humi = " + String(h) + " %"
                +",\tTemp = " + String(t) + "°C ";
  Serial.println(outstr);
}

void printLCD(float h, float t)
{
  String row1 = "Humi: " + String(h) + " %";

  // '°' = (char)223
  String unit = String((char)223) + "C";
  String row2 = "Temp: " + String(t) + unit;

  LCD1602.clear();
  LCD1602.setCursor(0, 0); LCD1602.print(row1);
  LCD1602.setCursor(0, 1); LCD1602.print(row2);
}

void sendATcmd(char *cmd, unsigned int delay)
{
  ESP8266.print(cmd);
  unsigned long timeout = millis() + delay;
  while(millis() < timeout ) {} // NOP

  String response = "";
  while (ESP8266.available()) {
    char ch = ESP8266.read();
    response += ch;
  };
  if (PRINT_WiFi_SERIAL) Serial.print(response);

  if (PRINT_WiFi_LCD) {
    String cmdstr((char*)cmd);
    cmdstr.trim();

    String result = response;
    result.replace(cmdstr, "");
    result.replace("OK", "");
    result.substring(0, 16);
    result.trim();
    if (result.length() == 0) result = "OK";
    // Serial.println("\r\nResult: '" + result + "'");

    LCD1602.clear();
    LCD1602.setCursor(0, 0); LCD1602.print(cmdstr);
    LCD1602.setCursor(0, 1); LCD1602.print(result);
  }
}

void sendHTML(byte connID,char* msg) 
{
  extern int connCount; 
  String html;
  char cipSend[128];
  char cipClose[128];

  // Reading temperature or humidity takes about 250 milliseconds
  float h = DHT_SENSOR.readHumidity();   
  
  // Read temperature as Celsius (the default)
  float t = DHT_SENSOR.readTemperature(); 
   
  html += "<html>";
  html += "<head>";
  html += "  <meta http-equiv=\"refresh\" content=\"10\">";
  html += "  <title>From ESP8266-01</title>";
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
  sendATcmd(cipSend, 1000);
  sendATcmd(html.c_str(), 1000);
  sendATcmd(cipClose, 1000);
}
