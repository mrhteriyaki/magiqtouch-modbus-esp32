#include "NetworkSettings.h"
#include <WiFi.h>
#include <WiFiClient.h>

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
}
