#include <ModbusMaster.h>//modbusmaster库，版本号0.11.1



uint16_t m_startAddress = 20;//要读取的从站寄存器地址，
uint8_t m_length = 1;//从从站起始地址开始你需要读取的数据的长度
uint8_t result;//串口通信结果

ModbusMaster node(1);//对1号从站进行通信

void setup() {
  pinMode(2,OUTPUT);   //2号数字口为DE/RE口，控制数据方向
  node.begin(9600);//定义arduino与从站之间的串口波特率，传感器默认9600
  Serial.begin(9600);//初始化串口
}

void loop() {  

  digitalWrite(2, HIGH);   //控制口高电平，主机开始呼叫从机
  result = node.readHoldingRegisters(m_startAddress, m_length);//调用相关函数
  
  if (result == node.ku8MBSuccess) {//如果通信成功
    digitalWrite(2,LOW);   //控制口低电平，主机开始接收信号
    Serial.print("DATA:");
    for (uint8_t j = 0; j < m_length; j++)
    {
      Serial.print( node.getResponseBuffer(j), DEC );//以DEC（十）进制显示你要的数据
    }
  }
  else {
    
    Serial.print("ERROR:");
    Serial.print(result, HEX);//以HEX（16）进制显示错误信息（E2：你的TX,RX断线，E0：你的485转ttl板子有问题）
  }
delay(500);//延时500毫秒

}
