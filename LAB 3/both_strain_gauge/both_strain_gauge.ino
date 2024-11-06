#include "HX711.h"

// HX711 pins
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

// INA128 pin
const int INA128_PIN = A0;
;
HX711 scale;

const int SAMPLES = 5; // Reduced for faster updates
const long TIMEOUT = 100; // Reduced timeout
const int UPDATE_INTERVAL = 50; // Update every 50ms

float calibration_factor = 7000; // Adjust as needed
float rest_reading_hx711 = 0;
int rest_reading_ina128 = 0;

// Tare interval
const unsigned long TARE_INTERVAL = 60000; // Tare every 60 seconds
unsigned long lastTareTime = 0;
unsigned long lastUpdateTime = 0;

void setup() {
  Serial.begin(115200);
  
  // HX711 setup
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  
  if (scale.wait_ready_timeout(TIMEOUT)) {
    Serial.println("Keep in rest position...");
    scale.set_scale(calibration_factor);
    scale.tare();
    delay(3000);
    rest_reading_hx711 = scale.get_units(SAMPLES);
    rest_reading_ina128 = getAverageReadingINA128();

    Serial.println("Calibration complete. Start swinging the object.");
    
  } else {
    Serial.println("HX711 not found. Check wiring!");
    while (1);
  }

  // INA128 setup
  analogReference(DEFAULT);

  lastTareTime = millis();
  lastUpdateTime = millis();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Periodic tare
  if (currentTime - lastTareTime > TARE_INTERVAL) {
    scale.tare();
    lastTareTime = currentTime;
    Serial.println("Tare performed");
  }

  // Update readings
  if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = currentTime;
    
    if (scale.is_ready()) {
      float reading_hx711 = scale.get_units(1); // Single reading for speed
      int reading_ina128 = analogRead(INA128_PIN); // Single reading for speed
      
      float position = calculatePosition(reading_hx711, reading_ina128);
      
      Serial.print("HX711:");
      Serial.print(reading_hx711);
      Serial.print(",INA128:");
      Serial.print(reading_ina128);
      Serial.print(",Position:");
      Serial.println(position);
    }
  }
}

int getAverageReadingINA128() {
  long sum = 0;
  for (int i = 0; i < SAMPLES; i++) {
    sum += analogRead(INA128_PIN);
  }
  return sum / SAMPLES;
}

float calculatePosition(float hx711_reading, int ina128_reading) {
  // Normalize readings
  float norm_hx711 = normalizeHX711(hx711_reading);
  float norm_ina128 = normalizeINA128(ina128_reading);
  
  // Calibrate (example using simple linear mapping)
  float pos_hx711 = map(norm_hx711, -1, 1, -90, 90);
  float pos_ina128 = map(norm_ina128, -1, 1, -90, 90);
  
  // Combine readings (simple average)
  return (pos_hx711*0.8 + pos_ina128*0.2);
}

float normalizeHX711(float reading) {
  const float HX711_MIN = 50;  // Adjust based on your observations
  const float HX711_MAX = 75;  // Adjust based on your observations
  return (reading - HX711_MIN) / (HX711_MAX - HX711_MIN) * 2 - 1;
}

float normalizeINA128(int reading) {
  const int INA128_MIN = 0;    // Adjust based on your observations
  const int INA128_MAX = 1023; // Assuming 10-bit ADC
  return (float)(reading - INA128_MIN) / (INA128_MAX - INA128_MIN) * 2 - 1;
}