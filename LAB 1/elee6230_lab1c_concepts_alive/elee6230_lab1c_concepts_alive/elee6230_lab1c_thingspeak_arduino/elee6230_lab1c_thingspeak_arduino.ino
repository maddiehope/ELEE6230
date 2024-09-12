#include <SoftwareSerial.h>

SoftwareSerial espSerial(2, 3); // RX, TX

// Define your sensor pins
#define SENSOR1_PIN A0
#define SENSOR2_PIN A1
#define SENSOR3_PIN A2

void setup() {
  Serial.begin(9600);  // For debugging
  espSerial.begin(9600);  // For communication with ESP32
  
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
  pinMode(SENSOR3_PIN, INPUT);
}

void loop() {
  int sensor1 = analogRead(SENSOR1_PIN);
  int sensor2 = analogRead(SENSOR2_PIN);
  int sensor3 = analogRead(SENSOR3_PIN);
  
  // Send data to ESP32
  espSerial.print(sensor1);
  espSerial.print(",");
  espSerial.print(sensor2);
  espSerial.print(",");
  espSerial.println(sensor3);
  
  // For debugging
  Serial.print("Sending: ");
  Serial.print(sensor1);
  Serial.print(",");
  Serial.print(sensor2);
  Serial.print(",");
  Serial.println(sensor3);
  
  delay(15000);  // Wait for 15 seconds before next reading
}
