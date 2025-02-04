#define BLYNK_PRINT Serial

#define BLYNK_TEMPLATE_ID "TMPL6znMymr2u"
#define BLYNK_TEMPLATE_NAME "Temperature monitor using DS18B20 sensor"
#define BLYNK_AUTH_TOKEN "o1qgSCawGVWQDMOWjtTv0v-TfJmzL5Xg"

// Motor A pins (Forward/Backward control)
#define motorA1 5  // IN1 for Motor A (forward)
#define motorA2 4  // IN2 for Motor A (backward)

// Joystick virtual pins
#define joystickY V6  // Y-axis controls forward/backward

#include <BlynkSimpleEsp32.h>
#include <WiFi.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "MIST";  
char pass[] = "Il0veMIST";      

// Function to initialize motor control pins
void setupMotors() {
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);

  // Initial motor state: stopped
  stopMotors();
}

// Stop the motor
void stopMotors() {
  digitalWrite(motorA1, LOW);
  digitalWrite(motorA2, LOW);
}

// Control motor movement based on joystick Y-axis position
void moveBoat(int yValue) {
  // Forward/backward control based on Y-axis
  if (yValue > 200) {
    // Move Forward
    digitalWrite(motorA1, HIGH);
    digitalWrite(motorA2, LOW);
  } else if (yValue < -200) {
    // Move Backward
    digitalWrite(motorA1, LOW);
    digitalWrite(motorA2, HIGH);
  } else {
    // Stop
    stopMotors();
  }
}

// Joystick Y-axis handler (for forward/backward)
BLYNK_WRITE(joystickY) {
  int yValue = param.asInt();  // Get joystick Y value
  Serial.println(yValue);      // Debugging output
  moveBoat(yValue);            // Pass Y value to moveBoat function
}

void setup() {
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);  // Connect to Blynk server
  setupMotors();                  // Initialize motor pins
}

void loop() {
  Blynk.run();  // Run Blynk
}
