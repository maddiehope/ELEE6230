#include "HX711.h"

const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;

const int SAMPLES = 10;
const long TIMEOUT = 1000;

float calibration_factor = 7000; // Adjust as needed

float rest_reading = 0;

void setup() {
  Serial.begin(115200);
  
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  if (scale.wait_ready_timeout(TIMEOUT)) {
    Serial.println("Keep in rest position...");
    scale.set_scale(calibration_factor);
    scale.tare();
    delay(3000);
    rest_reading = getAverageReading();

    Serial.println("Calibration complete. Start swinging the object.");
    
  } else {
    Serial.println("HX711 not found. Check wiring!");
    while (1);
  }
}

void loop() {
  if (scale.wait_ready_timeout(TIMEOUT)) {
    float reading = getAverageReading();
  
    // Output for Serial Plotter
    Serial.print("Reading:");
    Serial.print(reading);
    Serial.println();
  }

  delay(100);  // Short delay for smoother plotting
}

float getAverageReading() {
  float sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum += scale.get_units();
  }
  return sum / SAMPLES;
}