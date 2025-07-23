#include "NetworkSettings.h"

// Important to be defined BEFORE including ETH.h for ETH.begin() to work.
// Example RMII LAN8720 (Olimex, etc.)
#define ETH_PHY_TYPE ETH_PHY_LAN8720
#define ETH_PHY_ADDR 0
#define ETH_PHY_MDC 23
#define ETH_PHY_MDIO 18
#define ETH_PHY_POWER -1
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN

#include <ETH.h>
#include <WiFi.h>

static bool eth_connected = false;
unsigned long lastEthCheck = 0;

// WARNING: WiFiEvent is called from a separate FreeRTOS task (thread)!
void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("ESP32-HVAC-ESP");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

void EthernetStart() {
//for use of WT32-ETH01 ESP32 Module.
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  // The argument order changed in esp32-arduino v3+
  ETH.begin(ETH_PHY_LAN8720, 1, 23, 18, 16, ETH_CLOCK_GPIO0_IN);
#else
  ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN);
#endif
}


void LanController::Setup() {
  Serial.println("Ethernet Network Startup.");
  Serial.println("Note - Ethernet will not start with programmer connected and powered.");
  WiFi.onEvent(WiFiEvent);  // Will call WiFiEvent() from another thread.
  EthernetStart();
}


void LanController::DisconnectCheck() {
  // Check Ethernet every 30 seconds
  unsigned long currentMillis = millis();
  if (currentMillis - lastEthCheck > 30000) {
    if (!eth_connected) {
      Serial.println("Ethernet disconnected, attempting to reconnect");
      EthernetStart();
    }
    lastEthCheck = currentMillis;
  }
}
