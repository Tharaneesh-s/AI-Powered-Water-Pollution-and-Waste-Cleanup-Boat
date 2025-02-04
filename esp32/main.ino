#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6znMymr2u"
#define BLYNK_TEMPLATE_NAME "Temperature and TDS Monitor"
#define BLYNK_AUTH_TOKEN "o1qgSCawGVWQDMOWjtTv0v-TfJmzL5Xg"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TdsSensorPin 34
#define VREF 3.3
#define SCOUNT 30

const int oneWireBus = 15;

#define TurbiditySensorPin 35

#define motorA1 18
#define motorA2 19
#define motorB1 5
#define motorB2 4
#define joystickY V10
#define joystickX V9

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Redmi";
char pass[] = "12211221";

BlynkTimer timer;

int analogBuffer[SCOUNT];
int analogBufferTemp[SCOUNT];
int analogBufferIndex = 0;
float averageVoltage = 0, tdsValue = 0, temperature = 25, turbidityNTU = 0;

int getMedianNum(int bArray[], int iFilterLen) {
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++) {
    for (i = 0; i < iFilterLen - j - 1; i++) {
      if (bTab[i] > bTab[i + 1]) {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

void sendTDS() {
  static unsigned long analogSampleTimepoint = millis();
  if (millis() - analogSampleTimepoint > 40U) {
    analogSampleTimepoint = millis();
    analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);
    analogBufferIndex++;
    if (analogBufferIndex == SCOUNT)
      analogBufferIndex = 0;
  }

  static unsigned long printTimepoint = millis();
  if (millis() - printTimepoint > 800U) {
    printTimepoint = millis();
    int copy_index;
    for (copy_index = 0; copy_index < SCOUNT; copy_index++)
      analogBufferTemp[copy_index] = analogBuffer[copy_index];
    averageVoltage = getMedianNum(analogBufferTemp, SCOUNT) * (float)VREF / 4096.0;
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
    float compensationVolatge = averageVoltage / compensationCoefficient;
    tdsValue = (133.42 * compensationVolatge * compensationVolatge * compensationVolatge - 255.86 * compensationVolatge * compensationVolatge + 857.39 * compensationVolatge) * 0.5;

    Serial.print("TDS Value: ");
    Serial.print(tdsValue, 0);
    Serial.println(" ppm");

    Blynk.virtualWrite(V2, tdsValue);
  }
}

void sendTemperature() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println("ºC");

  if (tempC != DEVICE_DISCONNECTED_C) {
    temperature = tempC;
    Blynk.virtualWrite(V0, tempC);
  } else {
    Serial.println("Error: Could not read temperature data");
  }
}

void sendTurbidity() {
  int turbidityValue = analogRead(TurbiditySensorPin);
  float voltage = turbidityValue * (VREF / 4096.0);
  turbidityNTU = -1120.4 * voltage * voltage + 5742.3 * voltage - 4352.9;

  Serial.print("Turbidity: ");
  Serial.print(turbidityNTU);
  Serial.println(" NTU");

  if (turbidityNTU < 20) {
    Blynk.virtualWrite(V4, "Clear");
  } else if (turbidityNTU >= 20 && turbidityNTU < 50) {
    Blynk.virtualWrite(V4, "Cloudy");
  } else if (turbidityNTU >= 50) {
    Blynk.virtualWrite(V4, "dirty");
  }
}

void sendWQI() {
  float tempNorm = (temperature - 0) / (35 - 0);
  float turbidityNorm = (turbidityNTU - 0) / (100 - 0);

  float tempWeight = 0.1;
  float turbidityWeight = 0.08;

  float wqi = ((tempNorm * tempWeight)) / (tempWeight) * 100;

  wqi = constrain(wqi, 0, 100);

  Serial.print("Water Quality Index: ");
  Serial.println(wqi);

  String qualityDesc;
  if (wqi >= 90) qualityDesc = "Very Good";
  else if (wqi >= 70) qualityDesc = "Good";
  else if (wqi >= 50) qualityDesc = "Moderate";
  else if (wqi >= 25) qualityDesc = "Poor";
  else qualityDesc = "Very Poor";

  Blynk.virtualWrite(V12, String(wqi, 2) + " WQI - " + qualityDesc);
}

void setupMotors() {
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  stopMotors();
}

void stopMotors() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW);
  digitalWrite(motorB2, LOW);
}

void moveBoats(int yValue, int xValue) {
  if (yValue >= 235 && (xValue >= 98 && xValue <= 158)) {
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
    Serial.println("Moving Forward");
  } else if (yValue <= 20 && (xValue >= 98 && xValue <= 158)) {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, HIGH);
    Serial.println("Moving Backward");
  } else if (xValue >= 235 && (yValue >= 68 && yValue <= 188)) {
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
    digitalWrite(motorB1, LOW);
    digitalWrite(motorB2, HIGH);
    Serial.println("Turning Right");
  } else if (xValue <= 20 && (yValue >= 68 && yValue <= 188)) {
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
    digitalWrite(motorB1, HIGH);
    digitalWrite(motorB2, LOW);
    Serial.println("Turning Left");
  } else if (xValue == 128 && yValue == 128) {
    stopMotors();
    Serial.println("Stopping Motors");
  }
}

BLYNK_WRITE(joystickY) {
  int yValue = param.asInt();
  moveBoats(yValue, 128);
}

BLYNK_WRITE(joystickX) {
  int xValue = param.asInt();
  moveBoats(128, xValue);
}

void setup() {
  Serial.begin(115200);
  sensors.begin();
  Blynk.begin(auth, ssid, pass);
  setupMotors();
  timer.setInterval(1000L, sendTemperature);
  timer.setInterval(1000L, sendTDS);
  timer.setInterval(1000L, sendTurbidity);
  timer.setInterval(5000L, sendWQI);
}

void loop() {
  Blynk.run();
  timer.run();
}
