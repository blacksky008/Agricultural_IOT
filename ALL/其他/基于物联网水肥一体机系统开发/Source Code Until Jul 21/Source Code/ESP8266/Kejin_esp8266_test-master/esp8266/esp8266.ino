#include <ESP8266WiFi.h>//安装esp8266arduino开发环境
static WiFiClient espClient;

#include <AliyunIoTSDK.h>//引入阿里云 IoT SDK
//需要安装crypto库、PubSubClient库

//设置产品和设备的信息，从阿里云设备信息里查看
#define PRODUCT_KEY     "a1zR9dxRd0N"//替换自己的PRODUCT_KEY
#define DEVICE_NAME     "text1"//替换自己的DEVICE_NAME
#define DEVICE_SECRET   "iqY76ZMzcFHqznjegGUlQKNNFrAHEuja"//替换自己的DEVICE_SECRET
#define REGION_ID       "cn-shanghai"//默认cn-shanghai

#define WIFI_SSID       "SKYSIM"//替换自己的WIFI
#define WIFI_PASSWD     "hmdasc2017"//替换自己的WIFI

unsigned long lastMsMain = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);

  //连接到wifi
  wifiInit(WIFI_SSID, WIFI_PASSWD);

  //初始化 iot，需传入 wifi 的 client，和设备产品信息
  AliyunIoTSDK::begin(espClient, PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, REGION_ID);

  //绑定一个设备属性回调，当远程修改此属性，会触发LED函数
  AliyunIoTSDK::bindData("LED", LED);
}

void loop()
{
  AliyunIoTSDK::loop();//必要函数

  if (millis() - lastMsMain >= 2000)//每2秒发送一次
  {
    lastMsMain = millis();

    //发送LED状态到云平台（高电平：1；低电平：0）
    AliyunIoTSDK::send("LEDPIN", digitalRead(13));
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

//灯的属性修改的回调函数
void LED(JsonVariant L)//固定格式，修改参数l
{
  int LED = L["LED"];//参数l
  if (LED == 0)
  {
    digitalWrite(2, HIGH);
    Serial.printf("ok");
  }
  else 
  {
    digitalWrite(2, LOW);
    Serial.printf("no ok");
  }
  Serial.printf("收到的LED是："); Serial.println(LED);
}
