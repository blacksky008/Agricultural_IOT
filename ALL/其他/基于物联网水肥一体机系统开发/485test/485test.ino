//master可以理解为485通讯
//serial理解为串口通讯
#include <SoftwareSerial.h>
SoftwareSerial Master(10, 11); 
char val; 
int DE_RE=2;

void setup() {
  Serial.begin(9600);   
  Serial.println("Master is ready!");
  Master.begin(9600);
  pinMode(DE_RE,OUTPUT);  
  digitalWrite(DE_RE,LOW); 
}

//十进制转十六进制
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

void loop() { //发送数据
  while (Serial.available()) {
    digitalWrite(DE_RE,HIGH);//高发送
    val = Serial.read();//把串口的数据读到val中
    
    Master.write(val);//通过master发送val
  }


//接收，在loop中一直处于low，表示一直监听数据，当有数据输入时变为高电平，为发送态
  digitalWrite(DE_RE,LOW);//低电平时接收数据
  while(Master.available()) {//通过master接收信号
    val = Master.read();//将数据存储到val中
//    String hex = tohex(val);

//   Serial.print(hex);

   // Serial.print("+");
     Serial.write(val);//通过串口显示
  }
}
