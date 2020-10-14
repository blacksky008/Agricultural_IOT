void setup()
{
  Serial.begin(9600);
}
void loop()
{
 uint32_t x = 0x41C5748C;
float y = *(float*)&x;
Serial.println(y,6);
delay(1000);
}
