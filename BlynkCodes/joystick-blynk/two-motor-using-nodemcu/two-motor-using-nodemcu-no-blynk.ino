// Motor A connections
#define IN1 13
#define IN2 12
#define ENA 25

// Motor B connections
#define IN3 14
#define IN4 27
#define ENB 26

// PWM Channel settings
const int pwmChannelA = 0;
const int pwmChannelB = 1;
const int pwmFreq = 5000;
const int pwmResolution = 8; // Resolution 8-bit (values 0 - 255)

// Speed variables
int motorSpeedA = 200; // Motor A speed (0-255)
int motorSpeedB = 200; // Motor B speed (0-255)

void setup() {
  // Set motor control pins as output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Configure PWM for motor speed control
  ledcSetup(pwmChannelA, pwmFreq, pwmResolution);
  ledcSetup(pwmChannelB, pwmFreq, pwmResolution);
  
  // Attach PWM to motor control pins
  ledcAttachPin(ENA, pwmChannelA);
  ledcAttachPin(ENB, pwmChannelB);

  // Set initial direction
  motorStop();
}

void loop() {
  // Example 1: Move forward
  motorForward();
  delay(2000);

  // Example 2: Move backward
  motorBackward();
  delay(2000);

  // Example 3: Stop motors
  motorStop();
  delay(2000);

  // Example 4: Turn left (Motor B moves forward, Motor A stops)
  motorLeft();
  delay(2000);

  // Example 5: Turn right (Motor A moves forward, Motor B stops)
  motorRight();
  delay(2000);

  // Stop motors after movements
  motorStop();
  delay(2000);
}

// Function to move both motors forward
void motorForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(pwmChannelA, motorSpeedA);
  ledcWrite(pwmChannelB, motorSpeedB);
}

// Function to move both motors backward
void motorBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  ledcWrite(pwmChannelA, motorSpeedA);
  ledcWrite(pwmChannelB, motorSpeedB);
}

// Function to turn left (only Motor B moves)
void motorLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(pwmChannelA, 0);  // Stop Motor A
  ledcWrite(pwmChannelB, motorSpeedB);  // Motor B at speed
}

// Function to turn right (only Motor A moves)
void motorRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(pwmChannelA, motorSpeedA);  // Motor A at speed
  ledcWrite(pwmChannelB, 0);  // Stop Motor B
}

// Function to stop both motors
void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(pwmChannelA, 0);  // Stop Motor A
  ledcWrite(pwmChannelB, 0);  // Stop Motor B
}
