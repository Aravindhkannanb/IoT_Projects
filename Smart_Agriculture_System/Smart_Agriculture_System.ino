#include <LiquidCrystal.h>
#include <Servo.h>

// Pin Definitions
int temp = A1;            // Analog pin for temperature sensor
int moisture = A0;        // Analog pin for soil moisture sensor
int buzzer = 4;           // Digital pin for buzzer
int trig = 13;            // Ultrasonic sensor trigger pin
int echo = 12;            // Ultrasonic sensor echo pin
int digital_motor = 9;    // Digital pin for motor (PWM control)

// Initialize the LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(3, 2, 5, 6, 7, 8);
Servo servo;

void setup() {
  pinMode(temp, INPUT);           // Temperature sensor pin
  pinMode(moisture, INPUT);       // Soil moisture sensor pin
  pinMode(buzzer, OUTPUT);        // Buzzer pin
  pinMode(trig, OUTPUT);          // Ultrasonic trigger pin
  pinMode(echo, INPUT);           // Ultrasonic echo pin
  pinMode(digital_motor, OUTPUT); // Motor PWM pin
  
  servo.attach(11);
  lcd.begin(16, 2);               // Initialize the LCD as 16x2
  Serial.begin(9600);             // Initialize Serial communication
}

void loop() {
  // Read sensor values
  int moisture_raw = analogRead(moisture);
  int temperature_raw = analogRead(temp);
  float temperature = (temperature_raw * 5.0 / 1023.0) * 100.0; // Convert to Celsius
  
  // Print to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Soil Moisture: ");
  Serial.println(moisture_raw);

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Moist: ");
  lcd.print(moisture_raw);

  // Ultrasonic Tank Level
  int level = tank_level(trig, echo);
  Serial.print("Tank Level: ");
  Serial.println(level);

  lcd.setCursor(10, 1);
  lcd.print("Lvl: ");
  lcd.print(level);
  lcd.print("%");

  // Tank level logic
  if (level < 30) {
    motor_start(); // Start motor if tank level < 30%
    lcd.setCursor(0, 1);
    lcd.print("Filling Tank...  ");
  } else {
    motor_stop();  // Stop motor
  }

  // Buzzer alert for high temperature and dry soil
  if (temperature >= 30 && moisture_raw <= 300) { 
    tone(buzzer, 1000); // Turn on buzzer at 1000 Hz
    move_servo(0, 180); // Sweep the servo
  } else {
    noTone(buzzer); // Turn off buzzer
  }

  delay(2000); // Update every 2 seconds
}

// Function to calculate water tank level using ultrasonic sensor
int tank_level(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  int duration = pulseIn(echoPin, HIGH);
  int distance = (duration * 0.034) / 2; // Convert to cm
  
  int level = map(distance, 334, 0, 0, 100); // Assuming max distance = 334 cm
  return constrain(level, 0, 100); // Ensure level is between 0 and 100
}

// Function to start the motor
void motor_start() {
  analogWrite(digital_motor, 255); // Motor at full speed
  Serial.println("Motor ON: Filling Tank");
}

// Function to stop the motor
void motor_stop() {
  analogWrite(digital_motor, 0); // Stop the motor
  Serial.println("Motor OFF: Tank Full or Above Level");
}

// Function to sweep servo motor
void move_servo(int startAngle, int endAngle) {
  for (int angle = startAngle; angle <= endAngle; angle++) {
    servo.write(angle);
    delay(15);
  }
  for (int angle = endAngle; angle >= startAngle; angle--) {
    servo.write(angle);
    delay(15);
  }
}
