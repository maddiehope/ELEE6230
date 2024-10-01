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
// ----------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  delay(1000);

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
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    
    // Parse the received data
    int comma1 = data.indexOf(',');
    int comma2 = data.indexOf(',', comma1+1);
    
    float temperature = data.substring(0, comma1).toFloat();
    int fsrValue = data.substring(comma1+1, comma2).toInt();
    int ldrValue = data.substring(comma2+1).toInt();

    // Send data to ThingSpeak
    ThingSpeak.setField(1, temperature);
    ThingSpeak.setField(2, fsrValue);
    ThingSpeak.setField(3, ldrValue);
    
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if (x == 200) {
      Serial.println("Channel update successful.");
    } else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }
  }
}