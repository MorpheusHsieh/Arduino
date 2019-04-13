#include "ESP8266WiFi.h"

const char* ssid = "DADA9631";
const char* pswd = "22453975";

void setup(void)
{  
  Serial.begin(9600);

  WiFi.begin(ssid, pswd);
  while (WiFi.status() != WL_CONNECTED) {  //不斷嚐試連接
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected!");  //已連接
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());  //顯示IP位址
}

void loop() {
}
