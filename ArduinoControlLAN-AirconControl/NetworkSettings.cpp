#include "NetworkSettings.h"
#include <WiFi.h>
#include <WiFiClient.h>
//#include <ArduinoOTA.h>

const char* ssid = "WiFiNetworkName";      // Change this to your WiFi SSID
const char* password = "WirelessPasskey123";  // Change this to your WiFi password

void LanController::Setup() {
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
}

void LanController::DisconnectCheck() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, attempting to reconnect...");
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      attempts++;
      Serial.print(".");
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi reconnected");
    } else {
      Serial.println("WiFi reconnection failed");
    }
  }
}