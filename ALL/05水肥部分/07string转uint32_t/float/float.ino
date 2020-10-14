void setup()
{
  Serial.begin(9600);
}
void loop()
{
  String sz="3fb9999a";
  char buf[16];
  uint32_t tmp;
  sz.toCharArray(buf,sizeof(buf));
  sscanf(buf,"%lx",&tmp);
  delay(1000);

uint32_t x =uint32_t(tmp);
float y = *(float*)&x;
Serial.println(y,6);
delay(1000);

}
