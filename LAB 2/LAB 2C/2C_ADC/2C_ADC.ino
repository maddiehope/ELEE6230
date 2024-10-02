const int THERMISTOR_PIN = A0;  // Analog pin for thermistor input
const float BETA = 3636.0;      // Beta value for the thermistor
const float R0 = 1000.0;        // Nominal resistance at 25°C (298.15K)
const float T0 = 298.15;        // 25°C in Kelvin

void setup() {
  Serial.begin(9600);
  analogReference(DEFAULT);  // Use the default 5V reference voltage
}

void loop() {
  int adcValue = analogRead(THERMISTOR_PIN);
  float voltage = adcToVoltage(adcValue);
  float bridgeVoltage = voltageToBridgeVoltage(voltage);
  float resistance = bridgeVoltageToResistance(bridgeVoltage);
  float temperature = resistanceToTemperature(resistance);

  Serial.print("ADC Value: ");
  Serial.print(adcValue);
  Serial.print(", Voltage: ");
  Serial.print(voltage, 3);
  Serial.print("V, Resistance: ");
  Serial.print(resistance, 2);
  Serial.print("Ω, Temperature: ");
  Serial.print(temperature, 2);
  Serial.println("°C");

  delay(1000);  // Wait for a second before next reading
}

float adcToVoltage(int adcValue) {
  return (adcValue / 1023.0) * 5.0;  // Arduino UNO uses 10-bit ADC (0-1023)
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
