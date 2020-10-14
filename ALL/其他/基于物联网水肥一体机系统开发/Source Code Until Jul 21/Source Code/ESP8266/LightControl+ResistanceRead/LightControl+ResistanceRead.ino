#include <ESP8266WiFi.h>//安装esp8266arduino开发环境
static WiFiClient espClient;

#include <AliyunIoTSDK.h>//引入阿里云 IoT SDK
//需要安装crypto库、PubSubClient库

//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a12cewNBLYZ"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "ESP8266-2"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "7e70bf2bca566cef06584df24005ca7c"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

#define WIFI_SSID       "SKYSIM"//替换自己的WIFI
#define WIFI_PASSWD     "hmdasc2017"//替换自己的WIFI

unsigned long lastMsMain = 0;
bool LED_Status = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  //连接到wifi
  wifiInit(WIFI_SSID, WIFI_PASSWD);

  //初始化 iot，需传入 wifi 的 client，和设备产品信息
  AliyunIoTSDK::begin(espClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

  //绑定一个设备属性回调，当远程修改此属性，会触发LED函数
  AliyunIoTSDK::bindData("VALUE_Light",LED);
}

void loop()
{
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 1)//每0.1秒发送一次
  {
    lastMsMain = millis();

    //发送LED状态到云平台（高电平：1；低电平：0）
    AliyunIoTSDK::send("Ohm_value",  analogRead(A0));
    Serial.println(analogRead(A0));
    AliyunIoTSDK::send("VALUE_Light",LED_Status);
  }
}

//wifi 连接
void wifiInit(const char *ssid, const char *passphrase)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passphrase);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("WiFi not Connect");
  }
  Serial.println("Connected to AP");
}

void LED(JsonVariant L)//固定格式，修改参数l
{
  int LED = L["VALUE_Light"];//参数l
  if (LED == 1)
  {
    digitalWrite(4, HIGH);
    Serial.printf("success");
  }
  else 
  {
    digitalWrite(4, LOW);
    Serial.printf("failure");
  }
  Serial.printf("收到的LED是："); 
  Serial.println(LED);
  LED_Status = LED;
  
}
