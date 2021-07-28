#include "DHT.h"

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  dht.begin();
}

void loop()
{
  delay(1000);
  Serial.print(dht.readHumidity());
  Serial.print("\t");
  Serial.print(dht.readTemperature());
  Serial.print("\t");
  Serial.println(analogRead(A0));
}
