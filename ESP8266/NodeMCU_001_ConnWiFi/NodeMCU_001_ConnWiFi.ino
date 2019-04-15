#include <ESP8266WiFi.h>

// Replace these with your WiFi network settings
const char* WiFi_SSID = "DADA9631";
const char* WiFi_PSWD = "22453975";

#define BAUD_SER  9600

void setup()
{
  Serial.begin(BAUD_SER);
 
  WiFi.begin(WiFi_SSID, WiFi_PSWD);

  Serial.println();
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("success!");
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());
}

void loop() {
}
