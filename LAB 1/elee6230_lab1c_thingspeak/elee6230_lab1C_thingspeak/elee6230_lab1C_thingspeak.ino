#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_eap_client.h>
#include <ThingSpeak.h>

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
unsigned long myChannelNumber = 2648751;
const char * myWriteAPIKey = "DGTZ3H4VVTIDSP0Y";
// ------------------------------------------------------------------------------- //
// SENSOR INIT ------------------------------------------------------------------- //

// Potentiometer pin
const int potPin = 1;  // GPIO1 (ADC1_CH0)

// Ultrasonic sensor pins
const int trigPin = 2;  // GPIO2
const int echoPin = 3;  // GPIO3

// Hall effect sensor pin
const int hallPin = 4;  // GPIO4 (ADC1_CH4)

// Initialize our values
int potValue = 0;
float distance = 0;
int hallValue = 0;

// ------------------------------------------------------------------------------- //

void setup() {
  Serial.begin(9600);
  delay(1000);

  // Setup Ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Potentiometer and Hall effect sensor don't need setup (they're analog inputs)

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
  // Read Potentiometer
  potValue = analogRead(potPin);

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
  ThingSpeak.setField(1, potValue);
  ThingSpeak.setField(2, distance);
  ThingSpeak.setField(3, hallValue);

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
  Serial.print("Potentiometer: ");
  Serial.print(potValue);
  Serial.print(", Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Hall: ");
  Serial.println(hallValue);

  delay(5000); // Wait 20 seconds to update the channel again (ThingSpeak has a 15-second update limit)
}