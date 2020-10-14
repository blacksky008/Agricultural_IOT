//中断定义
#define PIN0 2
#define PIN2 21
#define PIN3 20

//计数
volatile long count_a =0;
volatile long count_b =0;
volatile long count_c =0;

//中间变量
long temp_a = 0;
long temp_b = 0;
long temp_c = 0;

//流量
double flow_a = 0;
double flow_b = 0;
double flow_c = 0;

//安全阀压强，默认为0.8Mpa
double safe_pressure = 0.8;

//流速
double flowrate_a = 0;
double flowrate_b = 0;
double flowrate_c = 0;

//温度
double tep_upload;
double ec_upload;
double ph_upload;
double sal_upload;
double tds_upload;


//下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>

//485通信、Modbus通讯部分背景设置及函数开始-----------------------------------

#include <ModbusMaster.h> //引入ModbusMaster库，版本号2.0.1

//abc桶继电器
bool relay_a=0;
bool relay_b=0;
bool relay_c=0;

//电机
bool motor = 0;

//安全阀状态
bool safe_relay = 0;

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x04 }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,1,253); //使用DHCPAddressPrinter扫描出
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
//实例化485传感器
ModbusMaster node1;    
ModbusMaster node5; 

//485定义
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

//485定义
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

unsigned long lastMsMain = 0;

void setup()
{
//中断作为输入
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

//流量计
attachInterrupt(0,blink_a,FALLING);
attachInterrupt(2,blink_b,FALLING);
attachInterrupt(3,blink_c,FALLING);

Ethernet.begin(mac,ip);  //初始化以太网

//初始化 iot，需传入实例化的的ethclient，和设备产品信息
AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

//初始化
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

//485作为输出口
pinMode(MAX485_RE_NEG, OUTPUT);
pinMode(MAX485_DE, OUTPUT);

  // Init in receive mode
  //初始化485
digitalWrite(MAX485_RE_NEG, 0);
digitalWrite(MAX485_DE, 0);

  // Modbus communication runs at 9600 baud
  //modbus协议在Serial1中，这个决定是因为18，19的tx，rx为uart1
  Serial1.begin(9600);      

 // Modbus slave ID 1,2,3
  node1.begin(1,Serial1);
  node5.begin(5,Serial1);
  Serial.begin(9600);
  // Callbacks allow us to configure the RS485 transceiver correctly
  //回调允许我们正确的配置RS485收发器
  node1.preTransmission(preTransmission);
  node1.postTransmission(postTransmission);
  node5.preTransmission(preTransmission);
  node5.postTransmission(postTransmission);

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
//安全阀
safe_pressure=(2.5/1023) * analogRead(A2);

//上传数据
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

AliyunIoTSDK::send("tep_upload", tep_upload);
AliyunIoTSDK::send("ph_upload", ph_upload);
AliyunIoTSDK::send("ec_upload", ec_upload);
AliyunIoTSDK::send("sal_upload", sal_upload);
AliyunIoTSDK::send("tds_upload", tds_upload);

ec();
sal();
tds();
tep();
ph();

//流量计计算
cal_a();
cal_b();
cal_c();

//当压强大于1Mpa时，卸压
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

//a桶计算
void blink_a()
{
count_a++;
}
//b桶计算
void blink_b()
{
count_b++;
}
//c桶计算
void blink_c()
{
count_c++;
}

//流量计算
void cal_a()
{
flowrate_a = (count_a-temp_a)/0.45;
flowrate_a = flowrate_a/60.00;
temp_a = count_a;
flow_a=flow_a+flowrate_a;
}

//流量计算
void cal_b()
{
flowrate_b = (count_b-temp_b)/0.45;
flowrate_b = flowrate_b/60.00;
temp_b = count_b;
flow_b=flow_b+flowrate_b;
}

//流量计算
void cal_c()
{
flowrate_c = (count_c-temp_c)/0.45;
flowrate_c = flowrate_c/60.00;
temp_c = count_c;
flow_c=flow_c+flowrate_c;
}


//继电器a回调函数
void relaya(JsonVariant L)
{
int relay_a = L["relay_a"];//参数l
}

//继电器b回调函数
void relayb(JsonVariant L)
{
int relay_b = L["relay_b"];//参数l
}

//继电器c回调函数
void relayc(JsonVariant L)
{
int relay_c = L["relay_status_3"];//参数l
}

//复位回调函数
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

//电机开始回调函数
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

//ec数据处理
void ec()
{
  uint8_t result5;   
result5 = node5.readHoldingRegisters(0x0000, 2); 
int ec_01;
int ec_00;
  if (result5 == node5.ku8MBSuccess)
  {
//    Serial.println("Communication With Node 5 Success");
//    ec = ((node5.getResponseBuffer(0x01)<< 8) |node5.getResponseBuffer(0x00)); // 
ec_01 =  node5.getResponseBuffer(0x01);
//        Serial.println(ec_01);
//delay(2000);
ec_00=  node5.getResponseBuffer(0x00);
//Serial.println(ec_00);
//delay(2000);
String ec_01_s = tohex(ec_01);
String ec_00_s = tohex(ec_00);
String hex=String(ec_01_s)+String(ec_00_s);
//Serial.println(hex);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
if(y > 0.01){
ec_upload = y;
delay(2000);
}

  }
  else{
    Serial.println("Communication With Node 5 Failed");
  }
}

//tds数据处理
void tds()
{
  uint8_t result5;   
result5 = node5.readHoldingRegisters(0x0006, 2); 
  int tds_01;
int tds_00;
  if (result5 == node5.ku8MBSuccess)
  {
//    Serial.println("Communication With Node 5 Success");
tds_01 =  node5.getResponseBuffer(0x01);
tds_00=  node5.getResponseBuffer(0x00);

String tds_01_s = tohex(tds_01);
String tds_00_s = tohex(tds_00);
String hex=String(tds_01_s)+String(tds_00_s);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
if(y>0.01){
tds_upload = y;
delay(2000);
}
  }
  else{
    Serial.println("Communication With Node 5 Failed");
  }
}

//管道温度数据处理
void tep()
{
  uint8_t result5;   
result5 = node5.readHoldingRegisters(0x0004, 2); 
  int tep_01;
int tep_00;
  if (result5 == node5.ku8MBSuccess)
  {
//    Serial.println("Communication With Node 5 Success");
tep_01 =  node5.getResponseBuffer(0x01);
tep_00=  node5.getResponseBuffer(0x00);

String tep_01_s = tohex(tep_01);
String tep_00_s = tohex(tep_00);
String hex=String(tep_01_s)+String(tep_00_s);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
if(y>0.01){
tep_upload = y;
delay(2000);
}
  }
  else{
    Serial.println("Communication With Node 5 Failed");
  }
}

//盐度数据处理
void sal()
{
  uint8_t result5;   
result5 = node5.readHoldingRegisters(0x0008, 2); 
  int sal_01;
int sal_00;
  if (result5 == node5.ku8MBSuccess)
  {
//    Serial.println("Communication With Node 5 Success");
sal_01 =  node5.getResponseBuffer(0x01);
sal_00=  node5.getResponseBuffer(0x00);

String sal_01_s = tohex(sal_01);
String sal_00_s = tohex(sal_00);
String hex=String(sal_01_s)+String(sal_00_s);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
if(y>0.01){
sal_upload = y;
delay(2000);
}
  }
  else{
    Serial.println("Communication With Node 5 Failed");
  }
}

//ph传感器数据处理
void ph()
{
  uint8_t result1;   
result1 = node1.readHoldingRegisters(0x0001, 2); 
int ph_01;
int ph_00;  
  if (result1 == node1.ku8MBSuccess)
  {
    Serial.println("Communication With Node 1 Success");
ph_01 =  node1.getResponseBuffer(0x01);
//        Serial.println(ec_01);
ph_00=  node1.getResponseBuffer(0x00);
String ph_01_s = tohex(ph_01);
String ph_00_s = tohex(ph_00);
String hex=String(ph_01_s)+String(ph_00_s);

char buf[16];
  uint32_t tmp;
  hex.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
Serial.println(y,2);
if(y>0.01){
ph_upload = y;
delay(2000);
}
  }
  else{
    Serial.println("Communication With Node 1  Failed");
  }
}

//10进制转16进制
String tohex(int n) {
  if (n == 0) {
    return "00"; //n为0
  }
  String result = "";
  char _16[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
  };
  const int radix = 16;
  while (n) {
    int i = n % radix;          // 余数
    result = _16[i] + result;   // 将余数对应的十六进制数字加入结果
    n /= radix;                 // 除以16获得商，最为下一轮的被除数
  }
  if (result.length() < 2) {
    result = '0' + result; //不足两位补零
  }
  return result;
}

//------------------------------------------
//数据上报
void tep_upload_t (JsonVariant L)
{
tep();
 tep_upload = L["tep_upload"];//参数l
AliyunIoTSDK::send("tep_upload", tep_upload);
}

//-----------------------------------------------
//数据上报
void tds_upload_t (JsonVariant L)
{
  //各传感器汇报数据
tds();
 tds_upload = L["tds_upload"];//参数l
AliyunIoTSDK::send("tds_upload", tds_upload);
}

//--------------------------------------------
//数据上报
void sal_upload_t (JsonVariant L)
{
  //各传感器汇报数据
sal();
 sal_upload = L["sal_upload"];//参数l
AliyunIoTSDK::send("sal_upload", sal_upload);
}
//----------------------------------------------

//数据上报
void ph_upload_t (JsonVariant L)
{
  //各传感器汇报数据
ph();
 ph_upload = L["ph_upload"];//参数l
AliyunIoTSDK::send("ph_upload", ph_upload);
}
//---------------------------------------------------

//数据上报
void ec_upload_t (JsonVariant L)
{
  //各传感器汇报数据
ec();
 ec_upload = L["ec_upload"];//参数l
AliyunIoTSDK::send("ec_upload", ph_upload);
}
