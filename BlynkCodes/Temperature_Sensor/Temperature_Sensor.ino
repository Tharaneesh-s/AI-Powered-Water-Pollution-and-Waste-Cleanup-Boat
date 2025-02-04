#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);	

DallasTemperature sensors(&oneWire);
int sensorPin = A0;

void setup(void)
{
  
  sensors.begin();
  Serial.begin(9600);
}

void loop(void)
{ 
  sensors.requestTemperatures(); 
  Serial.print("Temperature: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print("'C");
  Serial.println();
  delay(500);

/*
  int sensorValue = analogRead(sensorPin);
  int turbidity = map(sensorValue, 0, 750, 100, 0);
  delay(100);

  if (turbidity < 20) 
  {
    Serial.print("Clear\n");
  }
  if ((turbidity > 20) && (turbidity < 50)) 
  {
    Serial.print("Cloudy\n");
  }
  if (turbidity > 50) 
  {
    Serial.print("Dirty\n");
  } */
}