
#include <SoftwareSerial.h>
SoftwareSerial Slave(10, 11);  
char val; 
int DE_RE=2;

void setup() {
  Serial.begin(9600);   
  Serial.println("Slave is ready!");
  Slave.begin(9600); 
  pinMode(DE_RE,OUTPUT);   
  digitalWrite(DE_RE,LOW);
}

void loop() {
  while (Serial.available()) {
    digitalWrite(DE_RE,HIGH); 
    val = Serial.read();
    Slave.write(val);
  } 

  digitalWrite(DE_RE,LOW);
  while(Slave.available()) {
    val = Slave.read();
    Serial.write(val);
  } 
}
