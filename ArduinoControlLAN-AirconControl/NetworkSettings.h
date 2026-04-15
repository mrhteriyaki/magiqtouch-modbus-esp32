#include <ETH.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <ETH.h>
#include <Preferences.h>

#define ETH_PHY_TYPE ETH_PHY_LAN8720
#define ETH_PHY_ADDR 0
#define ETH_PHY_MDC 23
#define ETH_PHY_MDIO 18
#define ETH_PHY_POWER -1
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN

#define USING_WIRED_ETHERNET 1 //Set to 0 for the normal ESP32 modules. Set to 1 when using the Ethernet ESP32 module from Wireless Tag (WT32-ETH01).

class LanController {
public:
  static void Setup();
  static void saveWiFiCredentials(String& ssid, String& password);
	static void loadWiFiCredentials(String &ssid, String &password);

  static constexpr char  NET_HOSTNAME[] = "ESP32-AIRCON"; //Used for DHCP lease and mDNS discovery.
private:
  static constexpr const char* softAPpassword = "ExtraTasty123!";

  static constexpr bool OTA_ENABLED = true; //Set to true to enable updates over WiFi from Arduino IDE
  static constexpr char OTA_PASSWORD[] = "Otppass1234!";
  
  static Preferences prefs;
  
  static void ReconnectLoop(void* parameter);
  static void OTATask(void* parameter);
  static void NetworkConnectedEvent(arduino_event_id_t event);
  static void StartMDNS();

};