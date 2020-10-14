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
    Serial.write(val);//通过串口显示
  }
}
