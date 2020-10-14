//下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>
//水肥/缺素状态
//bool water_miss_status=0;
//苗床/墙植状态
bool seedbed_wall_status=0;
//苗床继电器状态
bool seedbed_relay_status=0;
//水肥继电器状态
bool water_relay_status=0;
//墙植继电器状态
bool wall_relay_status=0;
//缺素继电器状态
bool miss_relay_status=0;
//mac地址为01
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 }; //自定义mac地址，不会冲突即可
//ip地址
IPAddress ip(192,168,1,227); //使用DHCPAddressPrinter扫描出

//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1Xl1RECWXS"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Arduino_seclect_test"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "7575050b9a7f61da309c5d29c063bde0"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;

void setup()
{
Ethernet.begin(mac,ip);  //初始化以太网

//初始化 iot，需传入实例化的的ethclient，和设备产品信息
AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

//绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数
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

//初始化
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

//上传数据
AliyunIoTSDK::send("seedbed_relay",seedbed_relay_status);  
AliyunIoTSDK::send("wall_relay",wall_relay_status);
AliyunIoTSDK::send("water_relay",water_relay_status);  
AliyunIoTSDK::send("miss_relay",miss_relay_status);    
}
}

//水肥-苗床
void water_seedbed(JsonVariant L)
{
int water_seedbed = L["water_seedbed"];//参数l
//收到回传信号
if (water_seedbed == 0)
{
//1010
digitalWrite(22,LOW);
digitalWrite(23,HIGH);
digitalWrite(24,LOW);
digitalWrite(25,HIGH);
//继电器状态
water_relay_status=1;
miss_relay_status=0;
seedbed_relay_status=1;
wall_relay_status=0;
}
}

//水肥-墙植
void water_wall(JsonVariant L)
{
int water_wall = L["water_wall"];//参数l
//收到回传信号
if (water_wall == 0)
{
//1001
digitalWrite(22,LOW);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
digitalWrite(25,LOW);
//继电器状态
water_relay_status=1;
miss_relay_status=0;
seedbed_relay_status=0;
wall_relay_status=1;
}
}

//缺素-苗床
void miss_seedbed(JsonVariant L)
{
int water_wall = L["water_wall"];//参数l
//收到回传信号
if (water_wall == 0)
{
//0110
digitalWrite(22,HIGH);
digitalWrite(23,LOW);
digitalWrite(24,LOW);
digitalWrite(25,HIGH);
//继电器状态
water_relay_status=0;
miss_relay_status=1;
seedbed_relay_status=1;
wall_relay_status=0;
}
}

//缺素-墙植
void miss_wall(JsonVariant L)
{
int miss_wall = L["miss_wall"];//参数l
//收到回传信号
if (miss_wall == 0)
{
//0101
digitalWrite(22,HIGH);
digitalWrite(23,LOW);
digitalWrite(24,HIGH);
digitalWrite(25,LOW);
//继电器状态
water_relay_status=0;
miss_relay_status=1;
seedbed_relay_status=0;
wall_relay_status=1;
}
}

//继电器复位
void rs(JsonVariant L)
{
int rs = L["rs"];//参数l
//收到回传信号
if (rs == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
digitalWrite(25,HIGH);
//继电器状态
water_relay_status=0;
miss_relay_status=0;
seedbed_relay_status=0;
wall_relay_status=0;
}
}
