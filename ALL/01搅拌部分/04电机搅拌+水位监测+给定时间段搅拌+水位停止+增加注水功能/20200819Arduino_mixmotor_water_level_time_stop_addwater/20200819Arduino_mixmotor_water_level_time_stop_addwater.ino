//低电平触发
//下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>
//混合桶使用mac地址为00的，ip地址可以手动定义，之后累加
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x00 }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,1,226); //使用DHCPAddressPrinter扫描出，也可以自己定义

//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1ZY2wCfbv3"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Arduino_mixmotor"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "69a8469c0c48bcebeb692f22fbda00e8"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;

//定义检测电机状态
bool motor1_status = 0; //为0则停止运动，为1则开始运动
bool motor2_status = 0; //为0则停止运动，为1则开始运动
bool motor3_status = 0; //为0则停止运动，为1则开始运动

//定义流量判断状态，指示灯
bool liquid_status1 = 0; 
bool liquid_status2 = 0; 
bool liquid_status3 = 0; 

//定义注水继电器
bool water_relay_1 = 0;
bool water_relay_2 = 0;
bool water_relay_3 = 0;

//注水继电器状态
bool water_relay_status_1 = 0;
bool water_relay_status_2 = 0;
bool water_relay_status_3 = 0;


void setup()
{
Ethernet.begin(mac,ip);  //初始化以太网

//初始化 iot，需传入实例化的的ethclient，和设备产品信息
AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

//绑定一个设备属性回调，当远程修改此属性，会触发本地的Relay函数

//搅拌电机
AliyunIoTSDK::bindData("motor1",motor1);
AliyunIoTSDK::bindData("motor2",motor2);
AliyunIoTSDK::bindData("motor3",motor3);

//注水时间
AliyunIoTSDK::bindData("target_time1",target_time1);
AliyunIoTSDK::bindData("target_time2",target_time2);
AliyunIoTSDK::bindData("target_time3",target_time3);
AliyunIoTSDK::bindData("target_time",target_time);

//注水继电器
AliyunIoTSDK::bindData("water_relay_1",water_relay1);
AliyunIoTSDK::bindData("water_relay_2",water_relay2);
AliyunIoTSDK::bindData("water_relay_3",water_relay3);
AliyunIoTSDK::bindData("autowater",autowater);
AliyunIoTSDK::bindData("stopwater",stopwater);




//电机1
pinMode(22,OUTPUT);
//电机2
pinMode(23,OUTPUT);
//电机3
pinMode(24,OUTPUT);

//注水
pinMode(25,OUTPUT);
pinMode(26,OUTPUT);
pinMode(27,OUTPUT);

//初始化
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);

digitalWrite(25,HIGH);
digitalWrite(26,HIGH);
digitalWrite(27,HIGH);
}

void loop()
{
AliyunIoTSDK::loop();//必要函数

if (millis() - lastMsMain >= 1000) 
{
  
lastMsMain = millis();

//水位变送器，买的是0-1米的，因为要转换为百分比显示，因此计算公式为：（读取的数据）/1024*100
double liquid_level1 = ((analogRead(A0)) / 1024.00)*100;//要加.0，不然显示为0
double liquid_level2 = ((analogRead(A1)) / 1024.00)*100;
double liquid_level3 = ((analogRead(A2)) / 1024.00)*100;

//发送读数到云端 

//电机状态
AliyunIoTSDK::send("motor1",motor1_status);
AliyunIoTSDK::send("motor2",motor2_status);
AliyunIoTSDK::send("motor3",motor3_status);  
//水量
AliyunIoTSDK::send("liquid_level1",liquid_level1);  
AliyunIoTSDK::send("liquid_level2",liquid_level2);  
AliyunIoTSDK::send("liquid_level3",liquid_level3); 
//水位指示灯
AliyunIoTSDK::send("liquid_status1",liquid_status1);  
AliyunIoTSDK::send("liquid_status2",liquid_status2);  
AliyunIoTSDK::send("liquid_status3",liquid_status3); 
//注水继电器
AliyunIoTSDK::send("water_relay_1",water_relay_1);  
AliyunIoTSDK::send("water_relay_2",water_relay_2);  
AliyunIoTSDK::send("water_relay_3",water_relay_3); 


//注水继电器状态
AliyunIoTSDK::send("water_relay_status_1",water_relay_status_1);
AliyunIoTSDK::send("water_relay_status_2",water_relay_status_2);
AliyunIoTSDK::send("water_relay_status_3",water_relay_status_3);

water_relay_status_1=0;
water_relay_status_2=0;
water_relay_status_3=0;

//注水停止
//水位控制在90%
if(liquid_level1>=60)
{
digitalWrite(25,HIGH);
liquid_status1=1;
//water_relay_status_1=1;
}
else
{
liquid_status1=0;
//water_relay_status_1=0;
}

if(liquid_level2>=60)
{
digitalWrite(26,HIGH);
liquid_status2=1;
//water_relay_status_2=1;
}
else
{
liquid_status2=0;
//water_relay_status_2=0;
}

if(liquid_level3>=60)
{
digitalWrite(27,HIGH);
liquid_status3=1;
//water_relay_status_3=1;
}
else
{
liquid_status3=0;
//water_relay_status_3=0; 
}

}
}

//搅拌电机1
void motor1(JsonVariant L)
{
int motor1 = L["motor1"];//参数l
if (motor1 == 1)
{
digitalWrite(22,LOW);
motor1_status=1;
}
else 
{
digitalWrite(22, HIGH);
motor1_status=0;
}
}

//搅拌电机2
void motor2(JsonVariant L)
{
int motor2 = L["motor2"];//参数l
if (motor2 == 1)
{
digitalWrite(23,LOW);
motor2_status=1;
}
else 
{
digitalWrite(23, HIGH);
motor2_status=0;
}
}

//搅拌电机3
void motor3(JsonVariant L)
{
int motor3 = L["motor3"];//参数l
if (motor3 == 1)
{
digitalWrite(24,LOW);
motor3_status=1;
}
else 
{
digitalWrite(24, HIGH);
motor3_status=0;
}
}

//目标时间1
void target_time1(JsonVariant L)
{
int target_time1 = L["target_time1"];//参数l
digitalWrite(22,LOW);
target_time1=target_time1*1000;
delay(target_time1);
digitalWrite(22,HIGH);
}

//目标时间2
void target_time2(JsonVariant L)
{
int target_time2 = L["target_time2"];//参数l
digitalWrite(23,LOW);
target_time2=target_time2*1000;
delay(target_time2);
digitalWrite(23,HIGH);
}

//目标时间3
void target_time3(JsonVariant L)
{
int target_time3 = L["target_time3"];//参数l
digitalWrite(24,LOW);
target_time3=target_time3*1000;
delay(target_time3);
digitalWrite(24,HIGH);
}

//设置总的目标时间
void target_time(JsonVariant L)
{
int target_time = L["target_time"];//参数l
digitalWrite(22,LOW);
digitalWrite(23,LOW);
digitalWrite(24,LOW);
target_time=target_time*1000;
delay(target_time);
digitalWrite(22,HIGH);
digitalWrite(23,HIGH);
digitalWrite(24,HIGH);
}

//注水继电器1控制
void water_relay1(JsonVariant L)
{
int water_relay_1 = L["water_relay_1"];//参数l
if (water_relay_1 == 0)
{
digitalWrite(25,LOW);
water_relay_status_1=1;
}else
{
digitalWrite(25,HIGH);
water_relay_status_1=0;
}
}

//注水继电器2控制
void water_relay2(JsonVariant L)
{
int water_relay_2 = L["water_relay_2"];//参数l
if (water_relay_2 == 0)
{
digitalWrite(26,LOW);
water_relay_status_2=1;
}else
{
digitalWrite(26,HIGH);
water_relay_status_2=0;
}
}

//注水继电器3控制
void water_relay3(JsonVariant L)
{
int water_relay_3 = L["water_relay_3"];//参数l
if (water_relay_3 == 0)
{
digitalWrite(27,LOW);
water_relay_status_3=1;
}else
{
digitalWrite(27,HIGH);
water_relay_status_3=0;
}
}

//自动注水，因为有满水判断，只需要触发即可
void autowater(JsonVariant L)
{
int autowater = L["aotuwater"];//参数l
if (autowater == 0)
{
digitalWrite(25,LOW);
digitalWrite(26,LOW);
digitalWrite(27,LOW);
water_relay_status_1=1;
water_relay_status_2=1;
water_relay_status_3=1;
}
}

void stopwater(JsonVariant L)
{
int stopwater = L["stopwater"];//参数l
if (autowater == 0)
{
digitalWrite(25,HIGH);
digitalWrite(26,HIGH);
digitalWrite(27,HIGH);
water_relay_status_1=0;
water_relay_status_2=0;
water_relay_status_3=0;
}
}
