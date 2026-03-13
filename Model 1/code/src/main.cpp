/* -------------------------------------------------------------------------------------------------------------
Project: Ball and Beam PID Control System

Authors:
Fernando Cañadas Aránega, email: fernando.ca@ual.es
Enrique Rodriguez Miranda, email: erm969@ual.es

License: BSD 3-Clause License
Copyright (c) 2026, Fernando Cañadas Aránega and Enrique Rodriguez Miranda
All rights reserved.

See the LICENSE file or the README.md for full license information.
// NOTE: For the beam to be horizontal,
// the servo motor angle should be about 95 degrees.
-------------------------------------------------------------------------------------------------------------
*/ 
#include <Arduino.h> 
#include <Servo.h>      // Library to control the servo motor (instalar )
#include <PID_v1.h>     // Library that implements a PID controller

// Function prototypes (we will define them later)
void resetServo();      // Moves the servo to a safe starting position
float readPosition();   // Reads the distance from the ultrasonic sensor

// Pins used by the ultrasonic sensor
const int trigPin = 9;
const int echoPin = 10;

// Pin used by the servo motor
const int servoPin = 11;

// PID parameters
// These numbers define how strongly the system corrects errors
float Kp = 0.25;   // Proportional correction (reacts to the error)
float Ki = 0.0;    // Integral correction (reacts to accumulated error)
float Kd = 0.2;    // Derivative correction (smooths the movement)

// Variables used by the PID controller
double Setpoint, Input, Output, ServoOutput;

// Create the PID controller object
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// Create the servo object
Servo myServo;

// Filter parameters
// This filter helps smooth the distance measurement
float alpha = 0.4;              // Number between 0 and 1
float filteredDistance = 0.0;   // Memory of the filtered value

void setup() {

  // Start communication with the computer
  Serial.begin(9600);

  // Attach the servo motor to its pin
  myServo.attach(servoPin);

  // Move the servo to a safe starting position
  resetServo();

  // Configure ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  

  // Take the first distance measurement to initialize the filter
  filteredDistance = readPosition();
  Input = filteredDistance;

  // Turn on the PID controller
  myPID.SetMode(AUTOMATIC);

  // Limit how much the servo can move
  myPID.SetOutputLimits(-60,80);
}

void loop(){

  // Wait 50 milliseconds
  // This means the system updates 20 times per second
  delay(50);

  // Desired distance (target)
  // The system will try to keep the ball at 8 cm
  Setpoint = 8;

  // Read the real distance from the sensor
  float rawDistance = readPosition();

  // LOW-PASS FILTER
  // This makes the measurement smoother and less noisy
  filteredDistance = alpha * rawDistance + (1 - alpha) * filteredDistance;

  // The filtered value is used as the system input
  Input = filteredDistance;

  // The PID controller calculates how much to move the servo
  myPID.Compute();
  
  // Convert PID output into a servo angle
  ServoOutput = 95 + Output;

  // Move the servo to that angle
  myServo.write(ServoOutput);

  // Print information to the serial monitor
  Serial.print("Raw: ");          // Raw sensor distance
  Serial.print(rawDistance);

  Serial.print("  Filtered: ");   // Filtered distance
  Serial.print(filteredDistance);

  Serial.print("  Angle: ");      // Servo angle
  Serial.println(ServoOutput);
}

// This function centers the servo
void resetServo() {
  myServo.write(90);   // 90 degrees is the middle position
  delay(500);          // Wait half a second
}

// This function measures distance using the ultrasonic sensor
float readPosition() {

  // Send a short pulse so the sensor emits a sound wave
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Wait until the echo returns
  long duration = pulseIn(echoPin, HIGH);

  // Convert echo time into distance (in cm)
  float distance = duration * 0.034 / 2;

  // If the value is strange or too large,
  // we use a fixed value to avoid errors
  if (distance > 30 || distance <= 0) {
    distance = 40;
  }

  return distance;
}