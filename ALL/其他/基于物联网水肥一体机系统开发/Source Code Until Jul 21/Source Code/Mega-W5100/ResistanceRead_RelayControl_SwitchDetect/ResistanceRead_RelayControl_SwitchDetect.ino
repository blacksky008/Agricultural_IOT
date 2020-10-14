#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>
//需要安装Crypto库、PubSubClient库、AliyunIoTSDk库

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,3,254); //使用DHCPAddressPrinter扫描出

//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a12cewNBLYZ"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Mega-W5100"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "86e8296b85830ee659e68066ae7a01f3"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;
bool Relay_status = 0;
int Relay_pin = 22;

void setup()
{
  Serial.begin(9600);
  pinMode(Relay_pin, OUTPUT);
  pinMode(24, INPUT);
  
  Ethernet.begin(mac,ip);  //初始化以太网

  //初始化 iot，需传入实例化的的ethclient，和设备产品信息
  AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

  //绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数
  AliyunIoTSDK::bindData("Relay_value",Relay);
  

  
}

void loop()
{
  
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 10) //>=1则为0.1秒，10为1秒，下同
  {
    lastMsMain = millis();

    //发送电阻状态到云平台（高电平：1；低电平：0）
    AliyunIoTSDK::send("Ohm_value",  analogRead(A0));
    // Serial.println(analogRead(A0));

    //发送继电器状态和限位开关到云平台（高电平：1；低电平：0） 
    AliyunIoTSDK::send("Relay_value",Relay_status);
    AliyunIoTSDK::send("Switch_value",digitalRead(24)); 

    //检测限位开关是否到位，如到位则关停继电器，并更新继电器状态
    if(!((bool)digitalRead(24))){
     digitalWrite(Relay_pin, LOW); 
     Relay_status = 0;
    }
  }

  
}



void Relay(JsonVariant L) //调整Relay状态的函数
{
  int Relay = L["Relay_value"];//框中填入Aliyun传回的属性值，并把该值赋到本地变量上
  
  if (Relay == 1 && (bool)digitalRead(24))
  {
    digitalWrite(Relay_pin, HIGH);
    Relay_status = 1;
  }
  else 
  {
    digitalWrite(Relay_pin, LOW);   
    Relay_status = 0;
  }
    
  
}
