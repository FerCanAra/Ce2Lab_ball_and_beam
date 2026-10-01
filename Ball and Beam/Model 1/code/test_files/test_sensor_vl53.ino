#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

float minDistance = 1000;  // Very high initial value
float maxDistance = 0;     // Very low initial value
int readingCount = 0;

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("\n======================================");
  Serial.println("   VL53L0X SENSOR CALIBRATION");
  Serial.println("======================================\n");
  
  Wire.begin();
  delay(100);
  
  if (!sensor.init()) {
    Serial.println("ERROR: Sensor not detected");
    while (1) delay(1000);
  }
  
  Serial.println("Sensor detected");
  
  sensor.setTimeout(500);
  sensor.setMeasurementTimingBudget(50000);
  
  Serial.println("\n======================================");
  Serial.println("INSTRUCTIONS:");
  Serial.println("1. Move the sensor slowly closer to an object");
  Serial.println("2. Then move it away until it is no longer detected");
  Serial.println("3. The test will run for 60 seconds");
  Serial.println("======================================\n");
  
  delay(3000);
  Serial.println("STARTING TEST...\n");
}

void loop() {
  static unsigned long startTime = millis();
  unsigned long elapsedTime = millis() - startTime;
  
  // 60-second test
  if (elapsedTime > 60000) {
    printResults();
    while (1) delay(1000);
  }
  
  // Read distance
  uint16_t distance = sensor.readRangeSingleMillimeters();
  
  if (sensor.timeoutOccurred()) {
    Serial.println("TIMEOUT");
    return;
  }
  
  float distanceCm = distance / 10.0;
  
  // Only valid readings
  if (distance > 0 && distance < 8000) {
    if (distanceCm < minDistance) {
      minDistance = distanceCm;
      Serial.print("NEW MIN: ");
      Serial.print(minDistance);
      Serial.println(" cm");
    }
    
    if (distanceCm > maxDistance) {
      maxDistance = distanceCm;
      Serial.print("NEW MAX: ");
      Serial.print(maxDistance);
      Serial.println(" cm");
    }
    
    readingCount++;
  }
  
  // Show current values
  Serial.print("Current: ");
  Serial.print(distanceCm);
  Serial.print(" cm | Min: ");
  Serial.print(minDistance);
  Serial.print(" cm | Max: ");
  Serial.print(maxDistance);
  Serial.println(" cm");
  
  delay(300);
}

void printResults() {
  Serial.println("\n======================================");
  Serial.println("   CALIBRATION RESULTS");
  Serial.println("======================================\n");
  
  Serial.print("Minimum distance: ");
  Serial.print(minDistance);
  Serial.println(" cm");
  
  Serial.print("Maximum distance: ");
  Serial.print(maxDistance);
  Serial.println(" cm");
  
  Serial.print("Total range: ");
  Serial.print(maxDistance - minDistance);
  Serial.println(" cm");
  
  Serial.print("Valid readings: ");
  Serial.println(readingCount);
  
  Serial.println("\n======================================");
  Serial.println("ANALYSIS:");
  Serial.println("======================================\n");
  
  if (minDistance <= 6.5) {
    Serial.println("Minimum range (~6 cm) is NORMAL for VL53L0X");
  } else {
    Serial.print("Minimum range higher than expected: ");
    Serial.println(minDistance);
  }
  
  if (maxDistance >= 200) {
    Serial.println("Maximum range > 2 meters: EXCELLENT");
  } else if (maxDistance >= 100) {
    Serial.println("Maximum range: GOOD");
  } else {
    Serial.println("Maximum range: LIMITED");
  }
  
  Serial.println("\n======================================");
  Serial.println("RECOMMENDATIONS:");
  Serial.println("======================================\n");
  
  if (minDistance > 6.0) {
    Serial.print("- Minimum working distance: ");
    Serial.print(minDistance + 0.5);
    Serial.println(" cm");
  } else {
    Serial.println("- You can work from 6–7 cm");
  }
  
  Serial.println("- Recommended setpoint: 15 cm");
  Serial.println("- Recommended range: 7–25 cm");
  
  Serial.println("\nTest finished. Arduino idle.\n");
}