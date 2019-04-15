#include <ESP8266WiFi.h>

// Replace these with your WiFi network settings
const char* WiFi_SSID = "DADA9631";
const char* WiFi_PSWD = "22453975";

#define BAUD_ESP 115200

void setup() 
{
  Serial.begin(BAUD_ESP);

  Serial.println();
  Serial.print("Configuring WiFi access point...");
  
  /* You can remove the password parameter if you want the AP to be open. */
  boolean result = WiFi.softAP(WiFi_SSID, WiFi_PSWD);
  if(result == true) 
  {
    IPAddress myIP = WiFi.softAPIP();
  
    Serial.println("done!");
    Serial.println("");
    Serial.print("WiFi network name: ");
    Serial.println(WiFi_SSID);
    Serial.print("WiFi network password: ");
    Serial.println(WiFi_PSWD);
    Serial.print("Host IP Address: ");
    Serial.println(myIP);
    Serial.println("");
  }
  else {
    Serial.println("error! Something went wrong...");
  }
}

void loop() {
  Serial.printf("Number of connected devices (stations) = %d\n", WiFi.softAPgetStationNum());
  delay(3000);
}
