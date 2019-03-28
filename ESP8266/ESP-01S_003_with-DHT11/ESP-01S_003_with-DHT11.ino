#include <SoftwareSerial.h>
#include "DHT.h" // package required from https://github.com/adafruit/DHT-sensor-library

#define BAUD      115200
#define PIN_DHT   5
#define PIN_RX    3     // ESP8266_TX->RX(D3)
#define PIN_TX    2     // ESP8266_RX->TX(D2)
#define DELAY     2000  // ms

DHT dht(PIN_DHT, DHT11);            // digital pin5 connected to the DHT11
SoftwareSerial ESP(PIN_RX, PIN_TX); // ESP8266 ESP-01: Tx, Rx

void setup() {
    Serial.begin(BAUD);
    dht.begin();
    delay(DELAY); // Sensor readings may be up to 2 seconds
    Serial.println("DHT11 start ...");

    ESP.begin(BAUD);
    Serial.println("\nInitialize ESP-01 ...");
    sendATcmd("AT+RST\r\n", 3000);            // reset ESP-01
    sendATcmd("AT+CWMODE=2\r\n", 2000);       // config ESP-01 as AP mode
    sendATcmd("AT+CWMODE?\r\n", 1000);        // check AP mode
    sendATcmd("AT+GMR\r\n", 1000);            // check firmware version
    sendATcmd("AT+CIFSR\r\n", 2000);          // check IP
    sendATcmd("AT+CIPMUX=1\r\n", 2000);       // allow multiple access
    sendATcmd("AT+CIPSERVER=1,80\r\n",2000);  // start server at port:80
    Serial.println("\nServer started at port 80 ...");
}

void setDelay(unsigned int delay) {
    unsigned long timeout = delay+millis();
    while( millis()<timeout ) {} // NOP
}

void sendATcmd(char* cmd, unsigned int delay) {
    ESP.print( cmd ); // send AT command to ESP-01
    setDelay(delay);
}

void sendHTML(byte connID,char* msg) {
    extern int connCount; 
    String html;
    char cipSend[128];
    char cipClose[128];
    float h = dht.readHumidity();   // Reading temperature or humidity takes about 250 milliseconds
    float t = dht.readTemperature(); // Read temperature as Celsius (the default)
   
    html += "<html><head>";
    html += "<meta http-equiv=\"refresh\" content=\"10\">";
    html +="<title>From ESP-01</title></head><body>\n\r";
    html += "<p>ClientMsg: ";
    html += msg;
    html += "</p>\n\r";
    html += "<p>Humidity:";
    html += h;
    html += "%</p>\n\r";
    html += "<p>Temperature:";
    html += t;
    html += "*C</p>\n\r";
    html += "</body></html>";
    Serial.println(html);
    sprintf(cipSend,"AT+CIPSEND=%d,%d\r\n",connID,html.length());
    sprintf(cipClose,"AT+CIPCLOSE=%d\r\n",connID);
    sendATcmd(cipSend,1000);
    sendATcmd(html.c_str(),1000);
    sendATcmd(cipClose,1000);
}

void loop() {
    // send AT command to ESP-01 form console (serial)
    if ( Serial.available() ) {
        ESP.write( Serial.read() );
    }
    if ( ESP.available() ) { // receive message from ESP-01
        if ( ESP.find("+IPD,") ) { // detect the client's request
            String msg="";
            byte connID = ESP.read()-48; // client's connection ID
            while( ESP.available() ) { // collect client's request from the web browser
                msg += (char)ESP.read();
                delay(20); // the delay will let the message become more stable
            }
            sendHTML(connID,msg.c_str()); // send HTML message to client
            Serial.flush();
        }
    }
}
