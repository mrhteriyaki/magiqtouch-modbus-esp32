#ifndef WIFISEC_H
#define WIFISEC_H

#include <WiFi.h>
#include <WiFiClient.h>

#define SERIAL1_RX 26
#define SERIAL1_TX 27
#define SERIAL2_RX 16
#define SERIAL2_TX 17


//WiFi Network settings have moved to NetworkSettings.cpp file.

class LanController {
public:
  static void Setup();
  static void DisconnectCheck();
};

#endif