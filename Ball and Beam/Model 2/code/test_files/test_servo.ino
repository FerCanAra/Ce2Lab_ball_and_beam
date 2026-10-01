#include <Servo.h>

Servo myServo;                                                       //Initialize Servo.

void resetServo() {
  myServo.write(70);  // Set servo to 102 degrees for horizontal positioning limit in 70 and 140
  delay(500);          // Delay to allow the servo to reach the desired position
}
void setup() {
  myServo.attach(11); // Attach the servo to pin 9 OR 11
  resetServo();
}

void loop() {
  resetServo();
}