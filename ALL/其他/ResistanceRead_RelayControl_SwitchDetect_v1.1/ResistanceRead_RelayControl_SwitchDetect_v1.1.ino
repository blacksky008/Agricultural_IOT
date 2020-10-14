#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>
//需要安装Crypto库、PubSubClient库、AliyunIoTSDk库

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,3,254); //使用DHCPAddressPrinter扫描出

//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1VJEioFKJM"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Mega-W5100"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "0677e8f806d3cd86a5fd10fbe8e232d3"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;
bool relayStatus = 0;
int relayPin = 22;

void setup()
{
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  pinMode(24, INPUT);
  
  Ethernet.begin(mac,ip);  //初始化以太网

  //初始化 iot，需传入实例化的的ethclient，和设备产品信息
  AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

  //绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数
  AliyunIoTSDK::bindData("relayValue",Relay);
  

  
}

void loop()
{
  
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 1000) //>=1则为0.1秒，10为1秒，下同
  {
    lastMsMain = millis();

    //发送电阻状态到云平台（高电平：1；低电平：0）
    AliyunIoTSDK::send("ohmValue",  analogRead(A0));
    // Serial.println(analogRead(A0));

    //发送继电器状态和限位开关到云平台（高电平：1；低电平：0） 
    AliyunIoTSDK::send("relayValue",relayStatus);
    AliyunIoTSDK::send("switchValue",digitalRead(24)); 

    //检测限位开关是否到位，如到位则关停继电器，并更新继电器状态
    if(!((bool)digitalRead(24))){
     digitalWrite(relayPin, LOW); 
     relayStatus = 0;
    }
  }

  
}



void Relay(JsonVariant L) //调整Relay状态的函数
{
  int Relay = L["relayValue"];//框中填入Aliyun传回的属性值，并把该值赋到本地变量上
  
  if (Relay == 1 && (bool)digitalRead(24))
  {
    digitalWrite(relayPin, HIGH);
    relayStatus = 1;
  }
  else 
  {
    digitalWrite(relayPin, LOW);   
    relayStatus = 0;
  }
    
  
}
