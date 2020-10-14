
#include <ModbusMaster.h> //引入ModbusMaster库，版本号2.0.1

/*!
  We're using a MAX485-compatible RS485 Transceiver.
  Rx/Tx is hooked up to the hardware serial port at 'Serial'.
  The Data Enable and Receiver Enable pins are hooked up as follows:
*/
#define MAX485_DE      3   //此版库要求DE和RE口分开接
#define MAX485_RE_NEG  2

// instantiate ModbusMaster object
ModbusMaster node;         

void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

void setup()
{
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Init in receive mode
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);

  // Modbus communication runs at 9600 baud
  Serial.begin(9600);      

  // Modbus slave ID 1
  node.begin(1, Serial);  
  // Callbacks allow us to configure the RS485 transceiver correctly
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void loop()
{
  uint8_t result;     //示例中留有这两个变量，目的不明
  uint16_t data[6];
  
  

  // Read 16 registers starting at 0x3100),我们的传感器是0x0020，9个寄存器
  result = node.readInputRegisters(0x0020, 9); 
  if (result == node.ku8MBSuccess)
  {
    Serial.print("Nice");
    Serial.println(node.getResponseBuffer(0x01)); //暂时不明这里0x01是依据什么规则填写的
    
  }

  delay(1000);
}
