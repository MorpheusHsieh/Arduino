#include <DHT.h>
#include <ESP8266WiFi.h>

/*
 *  DHT Pins            Arduino Pin 
 *  ----------------------------------
 *  1 (VDD, 3~5.5V)     3.3V
 *  2 (Data)            D3 (GPIO0)
 *  3 (NC)
 *  4 (GND)             GND
 * 
 */

unsigned long currMillis = millis(); // ms

// DHT releated parameters
#define DHTPIN  0
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); 

unsigned long DHT_Interval = 10000; // ms
unsigned long DHT_Lastest  = currMillis - DHT_Interval; // ms
float DHT_Temp, DHT_Humi;

// WiFi SSID and Password 
const char* WiFi_SSID = "xxxxxxxx";
const char* WiFi_PSWD = "xxxxxxxx";

// ESP8266 Web server
WiFiServer server(80);

// ThingSpeak configurastion
const char* TS_Host = "api.thingspeak.com";
const int   TS_PORT = 80;
const char* WriteAPIKey = "xxxxxxxxxxxxxxxx";

// Function declare
void dht_Setting();
void connectWiFi();
void readTempHumid();
String prepareHtmlPage();
void sendToWebServer();
void sendToThingSpeak();

// ----------------------------------------------------------------------------

void setup() 
{
  delay(200);
  
  // Setup console
  Serial.begin(115200);
  Serial.println("Serial...OK!");

  dht_Setting(); 

  connectWiFi();
  if (WiFi.status() != WL_CONNECTED) return;

  server.begin();
  String ip = WiFi.localIP().toString();
  Serial.println();
  Serial.printf("Web server started, open %s in a web browser\n", ip.c_str());

}

// ----------------------------------------------------------------------------

void loop() 
{
  currMillis = millis();

  if (currMillis - DHT_Lastest >= DHT_Interval) {
    DHT_Lastest = currMillis;

    readTempHumid();
    sendToWebServer();
    sendToThingSpeak();
  }
}

// ----------------------------------------------------------------------------

void dht_Setting()
{
  dht.begin();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Fail to read from DHT sensor!"));
    return;
  }
  
  Serial.println(F("DHT working."));
}

// ----------------------------------------------------------------------------

void connectWiFi()
{
  Serial.println("\r\nWiFi connecting ...");

  unsigned long wait_ms  = 500; 

  WiFi.begin(WiFi_SSID, WiFi_PSWD);
  delay(500);

  while (WiFi.status() != WL_CONNECTED)
  {
    wait_ms *= 2;
    Serial.print(String(wait_ms/1000) + " ");
    delay(wait_ms);

    WiFi.begin(WiFi_SSID, WiFi_PSWD);
    delay(500);
  };
  Serial.println();
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.printf("\r\nWiFi connect failed with state = %d!", WiFi.status());
    Serial.println("\r\nWiFi Diag: ");
    WiFi.printDiag(Serial);
    return;
  }

  Serial.println("WiFi connected.");

  // Local IP Address
  Serial.print("\r\n IP Address: "); 
  Serial.println(WiFi.localIP());

  // Show MAC Address
  String macStr = WiFi.macAddress();
  Serial.print("MAC Address: "); Serial.println(macStr);
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
  DHT_Humi = h;
  DHT_Temp = t;

  String outstr = "\r\nHumidity: " + String(DHT_Humi) + " %, "
                + "Temperature: " + String(DHT_Temp) + " °C";
  Serial.println(outstr);
}

// ----------------------------------------------------------------------------

String prepareHtmlPage()
{
  String htmlPage = 
     String("HTTP/1.1 200 OK\r\n") +
            "Content-Type: text/html\r\n" +
            "Connection: close\r\n" +
            "Refresh: 5\r\n" +
            "\r\n" +
            "<!DOCTYPE HTML>" +
            "<html><head>" +
            "  <meta charset=\"utf-8\">" +
            "  <title>ESP8266 with DHT Sensor</title>" +
            "</head>" +
            "<body>" +
            "  <p>Humidity: " + String(DHT_Humi) + " %</p>" +
            "  <p>Temperature:  " + String(DHT_Temp) + " &deg;C</p>" +
            "</body></html>"
            "\r\n";
  return htmlPage;
}

// ----------------------------------------------------------------------------

void sendToWebServer()
{
  WiFiClient client = server.available();
  if (!client) return;

  Serial.println("\r\n[Client connected]");
  while (client.connected())
  {
    // read line by line what the client (web browser) is requesting
    if (client.available())
    {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      // wait for end of client's request, that is marked with an empty line
      if (line.length() == 1 && line[0] == '\n')
      {
        client.println(prepareHtmlPage());
        break;
      }
    }
  }
  delay(1); // give the web browser time to receive the data

  // close the connection:
  client.stop();
  Serial.println("[Client disonnected]");
}

// ----------------------------------------------------------------------------

void sendToThingSpeak()
{
  WiFiClient client = server.available();
  if (!client.connect(TS_Host, 80)) return;

  Serial.println("\r\n[ThingSpeak connected]");

  String url = "GET /update?key=";
  url += WriteAPIKey;
  url += "&field1=" + String(DHT_Humi);
  url += "&field2=" + String(DHT_Temp);
  url += "\r\n";

  // This will send the request to the server
  String sendStr = url 
                 + " HTTP/1.1\r\n" 
                 + "Host: " + TS_Host + "\r\n" 
                 + "Connection: close";
  client.print(sendStr + "\r\n");
  Serial.println(sendStr);

  Serial.println("[ThingSpeak disconnected]");
}
