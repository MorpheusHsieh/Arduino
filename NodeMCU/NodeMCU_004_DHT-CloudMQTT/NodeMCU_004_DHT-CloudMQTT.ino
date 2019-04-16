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
const char* WiFi_SSID = "xxxx";
const char* WiFi_PSWD = "xxxx";

IPAddress StaticIP(192,168,100,128);
IPAddress  Gateway(192,168,100,9);
IPAddress  Netmask(255,255,255,0);

// MQTT
const char* mqttServer = "m16.cloudmqtt.com";
const int   mqttPort = 10794;
const char* mqttUser = "xxxx";
const char* mqttPswd = "xxxx";
unsigned long MQTT_Interval = 60000;

WiFiClient espClient;
PubSubClient client(espClient);

// Function declare
void dht_Setting();
void lcd_Setting();
void readTempHumid();
void connectWiFi();
void connectMQTT();
void callback(char*, byte*, unsigned int);
void sendTempHuid();

unsigned long SendDhtTime = millis();
unsigned long SendMqttTime = SendDhtTime - MQTT_Interval;


// ----------------------------------------------------------------------------

void setup() 
{
  delay(500);
  
  // Setup console
  Serial.begin(115200);
  Serial.println("Serial...OK!");
  
  dht_Setting();  // Setup DHT
  lcd_Setting();  // Setup LCD
  Serial.println();

  connectWiFi();

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  connectMQTT();
  
  Serial.println();
}

// ----------------------------------------------------------------------------

void loop() 
{
  client.loop();

  // Show DHT information on LCD
  unsigned long currMillis = millis();

  if (currMillis - SendDhtTime >= DHT_Interval) {
    SendDhtTime = currMillis;
    readTempHumid();
  }

  if (currMillis - SendMqttTime >= MQTT_Interval) {
    SendMqttTime = currMillis;
    sendTempHumid();
  }
}


// ----------------------------------------------------------------------------

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
    result += ':';
  }
  return result;
}

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

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
  Serial.println();

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

  /* 
   * Assign static IP address to ESP8266, 
   * but the MQTT connect will have failed. 
   */
  //  WiFi.config(StaticIP, Gateway, Netmask);
  //  Serial.print("Static IP: ");
  //  Serial.println(WiFi.localIP());

  String macStr = WiFi.macAddress();
  Serial.print("MAC Address: "); Serial.println(macStr);
}

// ----------------------------------------------------------------------------

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
}

// ----------------------------------------------------------------------------

void connectMQTT()
{
  int i = 10;  // Maximum reconnect times
  while (!client.connected() && (--i) >= 0)
  {
    Serial.print("Connecting to MQTT...");
 
    if (client.connect("ESP8266Client", mqttUser, mqttPswd)) {
      Serial.println("connected");  
    } else {
      Serial.printf(" failed with state %d, %d\r\n", client.state(), i);
      delay(2000);
    }
  }

  if (!client.connected()) {
    Serial.println("MQTT connet failed!");
    return;
  }
}

// ----------------------------------------------------------------------------

void sendTempHumid()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  String json = "{ \"Humi\": \"" + String(h) + " %\""
              + ", \"Temp\": \"" + String(t) + " °C\" }";
  Serial.println(json);
  
  client.publish("esp/test", (char*)json.c_str());
  client.subscribe("esp/test");
}
