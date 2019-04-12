#include <SoftwareSerial.h>
#define BAUD_SERIAL   9600
#define BAUD_ESP8266  115200

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
 
SoftwareSerial mySerial(3,2);  // ESP-01S: Tx, Rx

void setup() {
  Serial.begin(BAUD_SERIAL);
  Serial.println("Arduino...OK");
  
  mySerial.begin(BAUD_ESP8266);
  Serial.println("ESP8266...OK");
}

void loop() {
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
    mySerial.write(Serial.read());
  }
}
