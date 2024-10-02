#include <Servo.h>

Servo fsrServo;  // Servo controlled by FSR
Servo ldrServo;  // Servo controlled by LDR

const int fsrPin = A1;
const int ldrPin = A2;
int fsrReading, ldrReading;
int fsrServoPosition, ldrServoPosition;

void setup() {
  fsrServo.attach(9);  // FSR servo on pin 9
  ldrServo.attach(10); // LDR servo on pin 10
  Serial.begin(9600);
}

void loop() {
  // Read FSR and control first servo
  fsrReading = analogRead(fsrPin);
  fsrServoPosition = map(fsrReading, 0, 1023, 0, 180);
  fsrServo.write(fsrServoPosition);

  // Read LDR and control second servo
  ldrReading = analogRead(ldrPin);
  ldrServoPosition = map(ldrReading, 0, 1023, 0, 180);
  ldrServo.write(ldrServoPosition);

  // Print readings for debugging
  Serial.print("FSR: ");
  Serial.print(fsrReading);
  Serial.print(" | LDR: ");
  Serial.println(ldrReading);

  delay(100);
}
