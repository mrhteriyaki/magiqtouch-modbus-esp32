#include "NetworkSettings.h"
#include <WiFi.h>
#include <WiFiClient.h>
//#include <ArduinoOTA.h>

const char* ssid = "WiFiNetworkName";      // Change this to your WiFi SSID
const char* password = "WirelessPasskey123";  // Change this to your WiFi password

void LanController::Setup() {
  WiFi.setHostname("ESP32-AIRCON");
  
  //Use longer WiFi scan method for network selection.
  //Default fast-scan method selects WAP by lowest channel number.
  WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
  WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Waiting for WiFi to Connect");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  //ArduinoOTA.setPassword("Otppass1234!");
  //ArduinoOTA.begin();
  xTaskCreate(ReconnectLoop, "ReconnectLoop", 4096, NULL, 1, NULL);
}

void LanController::ReconnectLoop(void* parameter) {
  while (true) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected, attempting reconnect...");
      WiFi.begin();
    }
    vTaskDelay(10000);
  }
}