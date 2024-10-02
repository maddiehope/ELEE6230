#include <SoftwareSerial.h>

// SENSOR PINS
const int thermistorPin = A0;
const int fsrPin = A2;
const int ldrPin = A1;
const int AD590_PIN = A3;

// THERMISTOR CONSTANTS
const float BETA = 3636.0;
const float R0 = 1000.0;
const float T0 = 298.15;

// AD590 CONSTANTS
const float RESISTOR = 1000.0;  // 1k resistor value in ohms

// SENSOR VALUES
int fsrValue = 0;
int ldrValue = 0;
float ad590Temperature = 0;

// THRESHOLDS
const int FSR_THRESHOLD = 500;  // Adjust as needed
const int LDR_THRESHOLD = 300;  // Adjust as needed

// CALIBRATION CONSTANTS
const float CALIBRATION_WEIGHT = 0.3;  // Adjust this value between 0 and 1

// Software Serial for ESP8266 communication
SoftwareSerial espSerial(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  analogReference(DEFAULT);  // Use the default 5V reference voltage for other sensors
}

void loop() {
  // Read sensor values
  int thermistorADC = analogRead(thermistorPin);
  fsrValue = analogRead(fsrPin);
  ldrValue = analogRead(ldrPin);
  ad590Temperature = readAD590Temperature();

  // Convert thermistor ADC to temperature
  float thermistorVoltage = adcToVoltage(thermistorADC);
  float bridgeVoltage = voltageToBridgeVoltage(thermistorVoltage);
  float thermistorResistance = bridgeVoltageToResistance(bridgeVoltage);
  float thermistorTemperature = resistanceToTemperature(thermistorResistance);

  // Calibrate thermistor temperature using AD590
  float calibratedThermistorTemp = calibrateThermistor(thermistorTemperature, ad590Temperature, CALIBRATION_WEIGHT);

  // Check conditions and send to ESP8266
  if (fsrValue > FSR_THRESHOLD || ldrValue > LDR_THRESHOLD) {
    // Send calibrated data to ESP8266
    espSerial.print(calibratedThermistorTemp);
    espSerial.print(",");
    espSerial.print(fsrValue);
    espSerial.print(",");
    espSerial.println(ldrValue);
  }

  // Print sensor values for debugging
  Serial.print("Thermistor Temp: ");
  Serial.print(thermistorTemperature * (9.0/5.0) + 32.0, 2);
  Serial.print("°F, AD590 Temp: ");
  Serial.print(ad590Temperature * (9.0/5.0) + 32.0, 2);
  Serial.print("°F, Calibrated Temp: ");
  Serial.print(calibratedThermistorTemp * (9.0/5.0) + 32.0, 2);
  Serial.print("°F, FSR: ");
  Serial.print(fsrValue);
  Serial.print(", LDR: ");
  Serial.println(ldrValue);

  delay(1000); // Wait 1 second before next reading
}

float adcToVoltage(int adcValue) {
  return (adcValue / 1023.0) * 5.0;  // Using 5V reference for other sensors
}

float voltageToBridgeVoltage(float voltage) {
  return (voltage - 2.5) / 2.0;  // Reverse the instrumentation amplifier equation
}

float bridgeVoltageToResistance(float bridgeVoltage) {
  float ratio = (bridgeVoltage / 5.0) + 0.5;
  return (1000.0 * ratio) / (1.0 - ratio);
}

float resistanceToTemperature(float resistance) {
  float lnR = log(resistance / R0);
  float invT = (1.0 / T0) + (1.0 / BETA) * lnR;
  return (1.0 / invT) - 273.15;  // Convert Kelvin to Celsius
}

float readAD590Temperature() {
  int adcValue = analogRead(AD590_PIN);
  float voltage = adcValue * (5.0 / 1023.0);
  
  // 1mV/K, already amplified by 10
  float tempKelvin = voltage * 100.0;
  float tempCelsius = tempKelvin - 273.15; // convert to C
  
  return tempCelsius;
}

float calibrateThermistor(float thermistorTemp, float ad590Temp, float weight) {
    return thermistorTemp * (1 - weight) + ad590Temp * weight;
}
