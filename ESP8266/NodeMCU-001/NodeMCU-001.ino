#include <ESP8266WiFi.h> 
#include <PubSubClient.h>
#include <DHT.h>
 
const char* ssid = "DADA9631";
const char* pswd = "22453975";
String WriteApiKey = "XBRHX7XEJW4D3RWQ";
String ReadApiKey  = "NYTNGMDKZ7M63550";
String GetFormatStr = "http://api.thingspeak.com/update?key=%s&field1=%d&field2=%d";

/* ---  DHT  --- */
#define DHTPIN    D3
#define DHTTYPE   DHT11
DHT dht(DHTPIN, DHTTYPE);

/* --- LED  --- */
#define LEDPIN    D2

void setup() {
  pinMode(LEDPIN, OUTPUT);

  Serial.begin(9600);
  
  dht.begin();
  Serial.println("DHT...OK!");
}

// the loop function runs over and over again forever
void loop() 
{
  readDHT11();
}

void readDHT11()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  String degC = " " + String((char)223) + "C";
  String outstr = "Humidity: " + String(h) + " %"
                + ", Temperature: " + String(t) + degC;
  Serial.println(outstr);
}
