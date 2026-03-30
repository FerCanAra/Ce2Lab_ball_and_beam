/* -------------------------------------------------------------------------------------------------------------
Project: Ball and Beam PID Control System

Authors:
Fernando Cañadas Aránega, email: fernando.ca@ual.es
Enrique Rodriguez Miranda, email: erm969@ual.es
José Luis Guzmán Sánchez, email: joguzman@ual.es

License: BSD 3-Clause License
Copyright (c) 2026, Fernando Cañadas Aránega, Enrique Rodriguez Miranda and José Luis Guzmán Sánchez.
All rights reserved.

See the LICENSE file or the README.md for full license information.
// NOTE: For the beam to be horizontal,
// the servo motor angle should be about 95 degrees.
-------------------------------------------------------------------------------------------------------------
*/ 
#include <Arduino.h>
#include <Servo.h>
#include <PID_v1.h>
#include <Wire.h>
#include <VL53L0X.h>

// Function prototypes
void resetServo();
float readPosition();

// Pin del servo
const int servoPin = 11;

// PID parameters
float St = 22; // Setpoint (cm)
float Kp = 0.75;
float Ki = 0.0;
float Kd = 0.5;

// Variables PID
double Setpoint, Input, Output;

// PID object
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

// Objetos
Servo myServo;
VL53L0X sensor;

// Filtro
float alpha = 0.4;
float filteredDistance = 0.0;

void setup() {
  Serial.begin(9600);
  delay(2000);

  Serial.println("Inicializando sistema...");

  // I2C
  Wire.begin();
  if (!sensor.init()) {
    Serial.println("ERROR: No se detecta VL53L0X");
    while (1);
  }

  sensor.setTimeout(500);
  sensor.setMeasurementTimingBudget(50000);

  Serial.println("Sensor OK");

  // Servo
  myServo.attach(servoPin);
  resetServo();

  // Inicializar filtro
  filteredDistance = readPosition();
  Input = filteredDistance;

  // PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-60, 80);

  Serial.println("Sistema listo");
}

void loop() {
  delay(50);

  Setpoint = St;

  float rawDistance = readPosition();

  // Low-pass filter for sensor noise
  filteredDistance = alpha * rawDistance + (1 - alpha) * filteredDistance;

  Input = filteredDistance;

  myPID.Compute();

  double servoAngle = 95 + Output; // 95° so that the mount is horizontal (adjust it using test_servo.ino for your model)

  // Physical limits of the motor
  servoAngle = constrain(servoAngle, 0, 180);

  myServo.write(servoAngle);

  Serial.print("Raw: ");
  Serial.print(rawDistance);
  Serial.print("  Filtered: ");
  Serial.print(filteredDistance);
  Serial.print("  Output: ");
  Serial.print(Output);
  Serial.print("  Angle: ");
  Serial.println(servoAngle);
}

// Reset servo
void resetServo() {
  myServo.write(95); 
  delay(500);
}

// Lectura sensor
float readPosition() {
  uint16_t distance = sensor.readRangeSingleMillimeters();
  float distanceCm = distance / 10.0;

  if (sensor.timeoutOccurred()) {
    Serial.println("Timeout sensor");
    return 30;
  }

  // Saturación
  if (distanceCm > 30 || distanceCm <= 0) {
    distanceCm = 30;
  }
  return distanceCm;
}