
 //Aliyun通讯部分背景设置开始-------------------------------------------

  //下列4个库为与Aliyun通讯部分的库
#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x05}; //自定义mac地址，不会冲突即可
//IPAddress ip(192,168,3,331); //使用DHCPAddressPrinter扫描出
IPAddress ip(192,168,137,99);


  //设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1CF7RlogYY"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "curtain_node_1"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "78f696e112c247000576af046526c29f"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;

//Aliyun通讯部分背景设置结束-------------------------------------------

//天帘电机控制部分背景设置开始------------------------------------------
//定义天帘的开闭
bool curtainOpen = false;
bool curtainClose = false;

long thresholdTime = 5000;

long counter = 0;

//白色遥控器通过外接继电器，进而控制遥控器的按键 
int forwardPin = 26;
int stopPin = 28;
int backwardPin = 27;

//定义遥控开关
int aPin = 30;
int bPin = 33;
int cPin = 32;
int dPin = 31;

//点触式电机向上
void motorForward() {

  digitalWrite(forwardPin, LOW);
  delay(500);
  digitalWrite(forwardPin, HIGH);  
}

//点触式电机向下
void motorBackward() {
digitalWrite(backwardPin, LOW);
delay(500);
digitalWrite(backwardPin, HIGH);
}

//点触式电机停止
void motorStop(){
  digitalWrite(stopPin, LOW);
  delay(500);
  digitalWrite(stopPin, HIGH);
}

//天帘电机控制部分背景设置结束------------------------------------------
void setup()
{
  Ethernet.begin(mac,ip);  //初始化以太网

  //初始化 iot，需传入实例化的的ethclient，和设备产品信息
  AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

  //绑定一个设备属性回调，当远程修改此属性，会触发本地的函数
  AliyunIoTSDK::bindData("curtainClose",Close);
  AliyunIoTSDK::bindData("curtainOpen",Open);
  
  
  Serial.begin(9600);
//白色遥控器的继电器
  pinMode(forwardPin,OUTPUT);
  pinMode(stopPin,OUTPUT);
  pinMode(backwardPin,OUTPUT);
  //遥控器的abcd
  pinMode(aPin,INPUT);
  pinMode(bPin,INPUT);
  pinMode(cPin,INPUT);
  pinMode(dPin,INPUT);

//初始化
  digitalWrite(forwardPin, HIGH);
  digitalWrite(stopPin, HIGH);
  digitalWrite(backwardPin, HIGH);  
}

void loop()
{
  //a键出发
  if (digitalRead(aPin) == HIGH){
    curtainOpen = true;
    curtainClose = false;
    counter = 0;
    Serial.println("signal Detected");
  }
//c键出发
  if (digitalRead(cPin) == HIGH){
    curtainClose = true;
    curtainOpen = false;
    counter = 0;
  }  
  
  if (curtainOpen){

    counter++;
    
    if(counter == 1){
      Serial.println("Curtain Opened");
      motorForward();
      delay(1000);
    }  
  }

   else if (curtainClose){

    counter ++;

    if(counter == 1){
      Serial.println("Curtain Closed");

    motorBackward();
    delay(1000);
    }
    }

//b键触发停止
   if (digitalRead(bPin) == HIGH){

    motorStop();
   }
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 10) //>=1则为0.1秒，10为1秒，下同
  {
    lastMsMain = millis();   

    //发送窗帘状态到云端 
    AliyunIoTSDK::send("curtainClose",curtainClose);
    AliyunIoTSDK::send("curtainOpen",curtainOpen);
  } 
}

//回调函数控制关闭
void Close(JsonVariant L) 
{
  curtainClose = L["curtainClose"];//框中填入Aliyun传回的属性值，并把该值赋到本地变量上
  curtainOpen = false;
  counter = 0;
}

//回调函数控制开启
void Open(JsonVariant L) 
{
  curtainOpen = L["curtainOpen"];//框中填入Aliyun传回的属性值，并把该值赋到本地变量上
  curtainClose = false;
  counter = 0;
}
