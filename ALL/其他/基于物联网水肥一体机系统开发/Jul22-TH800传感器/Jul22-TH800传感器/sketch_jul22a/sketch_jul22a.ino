/*      Master      */
#include <SoftwareSerial.h>
SoftwareSerial Master(10, 11);
char val;
int DE_RE=2;

void setup() {
  Serial.begin(9600);   
  Serial.println("Master is ready!");
  Master.begin(9600);
  pinMode(DE_RE,OUTPUT);  
  digitalWrite(DE_RE,LOW); //默认主机不发出信号
}

void loop() {
  while (Serial.available()) {
    digitalWrite(DE_RE,HIGH);   //主机发出呼叫
    val = Serial.read();        //从串口读取地址
    Master.write(val);          //主机根据返回的地址，呼叫从机
  }

  digitalWrite(DE_RE,LOW);      //主机进入接收模式
  while(Master.available()) {
    val = Master.read();        //读取从机发出的信号
    Serial.write(val);          
  }
}
