#include <dhtnew.h> 


DHTNEW sensor1(1);

void setup()
{
  Serial.begin(9600);

}

void loop()
{
  Serial.println();

  

  Serial.print("Humidity (%): ");
  Serial.println((float)sensor1.getHumidity());

  Serial.print("Temperature (C): ");
  Serial.println((float)sensor1.getTemperature());

  delay(2000);

}
