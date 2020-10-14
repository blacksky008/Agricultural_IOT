//Aliyun通讯部分背景设置开始-------------------------------------------

//下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>

bool water_miss_status=0;
bool seedbed_wall_status=0;


bool seedbed_relay_status=0;
bool water_relay_status=0;
bool wall_relay_status=0;
bool miss_relay_status=0;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,3,27); //使用DHCPAddressPrinter扫描出
//
//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1h8DKeMkVi"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Arduino_select"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "66e43cbc7af02c82525a227ebfe0f30d"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai
//
EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;



void setup()
{
Ethernet.begin(mac,ip);  //初始化以太网

//初始化 iot，需传入实例化的的ethclient，和设备产品信息
AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

//绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数
//  AliyunIoTSDK::bindData("water_miss",water_miss);
//  AliyunIoTSDK::bindData("seedbed_wall",seedbed_wall);

AliyunIoTSDK::bindData("water_seedbed",water_seedbed);
AliyunIoTSDK::bindData("water_wall",water_wall);
AliyunIoTSDK::bindData("miss_seedbed",miss_seedbed);
AliyunIoTSDK::bindData("miss_wall",miss_wall);
AliyunIoTSDK::bindData("rs",rs);

//水肥一体
pinMode(22,OUTPUT);
//缺素
pinMode(23,OUTPUT);
//苗床
pinMode(24,OUTPUT);
//墙植
pinMode(25,OUTPUT);

digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
digitalWrite(25,HIGH);
}

void loop()
{
AliyunIoTSDK::loop();//必要函数

if (millis() - lastMsMain >= 1000) 
{
lastMsMain = millis();


AliyunIoTSDK::send("seedbed_relay",seedbed_relay_status);  
AliyunIoTSDK::send("wall_relay",wall_relay_status);
AliyunIoTSDK::send("water_relay",water_relay_status);  
AliyunIoTSDK::send("miss_relay",miss_relay_status);    
}
}



void water_seedbed(JsonVariant L)
{
int water_seedbed = L["water_seedbed"];//参数l
if (water_seedbed == 0)
{
digitalWrite(22,LOW);
digitalWrite(23,HIGH);
digitalWrite(24,LOW);
digitalWrite(25,HIGH);
water_relay_status=1;
miss_relay_status=0;
seedbed_relay_status=1;
wall_relay_status=0;
}
}

void water_wall(JsonVariant L)
{
int water_wall = L["water_wall"];//参数l
if (water_wall == 0)
{
digitalWrite(22,LOW);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
digitalWrite(25,LOW);
water_relay_status=1;
miss_relay_status=0;
seedbed_relay_status=0;
wall_relay_status=1;
}
}



void miss_seedbed(JsonVariant L)
{
int water_wall = L["water_wall"];//参数l
if (water_wall == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,LOW);
digitalWrite(24,LOW);
digitalWrite(25,HIGH);

water_relay_status=0;
miss_relay_status=1;
seedbed_relay_status=1;
wall_relay_status=0;
}
}



void miss_wall(JsonVariant L)
{
int miss_wall = L["miss_wall"];//参数l
if (miss_wall == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,LOW);
digitalWrite(24,HIGH);
digitalWrite(25,LOW);

water_relay_status=0;
miss_relay_status=1;
seedbed_relay_status=0;
wall_relay_status=1;
}
}


void rs(JsonVariant L)
{
int rs = L["rs"];//参数l
if (rs == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
digitalWrite(25,HIGH);
water_relay_status=0;
miss_relay_status=0;
seedbed_relay_status=0;
wall_relay_status=0;
}
}

