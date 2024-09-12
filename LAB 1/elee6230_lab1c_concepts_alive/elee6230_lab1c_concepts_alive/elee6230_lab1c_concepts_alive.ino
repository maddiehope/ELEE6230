#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_eap_client.h>
#include <ThingSpeak.h>

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

void setup() {
  Serial.begin(115200);
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
  Serial.print("Local ESP32 IP address: ");
  Serial.println(WiFi.localIP());

  ThingSpeak.begin(wifiClient);
}

void loop() {
  unsigned long last_time = millis();

  // Your loop code here
  // For example, you could add ThingSpeak data sending here
}