#include "DHT.h"
#include "LiquidCrystal_I2C.h"
#include "Wire.h"
#include "SoftwareSerial.h"

/*
 *  LCD Pins            Arduino Pin 
 *  ----------------------------------
 *  GND                 GND
 *  VCC                 5V
 *  SDA                 SDA
 *  SCL                 SCL
 *
 *  DHT Pins            Arduino Pin 
 *  ----------------------------------
 *  1 (VDD, 3~5.5V)     3.3V
 *  2 (Data)            D5
 *  3 (NC)
 *  4 (GND)             GND
 *
 *  ESP8266 Pin         Arduino Pin
 *  ----------------------------------
 *  1 (GND)             GND
 *  2 (GPIO2)           NC
 *  3 (GPIO0)           NC
 *  4 (RX)              D3
 *  5 (VCC)             3.3V (External Power)
 *  6 (RST)             NC
 *  7 (CH_PD)           3.3V
 *  8 (TX)              D4
 * 
 */

unsigned long CurrTimestamp = millis();

/* ---  Serial setup  --- */
#define BAUD_SERIAL       9600        // 序列埠傳輸速率設定為 9600 bps
#define DELAY_MS          1000        // 系統預設延遲時間

/* ---  LCD setup  */
LiquidCrystal_I2C LCD1602(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


/* ---  DHT  --- */
#define DHTPIN  5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

unsigned long DHT_Interval = 2000;
unsigned long DHT_LastTime = millis();


/* ---  ESP8266 setup  --- */
/*
 * Be aware that 115200 will not work with software serial.
 * With Arduino Uno must set the a lower baud rate, ex. 9600
 */
#define BAUD_ESP8266  115200
#define WiFi_SSID     "your wifi SSID"
#define WiFi_PSWD     "your wifi PSWD"

#define ESP_TX     3                  // ESP8266 TX -> MCU D3
#define ESP_RX     4                  // ESP8266 RX -> MCU D4
SoftwareSerial ESP8266(ESP_TX, ESP_RX);

/* ---  MQTT setup --- */
char* HOST = "api.thingspeak.com";
String WriteApiKey = "0123456789ABCDEF";
String ReadApiKey  = "0123456789ABCDEF";


void setup() 
{
  Serial.begin(BAUD_SERIAL);
  Serial.println("Arduino...OK!");
  
  lcd_Setting();
  Serial.println("LCD...OK");

  dht.begin();
  Serial.println("DHT...OK!");

  wifi_Setting();
}

void loop()
{
  if ( millis() - DHT_LastTime >= DHT_Interval)
  {
    DHT_LastTime = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (isnan(h) || isnan(t)) { 
      Serial.println("Failed to read from DHT sensor!");
      return;
    }  
    printSerial(h, t); printLCD(h, t);

    sendMqttMesg(h, t);
  }
}

void lcd_Setting()
{
  // LCD initializing, default state is open backlight.
  LCD1602.begin(16, 2);      
  LCD1602.clear();

  // Flash three times
  for(int i = 0; i < 3; i++) {
    LCD1602.backlight();    // open backlight
    delay(250);
    LCD1602.noBacklight();  // close backlight
    delay(250);
  }
  LCD1602.backlight();

  // 輸出初始化文字
  LCD1602.setCursor(0, 0); // 設定游標位置在第一行行首
  LCD1602.print("LCD...OK!");
}

void wifi_Setting()
{
  Serial.println("\nESP8266 setup start...");

  // Setup ESP8266 Baud rate
  ESP8266.begin(BAUD_ESP8266);
  
  // Restart module
  sendATcmd("AT+RST\r\n", 5000);

  // Setup WiFi mode: 1 is Station
  sendATcmd("AT+CWMODE=1\r\n", 2000);

  // Connect to WiFi
  sendATcmd("AT+CWJAP=\""WiFi_SSID"\",\""WiFi_PSWD"\"\r\n", 3000);

  // Get local IP address
  sendATcmd("AT+CIFSR\r\n", 1000);

  Serial.println("ESP8266 setup finished.");
}

void sendMqttMesg(float h, float t)
{
  String mesg = "GET /update?key=XBRHX7XEJW4D3RWQ";
  mesg += "&field1=" + String(t);
  mesg += "&field2=" + String(h);
  mesg += "\r\n";
  int mesg_len = mesg.length();

  // ESP8266 connects to the server as a TCP client. 
  char cipStart[128];
  sprintf(cipStart, "AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n", HOST);
  sendATcmd(cipStart, 1000);

  // ESP8266 sends data to the server
  char cipSend[16];
  sprintf(cipSend, "AT+CIPSEND=%d\r\n", mesg_len);
  sendATcmd(cipSend, 1000);
  sendATcmd(mesg.c_str(), 1000);

  // End the TCP connection. 
}

void sendATcmd(char *cmd, unsigned long delay)
{
  ESP8266.print(cmd);
//  Serial.println(cmd);

  unsigned long curr_time = millis();
  while ( millis() - curr_time < delay ) {}

  String response = "";
  while ( ESP8266.available() ) {
    char ch = ESP8266.read();
    response += ch;
  }
//  Serial.println(response);
  Serial.print(response);
  Serial.println();
}

void printSerial(float h, float t)
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
