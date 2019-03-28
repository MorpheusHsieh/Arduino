#include <Wire.h> // I2C library, gyroscope

// Accelerometer ADXL345
#define ACC (0xA7>>1)     // ADXL345 ACC address
#define A_TO_READ (6)     // num of bytes we are going to read each time 
                          //(two bytes for each axis)

void initAcc() 
{
  // Turning on the ADXL345
  // POWER_CTL: 0x2D
  // D3 設為 1 表示 Measure 模式
  writeTo(ACC, 0x2D, 1<<3);

  // DATA_FORMAT: 0x31
  // D4 為 1時，裝置處於全解析模式，
  // 輸出的解析度隨著 range bit (D1 D0) 的設定而增加 g 值，
  // 以維持 4 mg/LSB 的輸出。
  writeTo(ACC, 0x31, 0x0B); 

  // BW_RATE: 0x2C
  // D3 D2 D1 D0 設為 1001 表示輸出速率 50 Hz, 頻寬 25 Hz
  // 預設值為 0x0A，也就是輸出速率 100 Hz, 頻寬 50 Hz
  writeTo(ACC, 0x2C, 0x09);
}

/*
 * 0x32: DATAX0
 * 0x33: DATAX1
 * 0x34: DATAY0
 * 0x35: DATAY1
 * 0x36: DATAZ0
 * 0x37: DATAZ1
 */
void getAccelerometerData(int * result) 
{
  //first axis-acceleration-data register on the ADXL345
  int regAddress = 0x32; // DATAX0
  
  byte buff[A_TO_READ];

  //read the acceleration data from the ADXL345
  readFrom(ACC, regAddress, A_TO_READ, buff); 
  
  // each axis reading comes in 10 bit resolution, ie 2 bytes.
  // Least Significat Byte first!!
  // thus we are converting both bytes in to one int
  result[0] = (((int)buff[1]) << 8) | buff[0];   
  result[1] = (((int)buff[3])<< 8)  | buff[2];
  result[2] = (((int)buff[5]) << 8) | buff[4];
}
void setup()
{
  Serial.begin(9600);
  Wire.begin();
  initAcc();
}

void loop()
{
  int hx, hy, hz;
  int acc[3];
  getAccelerometerData(acc);
  hx = acc[0];
  hy = acc[1];
  hz = acc[2];
  Serial.print(" X=");
  Serial.print(hx);
  Serial.print(" Y=");
  Serial.print(hy);
  Serial.print(" Z=");
  Serial.println(hz);
  delay(500); 
}

//---------------- Functions

//Writes val to address register on ACC
void writeTo(int DEVICE, byte address, byte val) 
{
  Wire.beginTransmission(DEVICE);  //start transmission to ACC 
  Wire.write(address);             // send register address
  Wire.write(val);                 // send value to write
  Wire.endTransmission();          //end transmission
}

//reads num bytes starting from address register on ACC in to buff array
void readFrom(int DEVICE, byte address, int num, byte buff[]) 
{
  Wire.beginTransmission(DEVICE); //start transmission to ACC 
  Wire.write(address);            //sends address to read from
  Wire.endTransmission();         //end transmission
  
  Wire.beginTransmission(DEVICE);   //start transmission to ACC
  Wire.requestFrom(DEVICE, num);    // request 6 bytes from ACC
  int i = 0;
  while(Wire.available())     //ACC may send less than requested (abnormal)
  { 
    buff[i] = Wire.read();    // receive a byte
    i++;
  }
  Wire.endTransmission();     //end transmission
}
