void setup() {
  Serial.begin(9600);
  pinMode(A0,INPUT);// put your setup code here, to run once:

}

void loop() {
  float x = analogRead(A0);
  Serial.println(x/1023);
  delay(100);// put your main code here, to run repeatedly:

}
