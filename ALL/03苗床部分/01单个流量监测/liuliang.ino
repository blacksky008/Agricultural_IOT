#define PIN 20
volatile long count =0;
long temp = 0;
void setup() {
  // put your setup code here, to run once:
  pinMode(PIN,INPUT);
  attachInterrupt(3,blinkA,FALLING);
  Serial.begin(9600);
}
 
void loop() {
  // put your main code here, to run repeatedly:
  double q;
  q = (count - temp)/0.45;
  Serial.println(q);
  temp = count;
  delay(500); 
}
void blinkA()
{
  count++;
}
