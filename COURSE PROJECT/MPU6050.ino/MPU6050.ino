/********************************************************************

                MPU6050 w/ ARDUINO UNO - Y-Position Tracking

 ********************************************************************/
// NOTES:

// BEFORE RUNNING: Make sure you change 0x68 to 0x72 in the Adafruit_MPU6050.h file 
                // #define MPU6050_DEVICE_ID 0x72 -----> change it from 0x68 to 0x72

// VIEWING OUTPUT: Notice that the serial is set to 115200 baud. 
                // Make sure you set the monitor & plotter to this if you want to view!

// ------------------------------------------------------------------------------------
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <SimpleKalmanFilter.h> // Source: https://github.com/denyssene/SimpleKalmanFilter

Adafruit_MPU6050 mpu; // Create the MPU6050 instance
SimpleKalmanFilter kalmanY(2, 2, 0.01); // Kalman filter for Y-axis (process noise, measurement noise, estimation error)

// Sensor Values ----------------------------------------------------------------------
double accX, accY, accZ;  // Accelerometer data
double gyroY;             // Gyroscope data
double filteredY;         // Kalman filtered Y-axis position
double initialPositionY;  // Initial Y-axis position (set after settling time)

double gyroYangle = 0;    // Gyro angle accumulation
uint32_t timer;           // Timer for delta time calculations
// ------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200); // Set baud rate for Serial Plotter
  while (!Serial) delay(10); // Wait until Serial is active

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) delay(10);
  }

  // Set MPU6050 configurations
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100); // Allow sensor to stabilize

  // Settle Kalman filter for initial position
  Serial.println("Calibrating initial position...");
  double sum = 0.0;
  const int samples = 100; // Number of samples for settling
  for (int i = 0; i < samples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    accX = a.acceleration.x;
    accY = a.acceleration.y;
    accZ = a.acceleration.z;

    double accPitch = atan2(-accX, accZ) * RAD_TO_DEG; // Y-axis pitch angle
    sum += kalmanY.updateEstimate(accPitch);          // Let Kalman filter stabilize
    delay(10);                                        // Small delay between samples
  }
  initialPositionY = sum / samples; // Set initial position as the averaged value
  Serial.println("Calibration complete.");
  timer = micros();                 // Initialize timer
}

void loop() {
  // Get sensor readings
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  accX = a.acceleration.x;
  accY = a.acceleration.y;
  accZ = a.acceleration.z;

  gyroY = g.gyro.y;

  // Calculate delta time
  double dt = (double)(micros() - timer) / 1000000;
  timer = micros();

  // Calculate pitch angle (Y-axis) from accelerometer
  double accPitch = atan2(-accX, accZ) * RAD_TO_DEG;

  // Convert gyroscope reading to degrees/second
  double gyroYrate = gyroY / 131.0;

  // Accumulate gyroscope angle
  gyroYangle += gyroYrate * dt;

  // Apply Kalman filter to combine accelerometer and gyroscope data
  filteredY = kalmanY.updateEstimate(accPitch);

  // Calculate relative position (relative to initial position)
  double relativePositionY = filteredY - initialPositionY;

  // Output relative Y position for Serial Plotter
  Serial.println(relativePositionY);

  delay(10); // Small delay for stability
}
