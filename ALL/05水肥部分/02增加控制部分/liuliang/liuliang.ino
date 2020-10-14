
#define PIN0 2
#define PIN2 21
#define PIN3 20

//计数
volatile long count_a =0;
volatile long count_b =0;
volatile long count_c =0;

long temp_a = 0;
long temp_b = 0;
long temp_c = 0;

double flow_a = 0;
double flow_b = 0;
double flow_c = 0;

double safe_pressure = 0.8;

double flowrate_a = 0;
double flowrate_b = 0;
double flowrate_c = 0;



//下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>


bool relay_a=0;
bool relay_b=0;
bool relay_c=0;
bool motor = 0;

bool safe_relay = 0;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x04 }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,3,30); //使用DHCPAddressPrinter扫描出
//
  //设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1PwBTmpyeZ"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Arduino_fertilizer"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "c811c5c46ce1c65de06d0814e9e10fd6"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai
//
EthernetClient ethClient;  //实例化以太网

#define MAX485_DE      3   //此版库要求DE和RE口分开接
#define MAX485_RE_NEG  3

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

unsigned long lastMsMain = 0;



void setup()
{

pinMode(PIN0,INPUT);
pinMode(PIN2,INPUT);
pinMode(PIN3,INPUT);
  
Serial.begin(9600);

//管路a
pinMode(22,OUTPUT);
//管路b
pinMode(23,OUTPUT);
//管路c
pinMode(24,OUTPUT);
//安全阀
pinMode(25,OUTPUT);
//泵
pinMode(26,OUTPUT);

attachInterrupt(0,blink_a,FALLING);
attachInterrupt(2,blink_b,FALLING);
attachInterrupt(3,blink_c,FALLING);

Ethernet.begin(mac,ip);  //初始化以太网

//初始化 iot，需传入实例化的的ethclient，和设备产品信息
AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);


digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
digitalWrite(25,HIGH);
digitalWrite(26,HIGH);
relay_a=0;
relay_b=0;
relay_c=0;
motor = 0;
safe_relay = 0;

//绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数
AliyunIoTSDK::bindData("rs",rs);
AliyunIoTSDK::bindData("start_up",start_up);
}

void loop()
{
AliyunIoTSDK::loop();//必要函数

if (millis() - lastMsMain >= 1000) 
{
lastMsMain = millis();

safe_pressure=(2.5/1023) * analogRead(A2);

AliyunIoTSDK::send("flowrate_a",flowrate_a);  
AliyunIoTSDK::send("flow_a",flow_a);
AliyunIoTSDK::send("flowrate_b",flowrate_b);  
AliyunIoTSDK::send("flow_b",flow_b);
AliyunIoTSDK::send("flowrate_c",flowrate_c);  
AliyunIoTSDK::send("flow_c",flow_c);  

AliyunIoTSDK::send("relay_a",relay_a); 
AliyunIoTSDK::send("relay_b",relay_b); 
AliyunIoTSDK::send("relay_c",relay_c); 
AliyunIoTSDK::send("motor",motor); 

AliyunIoTSDK::send("safe_relay",safe_relay); 
AliyunIoTSDK::send("safe_pressure", safe_pressure);

cal_a();
cal_b();
cal_c();

if(safe_pressure >= 1)
{
  digitalWrite(25,LOW);
  safe_relay = 1;
}else
{
digitalWrite(25,HIGH);
safe_relay = 0;
}

}
}

void blink_a()
{
count_a++;
}

void blink_b()
{
count_b++;
}

void blink_c()
{
count_c++;
}


void cal_a()
{
flowrate_a = (count_a-temp_a)/0.45;
flowrate_a = flowrate_a/60.00;
temp_a = count_a;
flow_a=flow_a+flowrate_a;
}

void cal_b()
{
flowrate_b = (count_b-temp_b)/0.45;
flowrate_b = flowrate_b/60.00;
temp_b = count_b;
flow_b=flow_b+flowrate_b;
}

void cal_c()
{
flowrate_c = (count_c-temp_c)/0.45;
flowrate_c = flowrate_c/60.00;
temp_c = count_c;
flow_c=flow_c+flowrate_c;
}

void relaya(JsonVariant L)
{
int relay_a = L["relay_a"];//参数l
}

void relayb(JsonVariant L)
{
int relay_b = L["relay_b"];//参数l
}


void relayc(JsonVariant L)
{
int relay_c = L["relay_status_3"];//参数l
}

void rs(JsonVariant L)
{
int rs = L["rs"];//参数l
if (rs == 0)
{
flow_a = 0;
flow_b = 0;
flow_c = 0;
}
}


void start_up(JsonVariant L)
{
  int start_up = L["start_up"];
  if (start_up == 0)
  {
 digitalWrite(22,LOW);
digitalWrite(23,LOW);
digitalWrite(24,LOW);
digitalWrite(26,LOW);
relay_a=1;
relay_b=1;
relay_c=1;
motor = 1;  
  }else
  {
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
digitalWrite(26,HIGH);
relay_a=0;
relay_b=0;
relay_c=0;
motor = 0;
  }
}

