#include "HX711.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SimpleKalmanFilter.h>
#include <Servo.h>

// HX711 Pins
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

// Servo Motor Pin
const int SERVO_PIN = 9;

// HX711 Setup
HX711 scale;
const int SAMPLES = 5;
const long TIMEOUT = 1000;
float calibration_factor = 7000; // Adjust as needed
float rest_reading_hx711 = 0;

// MPU6050 Setup
Adafruit_MPU6050 mpu; // MPU6050 instance
SimpleKalmanFilter kalmanY(2, 2, 0.01); // Kalman filter for Y-axis
double accX, accY, accZ;
double filteredY, initialPositionY; // Displacement values
uint32_t timer;

// Servo Setup
Servo servo;

// Feedback Loop Parameters
float combinedOutput = 0.0;
float Kp = 0.5; // Proportional gain
float smoothingFactor = 0.8; // Smoothing for stability

// Combined Output Range
const float COMBINED_MIN = -5.00;
const float COMBINED_MAX = 0.0;

// Function Prototypes
float getAverageReadingHX711();
float calculateCombinedPosition(float hx711_reading, double mpu_displacement);
float applyFeedbackLoop(float hx711_reading, double mpu_displacement);

void setup() {
  Serial.begin(115200);

  // HX711 Initialization
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  if (scale.wait_ready_timeout(TIMEOUT)) {
    Serial.println("HX711: Keep in rest position...");
    scale.set_scale(calibration_factor);
    scale.tare();
    delay(3000);
    rest_reading_hx711 = getAverageReadingHX711();
    Serial.println("HX711 calibration complete.");
  } else {
    Serial.println("HX711 not found. Check wiring!");
    while (1);
  }

  // MPU6050 Initialization
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100); // Allow sensor to stabilize

  // Calibrate MPU6050 for initial position
  Serial.println("MPU6050: Calibrating initial position...");
  double sum = 0.0;
  const int samples = 100; // Settling samples
  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    accX = a.acceleration.x;
    accZ = a.acceleration.z;
    double accPitch = atan2(-accX, accZ) * RAD_TO_DEG;
    sum += kalmanY.updateEstimate(accPitch);
    delay(10);
  }
  initialPositionY = sum / samples;
  Serial.println("MPU6050 calibration complete.");
  timer = micros();

  // Servo Initialization
  servo.attach(SERVO_PIN);
  servo.write(0); // Set to midpoint (90 degrees) initially

  // Print headers for Serial Plotter
  Serial.println("HX711\tMPU6050\tCombined\tServo Angle");
}

void loop() {
  // HX711 Reading
  float reading_hx711 = getAverageReadingHX711();

  // MPU6050 Reading
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  accX = a.acceleration.x;
  accZ = a.acceleration.z;

  double accPitch = atan2(-accX, accZ) * RAD_TO_DEG; // Calculate displacement
  filteredY = kalmanY.updateEstimate(accPitch);
  double relativeDisplacement = filteredY - initialPositionY;

  // Apply Feedback Loop
  combinedOutput = applyFeedbackLoop(reading_hx711, relativeDisplacement);

  // Map combined output to servo angle (0-180 degrees)
  int servoAngle = map(constrain(combinedOutput, COMBINED_MIN, COMBINED_MAX), COMBINED_MIN, COMBINED_MAX, 0, 180);
  servo.write(servoAngle);

  // Output values for Serial Plotter
  Serial.print("HX711:");
  Serial.print(reading_hx711);         // HX711 reading
  Serial.print(",MPU6050:");
  Serial.print(relativeDisplacement); // MPU6050 displacement
  Serial.print(",Detetction:");
  Serial.println(combinedOutput);       // Combined position

  delay(10); // Update every 10ms
}

// Get averaged HX711 reading
float getAverageReadingHX711() {
  float sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum += scale.get_units();
  }
  return sum / SAMPLES;
}

// Apply feedback loop to adjust combined output
float applyFeedbackLoop(float hx711_reading, double mpu_displacement) {
  // Calculate normalized readings relative to resting position
  float norm_hx711 = hx711_reading - rest_reading_hx711;
  float norm_mpu = mpu_displacement;

  // Calculate error (difference between HX711 and MPU6050 readings)
  float error = norm_hx711 - norm_mpu;

  // Apply proportional control to minimize error
  float adjustment = Kp * error;

  // Adjust the combined output with smoothing
  float newCombinedOutput = (norm_hx711 + norm_mpu + adjustment) / 2.0;
  combinedOutput = smoothingFactor * combinedOutput + (1 - smoothingFactor) * newCombinedOutput;

  // Return the adjusted combined output
  return combinedOutput;
}

