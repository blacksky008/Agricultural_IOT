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

// instantiate ModbusMaster object
ModbusMaster node1;  //Read Temperature & Humidity, ID 1
ModbusMaster node2;  //Read Light Intensity, ID 2
ModbusMaster node3;  //Read CO2 Concentration, ID 3
       

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
  Serial.begin(9600);
  // Callbacks allow us to configure the RS485 transceiver correctly
  node1.preTransmission(preTransmission);
  node1.postTransmission(postTransmission);
  node2.preTransmission(preTransmission);
  node2.postTransmission(postTransmission);
  node3.preTransmission(preTransmission);
  node3.postTransmission(postTransmission);
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
  //node 2 Code Begin:
  
  uint8_t result2;    
  

  // Read 2 registers starting at 0x0020
  result2 = node2.readHoldingRegisters(0x0007, 2); 
  
  if (result2 == node2.ku8MBSuccess)
  {
    Serial.println("Communication With Node 2 Success");
    lightValue= ((node2.getResponseBuffer(0x00) << 8) |node2.getResponseBuffer(0x01)); //
    Serial.print(lightValue);
    Serial.println(" lux");
   
  }
  else{
    Serial.println("Communication With Node 2 Failed");
    Serial.println(result2,HEX);
  }

  //node 2 Code End

 delay(100);
  //node 3 Code Begin:
  
  uint8_t result3;    
  

  // Read 2 registers starting at 0x0020
  result3 = node3.readHoldingRegisters(0x0002, 1); 
  
  if (result3 == node3.ku8MBSuccess)
  {
    Serial.println("Communication With Node 3 Success");
    co2Value = node3.getResponseBuffer(0x00); //
    Serial.print(co2Value);
    Serial.println(" ppm");
    
  }
  else{
    Serial.println("Communication With Node 3 Failed");
  }

  //node 3 Code End

  
  
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 10) //>=1则为0.1秒，10为1秒，下同
  {
    lastMsMain = millis();

    

    //发送光传感器读数到云端 
    AliyunIoTSDK::send("lightValue",lightValue);
    AliyunIoTSDK::send("temperatureValue",temperatureValue);
    AliyunIoTSDK::send("humidityValue",humidityValue);
    AliyunIoTSDK::send("co2Value",co2Value);
    
    

    
  }
  delay(2000);

  
}
