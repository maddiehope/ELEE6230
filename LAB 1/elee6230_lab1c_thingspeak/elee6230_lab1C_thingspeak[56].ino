#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_eap_client.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

// WIFI INIT --------------------------------------------------------------------- //
#define EAP_ANONYMOUS_IDENTITY ""
#define EAP_IDENTITY "mph63858@uga.edu"
#define EAP_PASSWORD "Rocky656428654278872!" // password for eduroam account

#define USE_EAP

#ifdef USE_EAP
  const char* ssid = "eduroam"; // eduroam SSID
#else
  const char* ssid = "hnet"; // home wifi SSID
  const char* WPA_PASSWORD = "hnetbricealison081002"; // password for home wifi
#endif

WiFiClient wifiClient;
// ------------------------------------------------------------------------------- //
// THINGSPEAK INIT --------------------------------------------------------------- //
unsigned long myChannelNumber = 2647294;
const char * myWriteAPIKey = "N0JA62MXN8E147M4";
// ------------------------------------------------------------------------------- //
// SENSOR INIT ------------------------------------------------------------------- //

// BMP280 sensor
Adafruit_BMP280 bmp;

// Ultrasonic sensor pins
const int trigPin = 2;  // GPIO2
const int echoPin = 3;  // GPIO3

// Hall effect sensor pin
const int hallPin = 4;  // GPIO4 (ADC1_CH4)

// Initialize our values
float temperature = 0;
float pressure = 0;
float distance = 0;
int hallValue = 0;

// ------------------------------------------------------------------------------- //

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Initialize I2C for BMP280
  Wire.begin(6, 7);  // SDA on GPIO6, SCL on GPIO7

  // Initialize BMP280
  if (!bmp.begin(0x77, &Wire)) {  // Try 0x77 if 0x76 doesn't work
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Setup Ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Hall effect sensor doesn't need setup (it's an analog input)

  // establishing connection
  WiFi.disconnect(true);
  WiFi.mode(WIFI_STA);

  #ifdef USE_EAP
    esp_eap_client_set_identity((uint8_t *)EAP_ANONYMOUS_IDENTITY, strlen(EAP_ANONYMOUS_IDENTITY));
    esp_eap_client_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_eap_client_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wifi_sta_enterprise_enable();
    WiFi.begin(ssid); // connect to wifi
  #else
    WiFi.begin(ssid, WPA_PASSWORD);
  #endif

  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local ESP32-C3 IP address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(wifiClient);
}

void loop() {
  // Read BMP280 sensor
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure() / 100.0F; // Convert Pa to hPa

  // Read Ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Calculate distance in cm

  // Read Hall effect sensor
  hallValue = analogRead(hallPin);

  // Set the fields with the sensor values
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, pressure);
  ThingSpeak.setField(3, distance);
  ThingSpeak.setField(4, hallValue);

  // set the status
  ThingSpeak.setStatus("Sensor Readings");

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  // Print sensor values to Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Pressure: ");
  Serial.print(pressure);
  Serial.print(" hPa, Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Hall: ");
  Serial.println(hallValue);

  delay(20000); // Wait 20 seconds to update the channel again (ThingSpeak has a 15-second update limit)
}