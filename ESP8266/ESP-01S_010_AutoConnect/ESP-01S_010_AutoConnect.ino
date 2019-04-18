#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


void setup() 
{
  Serial.begin(115200);

  //WiFiManager
  WiFiManager wifiManager;

  //reset saved settings
  //wifiManager.resetSettings();
    
  //set custom ip for portal

  // IPAddress ip(10,0,1,1);
  // IPAddress gateway(10,0,1,1);
  // IPAddress subnet(255.255.255.0);
  // wifiManager.setAPStaticIPConfig(ip, gateway, subnet);

  wifiManager.autoConnect("AutoConnectAP");
   
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
}

void loop() {
    // put your main code here, to run repeatedly:
}
