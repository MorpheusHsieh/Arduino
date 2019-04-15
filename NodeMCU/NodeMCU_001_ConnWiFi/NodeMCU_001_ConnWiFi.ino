#include <ESP8266WiFi.h>

// Replace these with your WiFi network settings
const char* WiFi_SSID = "xxxxxxxx";
const char* WiFi_PSWD = "xxxxxxxx";

#define D0  16  // NodeMCU LED_BUILTIN
#define D1  5   // I2C Bus SCL (clock)
#define D2  4   // I2C Bus SDA (data), ESP-12 LED
#define D3  0
#define D4  2   // Same as "LED_BUILTIN", but inverted logic
#define D5  14  // SPI Bus SCK (clock)
#define D6  12  // SPI Bus MISO 
#define D7  13  // SPI Bus MOSI
#define D8  15  // SPI Bus SS (CS)
#define D9  3   // RX0 (Serial console)
#define D10 1   // TX0 (Serial console)

#define BAUD_SER 115200

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  delay(1000);
  Serial.begin(BAUD_SER);
 
  WiFi.begin(WiFi_SSID, WiFi_PSWD);

  Serial.println();
  Serial.print("\r\nWiFi Connecting ...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  digitalWrite(LED_BUILTIN, LOW);

  Serial.println("\r\nWiFi Connected Success!");
  Serial.print("\r\nNodeMCU IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
}
