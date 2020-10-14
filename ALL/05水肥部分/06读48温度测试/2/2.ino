//Aliyun通讯部分背景设置开始-------------------------------------------

  //下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,3,254); //使用DHCPAddressPrinter扫描出

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

int ec_01;
int ec_00;

// instantiate ModbusMaster object
ModbusMaster node1;  //Read Temperature & Humidity, ID 1
ModbusMaster node2;  //Read Light Intensity, ID 2
ModbusMaster node3;  //Read CO2 Concentration, ID 3
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
  node1.begin(1, Serial1);  
  node2.begin(2, Serial1);  
  node3.begin(3, Serial1);  
  node5.begin(5,Serial1);
  Serial.begin(9600);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node1.preTransmission(preTransmission);
  node1.postTransmission(postTransmission);
  node2.preTransmission(preTransmission);
  node2.postTransmission(postTransmission);
  node3.preTransmission(preTransmission);
  node3.postTransmission(postTransmission);
  node5.preTransmission(preTransmission);
  node5.postTransmission(postTransmission);
}

void loop()
{
  
  //node 1 Code Begin:
  
  uint8_t result1;    
  
  result1 = node1.readHoldingRegisters(0x0020, 2); 
  
  if (result1 == node1.ku8MBSuccess)
  {
    Serial.println("Communication With Node 1 Success");
    temperatureValue = (node1.getResponseBuffer(0x00) / 10.0);
    humidityValue = (node1.getResponseBuffer(0x01)/10.0); //
    Serial.print(temperatureValue);
    Serial.println(" celsius");
    Serial.print(humidityValue);
    Serial.println(" %");
   
  }
  else{
    Serial.println("Communication With Node 1 Failed");
  }

  //node 1 Code End
  
  delay(100);

  uint8_t result5;   
result5 = node5.readHoldingRegisters(0x0000, 2); 
  
  if (result5 == node5.ku8MBSuccess)
  {
    Serial.println("Communication With Node 5 Success");
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
Serial.println(y,2);



delay(2000);
//String x = hex;
//float y = *(float*)&x;
//Serial.println(y,6);
//delay(1000);

  }
  else{
    Serial.println("Communication With Node 5 Failed");
  }


  
  
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 10) //>=1则为0.1秒，10为1秒，下同
  {
    lastMsMain = millis();

    
  }
  delay(2000);

  
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
