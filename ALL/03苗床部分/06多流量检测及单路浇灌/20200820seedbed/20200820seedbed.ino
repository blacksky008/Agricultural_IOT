//中断定义
#define PIN0 2
#define PIN2 21
#define PIN3 20

//计数
volatile long count_0 =0;
volatile long count_2 =0;
volatile long count_3 =0;

long temp_0 = 0;
long temp_2 = 0;
long temp_3 = 0;

double flow_0 = 0;
double flow_2 = 0;
double flow_3 = 0;

double flowrate_0 = 0;
double flowrate_2 = 0;
double flowrate_3 = 0;

//下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>


bool relay_status_0=0;
bool relay_status_2=0;
bool relay_status_3=0;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,3,26); //使用DHCPAddressPrinter扫描出
//
//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1WquA1UMew"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Arduino_seedbed"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "ea45b3064b28ead54ccf0ec6a763592c"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai
//
EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;



void setup()
{
  Serial.begin(9600);
  
pinMode(PIN0,INPUT);
pinMode(PIN2,INPUT);
pinMode(PIN3,INPUT);

//管路1
pinMode(22,OUTPUT);
//管路2
pinMode(23,OUTPUT);
//管路3
pinMode(24,OUTPUT);

attachInterrupt(0,blink_0,FALLING);
attachInterrupt(2,blink_2,FALLING);
attachInterrupt(3,blink_3,FALLING);
  
Ethernet.begin(mac,ip);  //初始化以太网

//初始化 iot，需传入实例化的的ethclient，和设备产品信息
AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);


digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
relay_status_0=0;
relay_status_2=0;
relay_status_3=0;

//绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数


AliyunIoTSDK::bindData("relay_status_0",relay_status0);
AliyunIoTSDK::bindData("relay_status_2",relay_status2);
AliyunIoTSDK::bindData("relay_status_3",relay_status3);
AliyunIoTSDK::bindData("rs",rs);
}

void loop()
{
AliyunIoTSDK::loop();//必要函数

if (millis() - lastMsMain >= 1000) 
{
lastMsMain = millis();


AliyunIoTSDK::send("flowrate_0",flowrate_0);  
AliyunIoTSDK::send("flow_0",flow_0);
AliyunIoTSDK::send("flowrate_2",flowrate_2);  
AliyunIoTSDK::send("flow_2",flow_2);
AliyunIoTSDK::send("flowrate_3",flowrate_3);  
AliyunIoTSDK::send("flow_3",flow_3);  

AliyunIoTSDK::send("relay_status_0",relay_status_0); 
AliyunIoTSDK::send("relay_status_2",relay_status_2); 
AliyunIoTSDK::send("relay_status_3",relay_status_3); 

cal_0();
cal_2();
cal_3();


if(flow_0 >= 5)
{
  digitalWrite(22,HIGH);
  relay_status_0=1;
}

if(flow_2 >= 5)
{
  digitalWrite(23,HIGH);
  relay_status_2=1;
}

if(flow_3 >= 5)
{
  digitalWrite(24,HIGH);
  relay_status_3=1;
}




}
}

void blink_0()
{
  count_0++;
}

void blink_2()
{
  count_2++;
}

void blink_3()
{
  count_3++;
}


void cal_0()
{
flowrate_0 = (count_0-temp_0)/0.45;
flowrate_0 = flowrate_0/60.00;
temp_0 = count_0;
flow_0=flow_0+flowrate_0;
}

void cal_2()
{
flowrate_2 = (count_2-temp_2)/0.45;
flowrate_2 = flowrate_2/60.00;
temp_2 = count_2;
flow_2=flow_2+flowrate_2;
//delay(100);
}

void cal_3()
{
flowrate_3 = (count_3-temp_3)/0.45;
flowrate_3 = flowrate_3/60.00;
temp_3 = count_3;
flow_3=flow_3+flowrate_3;
//delay(100);
}

void relay_status0(JsonVariant L)
{
 int relay_status_0 = L["relay_status_0"];//参数l
if (relay_status_0 == 0)
{
digitalWrite(22,LOW);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
//relay_status_0=1;
//relay_status_2=0;
//relay_status_3=0;
}
}

void relay_status2(JsonVariant L)
{
 int relay_status_2 = L["relay_status_2"];//参数l
if (relay_status_2 == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,LOW);
digitalWrite(24,HIGH);
//relay_status_0=0;
//relay_status_2=1;
//relay_status_3=0;
}
}


void relay_status3(JsonVariant L)
{
 int relay_status_3 = L["relay_status_3"];//参数l
if (relay_status_3 == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,LOW);
//relay_status_0=0;
//relay_status_2=0;
//relay_status_3=1;
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
//relay_status_0=0;
//relay_status_2=0;
//relay_status_3=0;
flow_0=0;
flow_2=0;
flow_3=0;
}
}
