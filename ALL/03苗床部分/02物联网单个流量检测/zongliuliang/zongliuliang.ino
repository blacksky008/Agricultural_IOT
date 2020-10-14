#define PIN 2
volatile long count =0;
long temp = 0;
    double flow_0;
  double flowrate_0;

#include <SPI.h>
#include <Ethernet.h>
#include <AliyunIoTSDK.h>
#include <PubSubClient.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x02 }; //自定义mac地址，不会冲突即可
IPAddress ip(192,168,3,26); //使用DHCPAddressPrinter扫描出


//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1WquA1UMew"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "Arduino_seedbed"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "ea45b3064b28ead54ccf0ec6a763592c"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai


EthernetClient ethClient;  //实例化以太网

unsigned long lastMsMain = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN,INPUT);
  attachInterrupt(0,blinkA,FALLING);
  Serial.begin(9600);
  
Ethernet.begin(mac,ip);  //初始化以太网


//初始化 iot，需传入实例化的的ethclient，和设备产品信息
AliyunIoTSDK::begin(ethClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

}
 
void loop() {


  AliyunIoTSDK::loop();//必要函数

if (millis() - lastMsMain >= 1000) 
{
lastMsMain = millis();


  // put your main code here, to run repeatedly:

  flowrate_0 = (count - temp)/0.45;
  flowrate_0=flowrate_0/60.00;
  Serial.println(flowrate_0);
//  Serial.println(q);
  temp = count;

  flow_0 = flow_0+flowrate_0;
  Serial.println(flow_0);
  delay(500); 
}
AliyunIoTSDK::send("flowrate_0",flowrate_0);  
AliyunIoTSDK::send("flow_0",flow_0);
}
void blinkA()
{
  count++;
}
