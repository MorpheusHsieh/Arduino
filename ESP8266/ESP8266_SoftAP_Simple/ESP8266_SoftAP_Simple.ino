#include <ESP8266WiFi.h>

// Replace these with your WiFi network settings
const char* AP_SSID = "ESP8266";
const char* AP_PSWD = "1234test";

#define BAUD_MNT 9600

void setup() 
{
  Serial.begin(BAUD_MNT);

  Serial.println();
  Serial.print("Configuring WiFi access point...");
  
  /* You can remove the password parameter if you want the AP to be open. */
  boolean result = WiFi.softAP(AP_SSID, AP_PSWD);
  if(result == true) 
  {
    Serial.println("done!");
    Serial.println("");
    Serial.print("WiFi network name: ");
    Serial.println(AP_SSID);
    Serial.print("WiFi network password: ");
    Serial.println(AP_PSWD);

    Serial.print("Host IP Address: ");
    IPAddress myIP = WiFi.softAPIP();
    Serial.println(myIP);

    Serial.println("");
  }
  else {
    Serial.println("error! Something went wrong...");
  }
}

void loop() {
  int num_dev = WiFi.softAPgetStationNum();
  Serial.printf("Number of connected devices (stations) = %d\n", num_dev);
  delay(3000);
}
