#include "DHT.h"
#include "LiquidCrystal_I2C.h"
#include <SoftwareSerial.h>
#include <Wire.h>

#define BAUD_SERIAL   9600
#define BAUD_ESP8266  115200

#define PRINT_DHT_SERIAL    false

#define AP_SSID "ESP8266"
#define AP_PSWD "1234test"

// 設定 LCD I2C 位址
#define PRINT_LCD_INTERVAL        1000
unsigned long LatestPrintLcdTime = millis();
LiquidCrystal_I2C LCD1602(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define PIN_DHT   5     // D5 connected to the DHT11
#define DHTTYPE   DHT11
DHT DHT_SENSOR(PIN_DHT, DHTTYPE);

#define PIN_TX    3     // D3 -> ESP RX, 
#define PIN_RX    4     // D4 -> ESP TX, 
SoftwareSerial ESP8266(PIN_RX, PIN_TX);

void setup() 
{
  String mesg = "";

  mesg = "Arduino...Ok";
  Serial.begin(BAUD_SERIAL);
  Serial.println(mesg);

  mesg = "DHT...Ok";
  DHT_SENSOR.begin(); 
  Serial.println(mesg);
  
  lcd_setting();

  wifi_setting();
}

void lcd_setting()
{
  // LCD initializing, default is open backlight
  LCD1602.begin(16, 2);      
  LCD1602.clear();

  // Flash three times
  for(int i = 0; i < 3; i++) {
    LCD1602.backlight();   delay(250);    // open backlight
    LCD1602.noBacklight(); delay(250);    // cloase backlight
  }
  LCD1602.backlight();

  // Print ready message
  LCD1602.setCursor(0, 0); // Set cursor at first row, first col
  LCD1602.print("LCD...OK!");
}

void wifi_setting() 
{
  Serial.println("\nESP8266 setup ...");

  // Setup ESP8266 Baud rate
  ESP8266.begin(BAUD_ESP8266);
  
  // View version info
  sendATcmd("AT+GMR\r\n", 1000);

  // Restart module
  sendATcmd("AT+RST\r\n", 5000);

  // Setup WiFi mode: 2 is Access Point
  sendATcmd("AT+CWMODE=2\r\n", 2000);

  // Configuration of softAP mode
  sendATcmd("AT+CWSAP=\""AP_SSID"\",\""AP_PSWD"\",11,3\r\n", 2000);

  // Get local IP address
  sendATcmd("AT+CIFSR\r\n", 1000);

  // Enable multiple connections
  sendATcmd("AT+CIPMUX=1\r\n", 2000);

  // Configure as server
  sendATcmd("AT+CIPSERVER=1,80\r\n", 2000);
  
  Serial.println("\r\nServer started at port 80...");
}

void printMonitor(float h, float t)
{
  String outstr = 
    "Humi = " + String(h) + " %" +",\tTemp = " + String(t) + "°C ";
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

  String response = "";
  unsigned long timeout = millis() + delay;
  while(ESP8266.available() || millis() < timeout ) {
    while (ESP8266.available()) {
      char ch = ESP8266.read();
      response += ch;
    }
  }
  Serial.println(response);
}

void sendHTML(byte connID,char* msg) 
{
  //  extern int connCount; 

  // Reading temperature or humidity takes about 250 milliseconds
  float h = DHT_SENSOR.readHumidity();   
  
  // Read temperature as Celsius (the default)
  float t = DHT_SENSOR.readTemperature(); 
   
  String html = "";
  html += "<html><head><meta charset=\"utf-8\">\n\r";
  html += "  <meta http-equiv=\"refresh\" content=\"10\">\n\r";
  html += "  <title>ESP-01 IoT Test</title>\n\r";
  html += "</head>\n\r";
  html += "<body>\n\r";
  html += "  <p>ClientMsg: ";   html += msg; html += "</p>\n\r";
  html += "  <p>Humidity: ";    html += h;   html += " %</p>\n\r";
  html += "  <p>Temperature: "; html += t;   html += " &deg;C</p>\n\r";
  html += "</body>\n\r";
  html += "</html>\n\r";

  Serial.println("\r\n"+html);
  
  // Send HTML content to user
  char cipSend[128];
  sprintf(cipSend, "AT+CIPSEND=%d,%d\r\n", connID, html.length());
  sendATcmd(cipSend, 1000);
  sendATcmd(html.c_str(), 1000);

  // disconnect with user
  char cipClose[128];
  sprintf(cipClose, "AT+CIPCLOSE=%d\r\n", connID);
  sendATcmd(cipClose, 1000);
}

void loop() 
{
  if ( millis() - LatestPrintLcdTime >= PRINT_LCD_INTERVAL )
  {
    float h = DHT_SENSOR.readHumidity();
    float t = DHT_SENSOR.readTemperature();
    if (PRINT_DHT_SERIAL) { printMonitor(h, t); }
    printLCD(h, t);

    LatestPrintLcdTime = millis();
  }

  // Receive message from ESP8266-01
  if (ESP8266.available())
  { 
    // Detect the client's request
    if ( ESP8266.find("+IPD,") ) 
    {
      byte connID = ESP8266.read() - 48;    // client's connection ID

      String msg = "+IPD";
      while( ESP8266.available() ) {        // collect client's request from the web browser
        msg += (char)ESP8266.read();
        delay(20);                          // the delay will let the message become more stable
      }
      Serial.println(); Serial.print(msg);
      sendHTML(connID, msg.c_str());        // send HTML message to client
      Serial.flush();
    }
  }

  delay(500);
}
