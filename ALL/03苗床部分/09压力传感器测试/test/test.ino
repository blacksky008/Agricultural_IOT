void setup(){
  Serial.begin(9600);
}

void loop(){
  Serial.println(((2.5 / 1023) * analogRead(A2)));
  delay(1000);

}