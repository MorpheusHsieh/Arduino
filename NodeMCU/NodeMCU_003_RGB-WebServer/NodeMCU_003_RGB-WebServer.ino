// Ref: https://www.etechpath.com/how-to-control-rgb-led-wirelessly-using-esp8266-wifi-web-interface/

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#define LED_RED D2
#define LED_GRN D3
#define LED_BLU D4

const char *AP_SSID = "RGB";
const char *AP_PSWD = "12345678";  
 
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);   //IP address of your ESP 
DNSServer dnsServer;
ESP8266WebServer webServer(80);
 
String getWebpage();
void handleRoot();
void testRGB();
void fade(int);

//Webpage html Code
String webpage = getWebpage();

void setup() 
{
  Serial.begin(9600);
  
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GRN, OUTPUT);
  pinMode(LED_BLU, OUTPUT);
  
  analogWrite(LED_RED, 1023);
  analogWrite(LED_GRN, 1023);
  analogWrite(LED_BLU, 1023);
  delay(1000);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  //WiFi.softAP(AP_SSID);
  WiFi.softAP(AP_SSID, AP_PSWD);
  
  dnsServer.start(DNS_PORT, "rgb", apIP);
  webServer.on("/", handleRoot);
  webServer.begin();
  testRGB();
}

void loop() 
{
  dnsServer.processNextRequest();
  webServer.handleClient();
}

String getWebpage()
{
  String html = "";

  html += "<!DOCTYPE html>\r\n";
  html += "<html>\r\n";
  html += "  <head>\r\n";
  html += "    <title>RGB LED Control</title>\r\n";
  html += "    <meta name='mobile-web-app-capable' content='yes' />\r\n";
  html += "    <meta name='viewport' content='width=device-width' />\r\n";
  html += "  </head>\r\n";
  html += "  <body style='margin: 0px; padding: 0px;'>\r\n";
  html += "    <canvas id='colorspace'></canvas>\r\n";
  html += "  </body>\r\n";
  html += "</html>\r\n";
  html += "\r\n";
  html += "<script type='text/javascript'>\r\n";
  html += "  (function () {\r\n";
  html += "    var canvas = document.getElementById('colorspace');\r\n";
  html += "    var ctx = canvas.getContext('2d');\r\n";
  html += "\r\n";
  html += "    function drawCanvas() {\r\n";
  html += "      var colours = ctx.createLinearGradient(0, 0, window.innerWidth, 0);\r\n";
  html += "      for(var i=0; i <= 360; i+=10) {\r\n";
  html += "        colours.addColorStop(i/360, 'hsl(' + i + ', 100%, 50%)');\r\n";
  html += "      }\r\n";
  html += "\r\n";
  html += "      ctx.fillStyle = colours;\r\n";
  html += "      ctx.fillRect(0, 0, window.innerWidth, window.innerHeight);\r\n";
  html += "      var luminance = ctx.createLinearGradient(0, 0, 0, ctx.canvas.height);\r\n";
  html += "      luminance.addColorStop(0, '#ffffff');\r\n";
  html += "      luminance.addColorStop(0.05, '#ffffff');\r\n";
  html += "      luminance.addColorStop(0.5, 'rgba(0,0,0,0)');\r\n";
  html += "      luminance.addColorStop(0.95, '#000000');\r\n";
  html += "      luminance.addColorStop(1, '#000000');\r\n";
  html += "      ctx.fillStyle = luminance;\r\n";
  html += "      ctx.fillRect(0, 0, ctx.canvas.width, ctx.canvas.height);\r\n";
  html += "    }\r\n";
  html += "\r\n";
  html += "    var eventLocked = false;\r\n";
  html += "    function handleEvent(clientX, clientY) {\r\n";
  html += "      if(eventLocked) {\r\n";
  html += "        return;\r\n";
  html += "      }\r\n";
  html += "\r\n";
  html += "      function colourCorrect(v) {\r\n";
  html += "        return Math.round(1023-(v*v)/64);\r\n";
  html += "      }\r\n";
  html += "\r\n";
  html += "      var data = ctx.getImageData(clientX, clientY, 1, 1).data;\r\n";
  html += "      var params = [\r\n";
  html += "        'r=' + colourCorrect(data[0]),\r\n";
  html += "        'g=' + colourCorrect(data[1]),\r\n";
  html += "        'b=' + colourCorrect(data[2])\r\n";
  html += "      ].join('&');\r\n";
  html += "\r\n";
  html += "      var req = new XMLHttpRequest();\r\n";
  html += "      req.open('POST', '?' + params, true);\r\n";
  html += "      req.send();\r\n";
  html += "      eventLocked = true;\r\n";
  html += "\r\n";
  html += "      req.onreadystatechange = function() {\r\n";
  html += "        if(req.readyState == 4) {\r\n";
  html += "          eventLocked = false;\r\n";
  html += "        }\r\n";
  html += "      }\r\n";
  html += "    }\r\n";
  html += "\r\n";
  html += "    canvas.addEventListener('click', function(event) {\r\n";
  html += "      handleEvent(event.clientX, event.clientY, true);\r\n";
  html += "    }, false);\r\n";
  html += "\r\n";
  html += "    canvas.addEventListener('touchmove', function(event){\r\n";
  html += "      handleEvent(event.touches[0].clientX, event.touches[0].clientY);\r\n";
  html += "    }, false);\r\n";
  html += "\r\n";
  html += "    function resizeCanvas() {\r\n";
  html += "      canvas.width = window.innerWidth;\r\n";
  html += "      canvas.height = window.innerHeight;\r\n";
  html += "      drawCanvas();\r\n";
  html += "    }\r\n";
  html += "\r\n";
  html += "    window.addEventListener('resize', resizeCanvas, false);\r\n";
  html += "    resizeCanvas();\r\n";
  html += "    drawCanvas();\r\n";
  html += "    document.ontouchmove = function(e) {e.preventDefault()};\r\n";
  html += "  })();\r\n";
  html += "</script>\r\n";

  return html;
}

void handleRoot() 
{
  // Serial.println("handle root..");
  String red   = webServer.arg(0);  // read RGB arguments
  String green = webServer.arg(1);  // read RGB arguments
  String blue  = webServer.arg(2);  // read RGB arguments
  Serial.println("R: "+String(red) + ", G: " + String(green) + ", B: " + String(blue));
 
  // For common anode
//  int redVal = red.toInt();
//  int grnVal = green.toInt();
//  int bluVal = blue.toInt();

  // For common cathode
  int redVal = 1023 - red.toInt();
  int grnVal = 1023 - green.toInt();
  int bluVal = 1023 - blue.toInt();
  
  analogWrite(LED_RED, redVal);
  analogWrite(LED_GRN, grnVal);
  analogWrite(LED_BLU, bluVal);
  
  webServer.send(200, "text/html", webpage);
}

void testRGB() 
{ 
  // fade in and out of Red, Green, Blue
  analogWrite(LED_RED, 1023); // Red off
  analogWrite(LED_GRN, 1023); // Green off
  analogWrite(LED_BLU, 1023); // Blue off
  
  fade(LED_RED); // Red fade effect
  fade(LED_GRN); // Green fade effect
  fade(LED_BLU); // Blue fade effect
}

void fade(int pin) 
{
  for (int u = 0; u < 1024; u++) {
    analogWrite(pin, 1023 - u);
    delay(1);
  }
  
  for (int u = 0; u < 1024; u++) {
    analogWrite(pin, u);
    delay(1);
  }
}  
