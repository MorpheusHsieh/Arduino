#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>;

/*
 * NodeMCU          Device
 * ---------------------------------
 * 3.3V             DHT VCC        
 * GPIO16 = D0      DHT Data Pin
 * GND              DHT GND
 * 
 * GPIO5  = D1      LCD SDA
 * GPIO4  = D2      LCD SCL
 * 3.3V             LCD VCC
 * GND              LCD GND
 */

unsigned long LastMillis = millis();

// DHT Setup
#define DHTPIN  0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); 

boolean DHT2LCD = true;
boolean DHT2COM = false;

unsigned long DHT_Interval = 1000;

// LCD Setup
LiquidCrystal_I2C LCD1602(0x27, 16, 2);

// WiFi connection
const char* WiFi_SSID = "DADA9631";
const char* WiFi_PSWD = "22453975";

IPAddress StaticIP(192,168,100,128);
IPAddress  Gateway(192,168,100,9);
IPAddress  Netmask(255,255,255,0);

// MQTT
const char* mqttServ = "m16.cloudmqtt.com";
const int   mqttPort = 11344;
const char* mqttUser = "rwgzuosm";
const char* mqttPswd = "fLPRoVo0tma7";
const char *clientID = "";
const char* topicSub = "test/dht";
const char* topicPub = "test/dht";


// Function declare
void dht_Setting();
void lcd_Setting();
void connectWiFi();
void readTempHumid();

void setup() 
{
  delay(500);
  
  // Setup console
  Serial.begin(115200);
  Serial.println("Serial...OK!");
  
  dht_Setting();  // Setup DHT
  lcd_Setting();  // Setup LCD

  connectWiFi();
  Serial.println();
}

void loop() 
{
  unsigned long currMillis = millis();
  if (currMillis - LastMillis >= DHT_Interval) {
    LastMillis = currMillis;
    readTempHumid();
  }
}

void dht_Setting()
{
  dht.begin();

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.println(F("DHT... OK!"));
}

void lcd_Setting()
{
  // Initializing LCD...
  LCD1602.begin();

  // Flash 3 times
  for(int i = 0; i < 3; i++) {
    LCD1602.backlight();   // Open backlight
    delay(250);
    LCD1602.noBacklight(); // close backlight 
    delay(250);
  }
  LCD1602.backlight();

  // 輸出初始化文字
  LCD1602.setCursor(0, 0);
  LCD1602.print("LCD...OK!");
  delay(500);

  Serial.println(F("LCD...OK!"));
}

void readTempHumid()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  } 

  if (DHT2COM) {
    String outstr = "Humidity: " + String(h) + " %,\t";
    outstr += "Temperature: " + String(t) + " °C";
    Serial.println(outstr);
  }

  if (DHT2LCD) {
    String row1 = "Humi: " + String(h) + " %";

    // '°' = (char)223
    String degC = String((char)223) + "C";
    String row2 = "Temp: " + String(t) + " " + degC;

    LCD1602.clear();
    LCD1602.setCursor(0, 0); LCD1602.print(row1);
    LCD1602.setCursor(0, 1); LCD1602.print(row2);
  }
}

void connectWiFi()
{
  Serial.println("\r\nWiFi connecting ...");

  WiFi.begin(WiFi_SSID, WiFi_PSWD);
  delay(500);

  unsigned long delay_ms = 500;
  unsigned long timeout = 15000;
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(delay_ms);
    timeout -= delay_ms;
    if (timeout <= 0) break;
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connect fail ...");
    Serial.printf("\r\nConnection status: %d\n", WiFi.status());
    Serial.println("\r\nWiFi Status: ");
    WiFi.printDiag(Serial);
    return;
  }

  Serial.println("WiFi connected!");

  // Get dynamic IP Address
  Serial.print("\r\nDynamic IP: "); 
  Serial.println(WiFi.localIP());

  // Assign static IP address
  WiFi.config(StaticIP, Gateway, Netmask);

  // Check static IP Address
  Serial.print("Static IP: ");
  Serial.println(WiFi.localIP());
}
