
 //Aliyun通讯部分背景设置开始-------------------------------------------

  //下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>


byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x02, 0x06}; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,1,332); //使用DHCPAddressPrinter扫描出

  //设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a17JrE6iSCv"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "wet_curtain_node_1"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "710bcd8d68793a31ed7b187c2e83670c"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;

//Aliyun通讯部分背景设置结束-------------------------------------------

//天帘电机控制部分背景设置开始------------------------------------------

bool fanOpen = false;
bool fanClose = true;
bool pumpOpen = false;
bool pumpClose = true;

bool putterOpen = false;
bool putterClose = true;

int pumpPin[] = {22,23,24,25};
int fanPin[] = {29};



void putterActivated(){
    digitalWrite(26,LOW);
    digitalWrite(27,HIGH);
//    delay(10000);
//    digitalWrite(26,HIGH);
//    digitalWrite(27,HIGH);
//    delay(1000);
}


void putterDeActivated(){
    digitalWrite(26,HIGH);
    digitalWrite(27,LOW);
//    delay(10000);
//    digitalWrite(26,HIGH);
//    digitalWrite(27,HIGH);
//    delay(1000); 
}
 
void pumpActivated() {

  
  digitalWrite(pumpPin[0],LOW);
  digitalWrite(pumpPin[1],LOW);
  digitalWrite(pumpPin[2],LOW);
  digitalWrite(pumpPin[3],LOW);
  
  Serial.println("Pump Activated");
  
}

void pumpDeActivated() {

  
  digitalWrite(pumpPin[0],HIGH);
  digitalWrite(pumpPin[1],HIGH);
  digitalWrite(pumpPin[2],HIGH);
  digitalWrite(pumpPin[3],HIGH);
  
  Serial.println("Pump DeActivated");
  
}


void fanActivated() {

   
   digitalWrite(fanPin[0],LOW);
   
   Serial.println("Fan Activated");
 
}

void fanDeActivated() {

   
   digitalWrite(fanPin[0],HIGH);
   
   Serial.println("Fan DeActivated");
 
}


//天帘电机控制部分背景设置结束------------------------------------------

void setup()
{
  
  
  Ethernet.begin(mac,ip);  //初始化以太网

  //初始化 iot，需传入实例化的的ethclient，和设备产品信息
  AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

  //绑定一个设备属性回调，当远程修改此属性，会触发本地的函数
  AliyunIoTSDK::bindData("pumpClose",Close1);
  AliyunIoTSDK::bindData("pumpOpen",Open1);
  AliyunIoTSDK::bindData("fanClose",Close2);
  AliyunIoTSDK::bindData("fanOpen",Open2);

 
   AliyunIoTSDK::bindData("putterOpen",Open3);
   AliyunIoTSDK::bindData("putterClose",Close3);
  
  
  Serial.begin(9600);

  for (int i = 0; i<sizeof(pumpPin);++i){
    pinMode(pumpPin[i],OUTPUT);
  }

  for (int i = 0; i<sizeof(fanPin);++i){
    pinMode(fanPin[i],OUTPUT);

//推杠
    pinMode(26,OUTPUT);
    pinMode(27,OUTPUT);

      digitalWrite(26,HIGH);
digitalWrite(27,HIGH);
  }


  
}

void loop()
{
  
  if (pumpOpen){
    
    Serial.println("Pump Opened");
   
    pumpActivated();
  
  }

   if (pumpClose){

    Serial.println("Pump Closed!");

    pumpDeActivated();
    
   }

    if (fanOpen){
    
    Serial.println("Fan Opened");
   
    fanActivated();
  
  }

   if (fanClose){

    Serial.println("fan Closed!");

    fanDeActivated();
    
   }


  
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 10) //>=1则为0.1秒，10为1秒，下同
  {
    lastMsMain = millis();   

    //发送窗帘状态到云端 
    AliyunIoTSDK::send("pumpClose",pumpClose);
    AliyunIoTSDK::send("pumpOpen",pumpOpen);
    AliyunIoTSDK::send("fanClose",fanClose);
    AliyunIoTSDK::send("fanOpen",fanOpen);

    AliyunIoTSDK::send("putterClose",putterClose);
    AliyunIoTSDK::send("putterOpen",putterOpen);

 
  }

  
}
//湿帘
void Close1(JsonVariant L) 
{
  pumpClose = L["pumpClose"];//框中填入Aliyun传回的属性值，并把该值赋到本地变量上
  pumpOpen = false;
 
}
//湿帘
void Open1(JsonVariant L) 
{
  pumpOpen = L["pumpOpen"];//框中填入Aliyun传回的属性值，并把该值赋到本地变量上
  pumpClose = false;
 
}
//风机
void Close2(JsonVariant L) 
{
  fanClose = L["fanClose"];//框中填入Aliyun传回的属性值，并把该值赋到本地变量上
  fanOpen = false;
 
}
//风机
void Open2(JsonVariant L) 
{
  fanOpen = L["fanOpen"];//框中填入Aliyun传回的属性值，并把该值赋到本地变量上
  fanClose = false;  
 
}

//推杠
void Close3(JsonVariant L) 
{
  putterClose = L["putterClose"];
   if(putterClose == 0){
    putterDeActivated();

    Serial.println("putter Closed");
   }
}

void Open3(JsonVariant L)
{
putterOpen = L["putterOpen"];
if(putterOpen == 0){
  Serial.println("putter opened");
    
    putterActivated();
   }
}

