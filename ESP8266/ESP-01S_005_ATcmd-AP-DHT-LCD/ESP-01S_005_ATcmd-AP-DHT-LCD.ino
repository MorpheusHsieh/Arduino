#include "DHT.h"
#include "LiquidCrystal_I2C.h"
#include <SoftwareSerial.h>
#include <Wire.h>

#define BAUD      9600

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
  Serial.begin(BAUD);
  Serial.println("Arduino...Ok");
  delay(1000);

  DHT_SENSOR.begin(); 
  Serial.println("DHT Sensor...Ok");
  delay(1000);

  lcd_setting();
//  wifi_setting();
}

void loop() {
  float h = DHT_SENSOR.readHumidity();
  float t = DHT_SENSOR.readTemperature();
  printMonitor(h, t);
  printLCD(h, t);
  delay(1000);
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
  LCD1602.print("Hello, world!");
  delay(1000);
  LCD1602.setCursor(0, 1); // 設定游標位置在第二行行首
  LCD1602.print("LCD is ready ...");
  delay(1000);
}

void wifi_setting() 
{
  Serial.println("\nESP8266 setup start ...");

  ESP8266.begin(BAUD);
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

void sendATcmd(char *cmd, unsigned int delay)
{
  ESP8266.print(cmd);
  unsigned long timeout = millis() + delay;
  while(millis() < timeout ) {} // NOP

  Serial.println();
  while (ESP8266.available()) {
    Serial.write(ESP8266.read());
  };
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
