//Aliyun通讯部分背景设置开始-------------------------------------------

  //下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x00 }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,3,26); //使用DHCPAddressPrinter扫描出
//
  //设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1ZY2wCfbv3"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Arduino_mixmotor"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "69a8469c0c48bcebeb692f22fbda00e8"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai
//
EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;

//Aliyun通讯部分背景设置结束-------------------------------------------

bool motor1_status = 0; //为0则停止运动，为1则开始运动
bool motor2_status = 0; //为0则停止运动，为1则开始运动
bool motor3_status = 0; //为0则停止运动，为1则开始运动


void setup()
{
  
  
  Ethernet.begin(mac,ip);  //初始化以太网

  //初始化 iot，需传入实例化的的ethclient，和设备产品信息
  AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

  //绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数
  AliyunIoTSDK::bindData("motor1",motor1);
  AliyunIoTSDK::bindData("motor2",motor2);
  AliyunIoTSDK::bindData("motor3",motor3);
  
//电机1
pinMode(22,OUTPUT);
//电机2
pinMode(23,OUTPUT);
//电机3
pinMode(24,OUTPUT);
}

void loop()
{
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 1000) 
  {
    lastMsMain = millis();

    //发送光传感器读数到云端 
    AliyunIoTSDK::send("motor1",motor1_status);
    AliyunIoTSDK::send("motor2",motor2_status);
    AliyunIoTSDK::send("motor3",motor3_status);  
  }
}

void motor1(JsonVariant L)
{
 int motor1 = L["motor1"];//参数l
  if (motor1 == 1)
  {
    digitalWrite(22,HIGH);
    motor1_status=1;
  }
  else 
  {
    digitalWrite(22, LOW);
    motor1_status=0;
  }
}

void motor2(JsonVariant L)
{
 int motor2 = L["motor2"];//参数l
  if (motor2 == 1)
  {
    digitalWrite(23,HIGH);
    motor2_status=1;
  }
  else 
  {
    digitalWrite(23, LOW);
    motor2_status=0;
  }
}

void motor3(JsonVariant L)
{
 int motor3 = L["motor3"];//参数l
  if (motor3 == 1)
  {
    digitalWrite(24,HIGH);
    motor3_status=1;
  }
  else 
  {
    digitalWrite(24, LOW);
    motor3_status=0;
  }
}
