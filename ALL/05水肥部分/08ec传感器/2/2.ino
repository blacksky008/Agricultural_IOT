//Aliyun通讯部分背景设置开始-------------------------------------------

  //下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x04 }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,30,33); //使用DHCPAddressPrinter扫描出

  //设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1VJEioFKJM"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Mega-W5100"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "0677e8f806d3cd86a5fd10fbe8e232d3"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;

//Aliyun通讯部分背景设置结束-------------------------------------------


//485通信、Modbus通讯部分背景设置及函数开始-----------------------------------

#include <ModbusMaster.h> //引入ModbusMaster库，版本号2.0.1


/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable and Receiver Enable pins are hooked up as follows:
*/
#define MAX485_DE      3   //此版库要求DE和RE口分开接
#define MAX485_RE_NEG  3
int lightValue;
double temperatureValue;
double humidityValue;
int co2Value;





// instantiate ModbusMaster object
ModbusMaster node1;    
ModbusMaster node5;    

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}


//485通信、Modbus通讯部分背景设置及函数结束-----------------------------------


void setup()
{
  
  
  Ethernet.begin(mac,ip);  //初始化以太网

  //初始化 iot，需传入实例化的的ethclient，和设备产品信息
  AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

  //绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数
  //AliyunIoTSDK::bindData("relayValue",Relay);
  


  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);

  // Modbus communication runs at 9600 baud
  Serial1.begin(9600);      

  // Modbus slave ID 1,2,3
  node1.begin(1,Serial1);
  node5.begin(5,Serial1);
  Serial.begin(9600);
  // Callbacks allow us to configure the RS485 transceiver correctly
    node1.preTransmission(preTransmission);
  node1.postTransmission(postTransmission);
  node5.preTransmission(preTransmission);
  node5.postTransmission(postTransmission);
}

void loop()
{
  
ec();
sal();
tds();
tep();
ph();
  
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 10) //>=1则为0.1秒，10为1秒，下同
  {
    lastMsMain = millis();

    
  }
  delay(2000);

  
}

void ec()
{
  uint8_t result5;   
result5 = node5.readHoldingRegisters(0x0000, 2); 
int ec_01;
int ec_00;
  if (result5 == node5.ku8MBSuccess)
  {
//    Serial.println("Communication With Node 5 Success");
//    ec = ((node5.getResponseBuffer(0x01)<< 8) |node5.getResponseBuffer(0x00)); // 
ec_01 =  node5.getResponseBuffer(0x01);
//        Serial.println(ec_01);
//delay(2000);
ec_00=  node5.getResponseBuffer(0x00);
//Serial.println(ec_00);
//delay(2000);
String ec_01_s = tohex(ec_01);
String ec_00_s = tohex(ec_00);
String hex=String(ec_01_s)+String(ec_00_s);
//Serial.println(hex);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
if(y > 0.01){
Serial.print("ec:");
Serial.println(y,2);
delay(2000);
}

  }
  else{
    Serial.println("Communication With Node 5 Failed");
  }

}


void tds()
{
  uint8_t result5;   
result5 = node5.readHoldingRegisters(0x0006, 2); 
  int tds_01;
int tds_00;
  if (result5 == node5.ku8MBSuccess)
  {
//    Serial.println("Communication With Node 5 Success");
tds_01 =  node5.getResponseBuffer(0x01);
tds_00=  node5.getResponseBuffer(0x00);

String tds_01_s = tohex(tds_01);
String tds_00_s = tohex(tds_00);
String hex=String(tds_01_s)+String(tds_00_s);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
if(y>0.01){
Serial.print("tds:");
Serial.println(y,2);

delay(2000);
}
  }
  else{
    Serial.println("Communication With Node 5 Failed");
  }

}


void tep()
{
  uint8_t result5;   
result5 = node5.readHoldingRegisters(0x0004, 2); 
  int tep_01;
int tep_00;
  if (result5 == node5.ku8MBSuccess)
  {
//    Serial.println("Communication With Node 5 Success");
tep_01 =  node5.getResponseBuffer(0x01);
tep_00=  node5.getResponseBuffer(0x00);

String tep_01_s = tohex(tep_01);
String tep_00_s = tohex(tep_00);
String hex=String(tep_01_s)+String(tep_00_s);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
if(y>0.01){
Serial.print("tep:");
Serial.println(y,2);
delay(2000);
}
  }
  else{
    Serial.println("Communication With Node 5 Failed");
  }

}

void sal()
{
  uint8_t result5;   
result5 = node5.readHoldingRegisters(0x0008, 2); 
  int sal_01;
int sal_00;
  if (result5 == node5.ku8MBSuccess)
  {
//    Serial.println("Communication With Node 5 Success");
sal_01 =  node5.getResponseBuffer(0x01);
sal_00=  node5.getResponseBuffer(0x00);

String sal_01_s = tohex(sal_01);
String sal_00_s = tohex(sal_00);
String hex=String(sal_01_s)+String(sal_00_s);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
if(y>0.01){
Serial.print("sal:");
Serial.println(y,2);
delay(2000);
}
  }
  else{
    Serial.println("Communication With Node 5 Failed");
  }

}

void ph()
{
  uint8_t result1;   
result1 = node1.readHoldingRegisters(0x0001, 2); 
int ph_01;
int ph_00;  
  if (result1 == node1.ku8MBSuccess)
  {
    Serial.println("Communication With Node 1 Success");
ph_01 =  node1.getResponseBuffer(0x01);
//        Serial.println(ec_01);
ph_00=  node1.getResponseBuffer(0x00);
String ph_01_s = tohex(ph_01);
String ph_00_s = tohex(ph_00);
String hex=String(ph_01_s)+String(ph_00_s);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
Serial.println(y,2);

delay(2000);

  }
  else{
    Serial.println("Communication With Node 1  Failed");
  }

}


String tohex(int n) {
  if (n == 0) {
    return "00"; //n为0
  }
  String result = "";
  char _16[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };
  const int radix = 16;
  while (n) {
    int i = n % radix;          // 余数
    result = _16[i] + result;   // 将余数对应的十六进制数字加入结果
    n /= radix;                 // 除以16获得商，最为下一轮的被除数
  }
  if (result.length() < 2) {
    result = '0' + result; //不足两位补零
  }
  return result;
}
