//中断定义
#define PIN0 2
#define PIN2 21
#define PIN3 20

//计数
volatile long count_0 =0;
volatile long count_2 =0;
volatile long count_3 =0;
//临时变量
long temp_0 = 0;
long temp_2 = 0;
long temp_3 = 0;
//流量预设
double flow_0 = 0;
double flow_2 = 0;
double flow_3 = 0;
//安全压力预设为0.8Mpa
double safe_pressure = 0.8;
//流速预设
double flowrate_0 = 0;
double flowrate_2 = 0;
double flowrate_3 = 0;
//设定默认浇灌量为30L
double set_flow_0 = 30;
double set_flow_2 = 30;
double set_flow_3 = 30;
double set_flow_zong = 30;

//完成标志
bool seclect = 0;


//管路继电器状态
bool relay_status_0=0;
bool relay_status_2=0;
bool relay_status_3=0;
//安全阀继电器状态
bool safe_relay = 0;

//下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,1,228); //使用DHCPAddressPrinter扫描出

//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1WquA1UMew"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Arduino_seedbed"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "ea45b3064b28ead54ccf0ec6a763592c"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;

void setup()
{
Serial.begin(9600);
//中断输入
pinMode(PIN0,INPUT);
pinMode(PIN2,INPUT);
pinMode(PIN3,INPUT);

//管路1继电器
pinMode(22,OUTPUT);
//管路2继电器
pinMode(23,OUTPUT);
//管路3继电器
pinMode(24,OUTPUT);
//安全阀继电器
pinMode(25,OUTPUT);

//中断的方式计算流量
attachInterrupt(0,blink_0,FALLING);
attachInterrupt(2,blink_2,FALLING);
attachInterrupt(3,blink_3,FALLING);

Ethernet.begin(mac,ip);  //初始化以太网

//初始化 iot，需传入实例化的的ethclient，和设备产品信息
AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

//初始化
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
digitalWrite(25,HIGH);
relay_status_0=0;
relay_status_2=0;
relay_status_3=0;
safe_relay = 0;

//绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数
//继电器
AliyunIoTSDK::bindData("relay_status_0",relay_status0);
AliyunIoTSDK::bindData("relay_status_2",relay_status2);
AliyunIoTSDK::bindData("relay_status_3",relay_status3);
AliyunIoTSDK::bindData("rs",rs);
AliyunIoTSDK::bindData("zong",zong);
//设定流量
AliyunIoTSDK::bindData("set_flow_0",set_flow0);
AliyunIoTSDK::bindData("set_flow_2",set_flow2);
AliyunIoTSDK::bindData("set_flow_3",set_flow3);
AliyunIoTSDK::bindData("set_flow_zong",set_flowzong);

AliyunIoTSDK::bindData("all_open",all_open);
AliyunIoTSDK::bindData("all_close",all_close);
}

void loop()
{
AliyunIoTSDK::loop();//必要函数

if (millis() - lastMsMain >= 1000) 
{
lastMsMain = millis();
//安全阀为0-2.5Mpa，需要把它等分后乘以读到的模拟量，公式为：读数=2.5/1023*模拟量
safe_pressure=(2.5/1023) * analogRead(A2);

//回传信息
//流量、流速信息
AliyunIoTSDK::send("flowrate_0",flowrate_0);  
AliyunIoTSDK::send("flow_0",flow_0);
AliyunIoTSDK::send("flowrate_2",flowrate_2);  
AliyunIoTSDK::send("flow_2",flow_2);
AliyunIoTSDK::send("flowrate_3",flowrate_3);  
AliyunIoTSDK::send("flow_3",flow_3);  
//继电器状态信息
AliyunIoTSDK::send("relay_status_0",relay_status_0); 
AliyunIoTSDK::send("relay_status_2",relay_status_2); 
AliyunIoTSDK::send("relay_status_3",relay_status_3); 
//设定流量信息
AliyunIoTSDK::send("set_flow_zong",set_flow_0); 
AliyunIoTSDK::send("set_flow_zong",set_flow_2); 
AliyunIoTSDK::send("set_flow_zong",set_flow_3); 
AliyunIoTSDK::send("set_flow_zong",set_flow_zong); 
//安全阀继电器状态和安全阀值信息
AliyunIoTSDK::send("safe_relay",safe_relay); 
AliyunIoTSDK::send("safe_pressure", safe_pressure);

AliyunIoTSDK::send("all_open",all_open); 
AliyunIoTSDK::send("all_close",all_close); 


//计算流量
cal_0();
cal_2();
cal_3();

//判断，当前流量如果比设定流量大，表示流量满足预设值，则停止
//管路1流量判断
if(flow_0 >= set_flow_0)
{
digitalWrite(22,HIGH);
relay_status_0=1;
}
//管路2判断
if(flow_2 >= set_flow_2)
{
digitalWrite(23,HIGH);
relay_status_2=1;
}
//管路3判断
if(flow_3 >= set_flow_3)
{
digitalWrite(24,HIGH);
relay_status_3=1;
}
//安全阀判断，当管路压力大于0.9Mpa时，释放压力
if(safe_pressure >= 0.9)
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

//流量计算
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

//流量计算方法：
//流量=这一段时间的流量（vt）+前一段时间的流量（vt）
//流速=（当前流量-前一段流量）/（当前时间-前一段时间），v=▲s/▲t，因为时间很短，用微分思想可以近似理解为流速
//除以60，将单位变为L/s
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
}

//回调函数控制第一路
void relay_status0(JsonVariant L)
{
int relay_status_0 = L["relay_status_0"];//参数l
if (relay_status_0 == 0)
{
digitalWrite(22,LOW);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
relay_status_0=1;
relay_status_2=0;
relay_status_3=0;
}
}

//第二路
void relay_status2(JsonVariant L)
{
int relay_status_2 = L["relay_status_2"];//参数l
if (relay_status_2 == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,LOW);
digitalWrite(24,HIGH);
relay_status_0=0;
relay_status_2=1;
relay_status_3=0;
}
}

//第三路
void relay_status3(JsonVariant L)
{
int relay_status_3 = L["relay_status_3"];//参数l
if (relay_status_3 == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,LOW);
relay_status_0=0;
relay_status_2=0;
relay_status_3=1;
}
}

//重置继电器
void rs(JsonVariant L)
{
int rs = L["rs"];//参数l
if (rs == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
relay_status_0=0;
relay_status_2=0;
relay_status_3=0;
flow_0=0;
flow_2=0;
flow_3=0;
}
}

//总控制
void zong(JsonVariant L)
{

int zong = L["zong"];//参数l

if (zong == 0)
{
seclect=1;
while(seclect==1){


for(;set_flow_zong > flow_0;)
{
digitalWrite(22,LOW);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
cal_0();
cal_2();
cal_3();
AliyunIoTSDK::send("flowrate_0",flowrate_0);  
AliyunIoTSDK::send("flow_0",flow_0);
AliyunIoTSDK::send("flowrate_2",flowrate_2);  
AliyunIoTSDK::send("flow_2",flow_2);
AliyunIoTSDK::send("flowrate_3",flowrate_3);  
AliyunIoTSDK::send("flow_3",flow_3);  

AliyunIoTSDK::send("relay_status_0",relay_status_0); 
AliyunIoTSDK::send("relay_status_2",relay_status_2); 
AliyunIoTSDK::send("relay_status_3",relay_status_3); 

AliyunIoTSDK::send("set_flow_zong",set_flow_0); 
AliyunIoTSDK::send("set_flow_zong",set_flow_2); 
AliyunIoTSDK::send("set_flow_zong",set_flow_3); 
AliyunIoTSDK::send("set_flow_zong",set_flow_zong); 
  
AliyunIoTSDK::send("safe_relay",safe_relay); 
AliyunIoTSDK::send("safe_pressure", safe_pressure);
}

for(;set_flow_zong > flow_2;)
{
digitalWrite(22,HIGH);
digitalWrite(23,LOW);
digitalWrite(24,HIGH);
relay_status_0=1;

cal_0();
cal_2();
cal_3();
AliyunIoTSDK::send("flowrate_0",flowrate_0);  
AliyunIoTSDK::send("flow_0",flow_0);
AliyunIoTSDK::send("flowrate_2",flowrate_2);  
AliyunIoTSDK::send("flow_2",flow_2);
AliyunIoTSDK::send("flowrate_3",flowrate_3);  
AliyunIoTSDK::send("flow_3",flow_3);  

AliyunIoTSDK::send("relay_status_0",relay_status_0); 
AliyunIoTSDK::send("relay_status_2",relay_status_2); 
AliyunIoTSDK::send("relay_status_3",relay_status_3); 

AliyunIoTSDK::send("set_flow_zong",set_flow_0); 
AliyunIoTSDK::send("set_flow_zong",set_flow_2); 
AliyunIoTSDK::send("set_flow_zong",set_flow_3); 
AliyunIoTSDK::send("set_flow_zong",set_flow_zong); 

AliyunIoTSDK::send("safe_relay",safe_relay); 
AliyunIoTSDK::send("safe_pressure", safe_pressure);
}

for(;set_flow_zong > flow_3;)
{

digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,LOW);
relay_status_2=1;

cal_0();
cal_2();
cal_3();
  
AliyunIoTSDK::send("flowrate_0",flowrate_0);  
AliyunIoTSDK::send("flow_0",flow_0);
AliyunIoTSDK::send("flowrate_2",flowrate_2);  
AliyunIoTSDK::send("flow_2",flow_2);
AliyunIoTSDK::send("flowrate_3",flowrate_3);  
AliyunIoTSDK::send("flow_3",flow_3);  

AliyunIoTSDK::send("relay_status_0",relay_status_0); 
AliyunIoTSDK::send("relay_status_2",relay_status_2); 
AliyunIoTSDK::send("relay_status_3",relay_status_3); 

AliyunIoTSDK::send("set_flow_zong",set_flow_0); 
AliyunIoTSDK::send("set_flow_zong",set_flow_2); 
AliyunIoTSDK::send("set_flow_zong",set_flow_3); 
AliyunIoTSDK::send("set_flow_zong",set_flow_zong); 

AliyunIoTSDK::send("safe_relay",safe_relay); 
AliyunIoTSDK::send("safe_pressure", safe_pressure);
}


digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
relay_status_2=1;
AliyunIoTSDK::send("flowrate_0",flowrate_0);  
AliyunIoTSDK::send("flow_0",flow_0);
AliyunIoTSDK::send("flowrate_2",flowrate_2);  
AliyunIoTSDK::send("flow_2",flow_2);
AliyunIoTSDK::send("flowrate_3",flowrate_3);  
AliyunIoTSDK::send("flow_3",flow_3);  

AliyunIoTSDK::send("relay_status_0",relay_status_0); 
AliyunIoTSDK::send("relay_status_2",relay_status_2); 
AliyunIoTSDK::send("relay_status_3",relay_status_3); 

AliyunIoTSDK::send("set_flow_zong",set_flow_0); 
AliyunIoTSDK::send("set_flow_zong",set_flow_2); 
AliyunIoTSDK::send("set_flow_zong",set_flow_3); 
AliyunIoTSDK::send("set_flow_zong",set_flow_zong); 

AliyunIoTSDK::send("safe_relay",safe_relay); 
AliyunIoTSDK::send("safe_pressure", safe_pressure);

seclect = 0;
}
}
}

void all_open(JsonVariant L)
{
int all_open = L["all_open"];//参数l

if (all_open == 0)
{
digitalWrite(22,LOW);
digitalWrite(23,LOW);
digitalWrite(24,LOW);
relay_status_0=1;
relay_status_2=1;
relay_status_3=1;
}
}

void all_close(JsonVariant L)
{
int all_close = L["all_close"];
if(all_close == 0)
{
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
relay_status_0=0;
relay_status_2=0;
relay_status_3=0;
}

}

//设定流量0
void set_flow0(JsonVariant L)
{
set_flow_0 = L["set_flow_0"];//参数l
}

//设定流量2
void set_flow2(JsonVariant L)
{
set_flow_2 = L["set_flow_2"];//参数l
}

//设定流量3
void set_flow3(JsonVariant L)
{
set_flow_3 = L["set_flow_3"];//参数l
}

//设定总流量
void set_flowzong(JsonVariant L)
{
set_flow_zong = L["set_flow_zong"];//参数l
}
