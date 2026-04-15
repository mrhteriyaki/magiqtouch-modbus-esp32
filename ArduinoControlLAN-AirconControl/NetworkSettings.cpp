#include "NetworkSettings.h"

bool mdns_started = false;
Preferences LanController::prefs;

void LanController::Setup() {

  Network.onEvent(NetworkConnectedEvent);

#if USING_WIRED_ETHERNET == 1 && ESP_ARDUINO_VERSION_MAJOR >= 3
  // The argument order changed in esp32-arduino v3+
  ETH.begin(ETH_PHY_LAN8720, 1, 23, 18, 16, ETH_CLOCK_GPIO0_IN);
#elif USING_WIRED_ETHERNET == 1
  ETH.begin(1, 16, 23, 18, ETH_PHY_LAN8720, ETH_CLOCK_GPIO0_IN);
#endif

  //Use longer WiFi scan method for network selection.
  //Default fast-scan method selects WAP by lowest channel number.
  WiFi.setScanMethod(WIFI_ALL_CHANNEL_SCAN);
  WiFi.setSortMethod(WIFI_CONNECT_AP_BY_SIGNAL);
#if USING_WIRED_ETHERNET == 1
  String WiFiHostname = String(NET_HOSTNAME) + "-WiFi";
  WiFi.setHostname(WiFiHostname.c_str());
#else
  WiFi.setHostname(NET_HOSTNAME);
#endif

  String ssid_load, passkey_load;
  loadWiFiCredentials(ssid_load, passkey_load);
  if (ssid_load.isEmpty() || passkey_load.isEmpty()) {
    WiFi.softAP(NET_HOSTNAME, softAPpassword);
  } else {
    WiFi.begin(ssid_load, passkey_load);
  }

  xTaskCreate(ReconnectLoop, "ReconnectLoop", 4096, NULL, 1, NULL);
  if (OTA_ENABLED) {
    xTaskCreate(OTATask, "OTATask", 4096, NULL, 1, NULL);
  }
}

void LanController::saveWiFiCredentials(String &ssid, String &passkey) {
  prefs.begin("wifi", false);  // "wifi" = namespace, false = read/write
  prefs.putString("wifi_ssid", ssid);
  prefs.putString("wifi_key", passkey);
  prefs.end();

  //Restart device to use new details.
  ESP.restart();
}

void LanController::loadWiFiCredentials(String &ssid, String &password) {
  prefs.begin("wifi", true);  // true = read-only
  ssid = prefs.getString("wifi_ssid", "");
  password = prefs.getString("wifi_key", "");
  prefs.end();
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

void LanController::NetworkConnectedEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      ETH.setHostname(NET_HOSTNAME);
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
      StartMDNS();
      break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("WiFi connected");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());
      StartMDNS();
      break;
  }
}

void LanController::StartMDNS() {

  if (mdns_started) {
    MDNS.end();
  }
  if (MDNS.begin(NET_HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("magiqaircon", "tcp", 80);
    mdns_started = true;
  }
}

void LanController::OTATask(void* parameter) {
  ArduinoOTA.onStart([]() {
              String type = ArduinoOTA.getCommand() == U_FLASH ? "sketch" : "filesystem";
              // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
              Serial.println("Start updating " + type);
            })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.setHostname(NET_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.begin();
  while (true) {
    ArduinoOTA.handle();
    vTaskDelay(20);
  }
}