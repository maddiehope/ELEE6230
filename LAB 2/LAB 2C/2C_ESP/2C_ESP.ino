#include <SoftwareSerial.h>

// SENSOR PINS
const int thermistorPin = A0;
const int fsrPin = A1;
const int ldrPin = A2;
const int ad590Pin = A3;

// SENSOR VALUES
int thermistorValue = 0;
int fsrValue = 0;
int ldrValue = 0;
float ad590Temperature = 0;

// THRESHOLDS
const int FSR_THRESHOLD = 500;  // Adjust as needed
const int LDR_THRESHOLD = 300;  // Adjust as needed

// Software Serial for ESP8266 communication
SoftwareSerial espSerial(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
}

void loop() {
  // Read sensor values
  thermistorValue = analogRead(thermistorPin);
  fsrValue = analogRead(fsrPin);
  ldrValue = analogRead(ldrPin);
  ad590Temperature = readAD590Temperature();

  // Calibrate thermistor using AD590
  float calibratedTemperature = calibrateThermistor(thermistorValue, ad590Temperature);

  // Check conditions and send to ESP8266
  if (fsrValue > FSR_THRESHOLD || ldrValue > LDR_THRESHOLD) {
    // Send data to ESP8266
    espSerial.print(calibratedTemperature);
    espSerial.print(",");
    espSerial.print(fsrValue);
    espSerial.print(",");
    espSerial.println(ldrValue);
  }

  // Print sensor values for debugging
  Serial.print("Thermistor: ");
  Serial.print(thermistorValue);
  Serial.print(", FSR: ");
  Serial.print(fsrValue);
  Serial.print(", LDR: ");
  Serial.print(ldrValue);
  Serial.print(", AD590 Temp: ");
  Serial.print(ad590Temperature);
  Serial.print(", Calibrated Temp: ");
  Serial.println(calibratedTemperature);

  delay(15000); // Wait 15 seconds before next reading
}

float readAD590Temperature() {
  int adcValue = analogRead(ad590Pin);
  float voltage = adcValue * (5.0 / 1023.0);
  return (voltage / 0.001) - 273.15;
}

float calibrateThermistor(int thermistorValue, float trueTemperature) {
  float rawTemperature = thermistorValue * (5.0 / 1023.0);
  float calibrationFactor = trueTemperature / rawTemperature;
  return rawTemperature * calibrationFactor;
}
