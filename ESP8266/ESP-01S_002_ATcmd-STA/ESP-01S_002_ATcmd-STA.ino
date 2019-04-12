/*  
 *  AT Command                說明
 *  ---------------------------------------------------------------------
 *  AT                        OK
 *  AT+GMR                    Version
 *  AT+RST                    硬體重置
 *  AT+CWMODE?                WiFi 組態查詢
 *  AT+CWMODE={1,2 or 3}      WiFi 組態設定 (1:STA, 2:AP, 3:Both)
 *  AT+CWLAP                  讓模組列出當前環境下存在無線路由器的列表
 *  AT+CWJAP="ID","password"  讓模組連上自己的路由器
 *  AT+CWJAP?                 檢測模組是否連上自己的路由器
 *  AT+CIPMUX=1               啟動模組允許多重連線
 *  AT+CIPSERVER=1,80         開啟模組本地的TCP 伺服器(1:Open, 0:Close)
 *  AT+CIFSR=?                檢查模組的IP位址
 */

#include <SoftwareSerial.h>
#define BAUD      9600

SoftwareSerial mySerial(3, 2); // mySerial8266 mySerial-01: Tx, Rx
 
void setup() {
  Serial.begin(BAUD);
  Serial.println("Arduino...Ok");

  mySerial.begin(BAUD);
  Serial.println("ESP8266...Ok");

  Serial.println("\r\nInitialize ESP-01S ...");
  initESP();
  Serial.println("\r\nESP8266 ready ...");

}

void loop() {
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}

void initESP() {
//  sendATcmd("AT+GMR\r\n",      1000);
  sendATcmd("AT+RST\r\n",      5000);
  sendATcmd("AT+CWMODE=1\r\n", 2000);
  sendATcmd("AT+CWMODE?\r\n",  1000);
  sendATcmd("AT+CWJAP=\"ESP8266\",\"1234test\"\r\n", 5000);
  sendATcmd("AT+CIFSR\r\n",    1000);
}

void sendATcmd(char *cmd, unsigned int delay)
{
  mySerial.print(cmd);
  unsigned long timeout = millis() + delay;
  while(millis() < timeout ) {} // NOP

  Serial.println();
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  };
}
