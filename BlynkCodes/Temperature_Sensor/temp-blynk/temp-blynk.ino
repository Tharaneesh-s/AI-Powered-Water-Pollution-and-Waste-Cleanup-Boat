

#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6znMymr2u"
#define BLYNK_TEMPLATE_NAME "Temperature and TDS Monitor"
#define BLYNK_AUTH_TOKEN "o1qgSCawGVWQDMOWjtTv0v-TfJmzL5Xg"

char auth[] = BLYNK_AUTH_TOKEN;
//char ssid[] = "Redmi";  
//char pass[] = "12211221";    

char ssid[] = "Air Cdre Mustafiz";
char pass[] = "mustafiz8430";


#include <Arduino.h>
#include <OneWire.h>
#include <DFRobot_ESP_PH_WITH_ADC.h> 
#include <Adafruit_ADS1015.h>     
#include <DFRobot_ESP_EC.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <BlynkSimpleEsp32.h>
#include <SimpleTimer.h>
 
DFRobot_ESP_EC ec;
DFRobot_ESP_PH_WITH_ADC ph;
Adafruit_ADS1115 ads;
 
float phvoltage, phValue, phtemperature = 25;
float voltage, ecValue, temperature = 25;
 

SimpleTimer timer;
 
 
float readTemperature()
{
  //add your code here to get the temperature from your temperature sensor
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);
  
}
 
void setup()
{
  Serial.begin(115200);
    Blynk.begin(auth, ssid, pass);
  EEPROM.begin(32);//needed EEPROM.begin to store calibration k in eeprom
  ph.begin();
  ec.begin();
  sensors.begin();
 
 
  timer.setInterval(1000L,MainFunction);
}
 
void loop()
{
 
    Blynk.run();
    timer.run(); // Initiates BlynkTimer
    
 
}
 
void MainFunction()
{
 
 
    voltage = analogRead(A0); // A0 is the gpio 36 
    //Serial.print("voltage:");
    //Serial.println(voltage, 4);
 
    temperature = readTemperature();  // read your temperature sensor to execute temperature compensation
    Serial.print("temperature:");
    Serial.print(temperature, 1); 
    Serial.println("^C");
 
    ecValue = ec.readEC(voltage, temperature); // convert voltage to EC with temperature compensation
    Serial.print("EC:");
    //Serial.print(ecValue, 4);
    //Serial.println("ms/cm");
    Serial.println(ecValue); 
 
    
 
// Sensor Values to Blynk application
 
  Blynk.virtualWrite(V2, temperature);
  Blynk.virtualWrite(V3, ecValue);
 
 
  ec.calibration(voltage, temperature); // calibration process by Serail CMD
 
}