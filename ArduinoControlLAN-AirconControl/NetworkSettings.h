#include <ETH.h>

#define USING_WIRED_ETHERNET 0 //Set to 0 for the normal ESP32 modules. Set to 1 when using the Ethernet ESP32 module from Wireless Tag (WT32-ETH01).


class LanController {
public:
  static void Setup();

private:
  static constexpr char  WIFI_NETWORK_NAME[] = "WiFiNetworkName";      // Change this to your WiFi SSID
  static constexpr char  WIFI_NETWORK_KEY[] = "WirelessPasskey123";  // Change this to your WiFi password
  static constexpr char  NET_HOSTNAME[] = "ESP32-AIRCON"; //Used for DHCP lease and mDNS discovery.

  static constexpr bool OTA_ENABLED = false;
  static constexpr char OTA_PASSWORD[] = "Otppass1234!";
  
  static void ReconnectLoop(void* parameter);
  static void OTATask(void* parameter);
  static void NetworkConnectedEvent(arduino_event_id_t event);
  static void StartMDNS();
};