const int analogPin = A0;  
unsigned long previousMicros = 0;
unsigned long interval = 1000;  // sampling interval (us)

float fundamentalFreq = 100.0;  // fundamental frequency, 100 Hz
float sampleRate = 0.1; // X times the fundamental frequency

// ----------------------------------------------------------------//
void setup() {
  Serial.begin(115200);  
}
// ----------------------------------------------------------------//
void loop() {
  setSamplingRate(sampleRate);
  sampleAndPlotForDuration(5000);  // sampling for 5s
}
// ----------------------------------------------------------------//
void setSamplingRate(float ratio) {
  interval = (unsigned long)(1000000.0 / (ratio * fundamentalFreq));
  Serial.print("Sampling at ");
  Serial.print(ratio);
  Serial.println("x fundamental frequency");
}
// ----------------------------------------------------------------//
void sampleAndPlotForDuration(unsigned long duration) {
  unsigned long startTime = millis();
  while (millis() - startTime < duration) {
    unsigned long currentMicros = micros();
    if (currentMicros - previousMicros >= interval) {
      previousMicros = currentMicros;
      int sensorValue = analogRead(analogPin);
      float voltage = sensorValue * (5.0 / 1023.0);  
      Serial.println(voltage);
    }
  }
  Serial.println(); 
}
// ----------------------------------------------------------------//