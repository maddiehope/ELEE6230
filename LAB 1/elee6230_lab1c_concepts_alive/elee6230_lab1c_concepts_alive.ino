#include <Servo.h>

Servo servo1;
Servo servo2;

const int potPin = A0;  // Potentiometer connected to analog pin A0

void setup() {
  servo1.attach(9);  // Attach servo1 to pin 9
  servo2.attach(10); // Attach servo2 to pin 10
  Serial.begin(9600);
}

void loop() {
  // Call each demonstration function here
  //demonstrateAccuracy();
  //demonstrateResolution();
  demonstrateFuelGauge();
  //demonstrateNoiseVsInterference();
  //demonstrateSensitivity();
  //demonstrateSaturation();
}

// Read potentiometer and map to servo angle
int getPotAngle() {
  int rawValue = analogRead(potPin);
  int servoAngle = map(rawValue, 0, 1023, 0, 180);
  
  Serial.print("Raw: ");
  Serial.print(rawValue);
  
  return servoAngle;
}

// -------------------------------------------------------------------- //
void demonstrateAccuracy() {
  int servoAngle = getPotAngle();

  servo1.write(servoAngle);  // accurate movement
  servo2.write(servoAngle + random(-5, 6));  // add error

  delay(50);
}
// -------------------------------------------------------------------- //
void demonstrateResolution() {
  int servoAngle = getPotAngle();

  servo1.write(servoAngle);  // full resolution
  servo2.write(map(servoAngle & 0xE0, 0, 224, 0, 180));  // 3-bit resolution

  delay(50);
}
// -------------------------------------------------------------------- //
void demonstrateFuelGauge() {
  int servoAngle = getPotAngle();

  servo1.write(servoAngle);  // smooth movement
  
  // non-linear movement 
  int pos = servoAngle < 90 ? map(servoAngle, 0, 90, 0, 60) :
            servoAngle < 135 ? map(servoAngle, 90, 135, 60, 75) :
            map(servoAngle, 135, 180, 75, 180);
  servo2.write(pos);
  
  delay(50);
}
// -------------------------------------------------------------------- //
void demonstrateNoiseVsInterference() {
  int servoAngle = getPotAngle();

  servo1.write(servoAngle + random(-5, 6));  // add noise
  
  // add interference (periodic disturbance)
  int interference = 15 * sin(servoAngle * PI / 90);
  servo2.write(constrain(servoAngle + interference, 0, 180));
  
  delay(50);
}
// -------------------------------------------------------------------- //
void demonstrateSensitivity() {
  int servoAngle = getPotAngle();

  servo1.write(servoAngle);  // normal sensitivity
  servo2.write(constrain(servoAngle * 2, 0, 180));  // increased sensitivity

  delay(50);
}
// -------------------------------------------------------------------- //
void demonstrateSaturation() {
  int rawValue = analogRead(potPin);
  int servoAngle = map(rawValue, 0, 1023, 0, 255);  // 0-255 

  int normalPos = map(servoAngle, 0, 255, 0, 180);
  servo1.write(normalPos);  // normal behavior
  
  // saturated behavior
  int saturatedPos = servoAngle < 85 ? map(servoAngle, 0, 85, 0, 180) :
                     servoAngle > 170 ? 180 :
                     map(servoAngle, 85, 170, 180, 180);
  servo2.write(saturatedPos);
  
  delay(50);
}
// -------------------------------------------------------------------- //