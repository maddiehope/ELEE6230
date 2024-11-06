const int INA128_PIN = A0;  // Analog input pin connected to the INA128 output
const int SAMPLES = 10;     // Number of samples to average
const int DELAY_MS = 100;   // Delay between readings in milliseconds

// Adjust these based on your specific setup and calibration
const float VREF = 5.0;     // Arduino Uno reference voltage
const float GAIN = 200.0;   // Gain of your INA128 (adjust based on your Rg resistor)

const int OVERSAMPLE = 16;  // Oversample by 16x for 2 extra bits of resolution

// For voltage conversion
const float MAX_ADC = 1023.0; // 10-bit ADC on Arduino Uno

void setup() {
  Serial.begin(115200);
  analogReference(DEFAULT);  // Use default (5V) as analog reference
  Serial.println("INA128 Reading Setup");
}

void loop() {
  int rawValue = getAverageReading();
  long oversampledValue = getOversampledReading();
  
  // Convert raw ADC value to voltage
  float outputVoltage = (rawValue / MAX_ADC) * VREF;
  
  // Calculate the input voltage difference (before amplification)
  float inputDifference = outputVoltage / GAIN;

  // Output for Serial Plotter
  Serial.print("RawADC:");
  Serial.print(rawValue);
  Serial.println();

  delay(DELAY_MS);
}

int getAverageReading() {
  long sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum += analogRead(INA128_PIN);
  }
  return sum / SAMPLES;
}

long getOversampledReading() {
  long sum = 0;
  for (int i = 0; i < OVERSAMPLE; i++) {
    sum += analogRead(INA128_PIN);
  }
  return sum >> 2;  // Divide by 4 to get the average and 2 extra bits
}