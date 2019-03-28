#include <SoftwareSerial.h>

SoftwareSerial ESP(3, 2); // ESP8266 ESP-01: Tx, Rx
 
void setup() {
    Serial.begin(115200);
    ESP.begin(115200);
    delay(3000);
    Serial.println("========START AT CMD========");
}

void loop() {
    // send AT command to ESP-01 form console (serial)
    // !NOTE! console must include \r\n at the end of your command line
    while ( Serial.available() ) {
        ESP.write( Serial.read() );
    }
    // AT+GMR     : ESP-01 will return firmware version
    // AT+CWMODE? : ESP-01 will return the configuration mode
    // AT+CIFSR   : ESP-01 will return the IP
    while ( ESP.available() ) { // receive message from ESP-01
        char ch = ESP.read();
        Serial.write( ch );
    }
}
