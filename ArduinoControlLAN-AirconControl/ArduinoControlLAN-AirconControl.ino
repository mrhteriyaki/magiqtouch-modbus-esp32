//ESP32 Aircon Interface
//Board Library esp32 by Espressif Systems in use.

//Espressif Board Library 3.x is not stable.
//Ensure board library used is 2.0.17

//Testing with v3 issues.
//Default serial IO9/10 dont work, alternate serial pins resolved serial relay issue.
//Web Stops responding after bootup - not tested.


//Libraries.
#include <WebServer.h>
#include <ArduinoJson.h>
#include <esp_timer.h>

#include "NetworkSettings.h"
#include "PinSettings.h"


//Max message size for modbus, reduce due to ram usage.
#define MAXMSGSIZE 512
#define MAXSLAVEID 30
#define GAPTHRESHOLD 50

WebServer server(80);

SemaphoreHandle_t msgSemaphore;

unsigned long s1previousMillis = 0;
uint8_t serial1buffer[MAXMSGSIZE];
int serial1Index = 0;

unsigned long s2previousMillis = 0;
uint8_t serial2buffer[MAXMSGSIZE];
int serial2Index = 0;

bool SerialOutputModbus = false;

//Aircon Control Variables.
bool SystemPower = false;
uint8_t FanSpeed = 1;
uint8_t SystemMode = 0;
uint8_t TargetTemp = 20;
uint8_t TargetTemp2 = 20;
bool HeaterZone1 = false;
bool HeaterZone2 = false;


//Modes.
//0 = External Fan
//1 = Fan Recycle
//2 = Cooler Manual
//3 = Cooler Auto (Temp Mode)
//4 = Heater


//Aircon Info Variables.
uint16_t CommandInfo = 0x0;  //Increments on command change from Control Panel.
uint8_t SystemPowerInfo = 0x0;
uint8_t EvapModeInfo = 0x0;
uint8_t EvapFanSpeedInfo = 0x0;

uint8_t HeaterModeInfo = 0x0;
uint8_t HeaterFanSpeedInfo = 0x0;
uint8_t Zone1EnabledInfo = 0x0;
uint8_t Zone2EnabledInfo = 0x0;
uint8_t Zone1TempInfo = 0x0;
uint8_t Zone2TempInfo = 0x0;
uint8_t TargetTempInfo = 0x0;
uint8_t TargetTemp2Info = 0x0;
uint8_t ThermisterTempInfo = 0x0;
uint8_t AutomaticCleanRunning = 0x0;

bool sendCommand = false;
uint8_t IOTModuleCommandRequest[] = { 0xEB, 0x03, 0x0B, 0x0C, 0x00, 0x0D, 0x50, 0xE2 };
uint8_t x2vl = 0x9F;  //Increases for each command sent.
uint8_t x3vl = 0x21;  //WiFi DB Signal
uint8_t x4vl = 0x11;  //System Off 10 = On, 11 = Off.
uint8_t x5vl = 0x42;  //Cooler Mode 42 = Off, 02 = On
uint8_t x6vl = 0x43;  //Evap Fan setting.
uint8_t x7vl = 0x04;  //Unknown
uint8_t x8vl = 0x22;  //Heater Fan Settings.

uint8_t x10v = 0x24;
uint8_t x12v = 0x24;
uint8_t x14v = 0x00;  //zone control.

uint8_t x17v = 0x14;  //Zone 2 Temp - Default 20C
uint8_t x18v = 0x14;  //Zone 1 Temp



//Request for IOT Module info - Sends MAC address.
uint8_t IOTModuleInfoRequest[] = { 0xEB, 0x03, 0x03, 0xE4, 0x00, 0x05, 0xD3, 0x70 };
uint8_t IOTModuleInfoResponse[] = { 0xEB, 0x03, 0x0A, 0x01, 0x09, 0x01, 0x09, 0x70, 0x90, 0x2C, 0x65, 0x27, 0x0B, 0xA8, 0x11 };  //15 bytes length


//Recurring responses that match the start of the request sent from CP1 - Function 10 Response.
uint8_t eb1005d80023[] = { 0xEB, 0x10, 0x05, 0xD8, 0x00, 0x23, 0x17, 0xED };  //CRC included.
uint8_t eb1005fb0021[] = { 0xEB, 0x10, 0x05, 0xFB, 0x00, 0x21, 0x67, 0xE6 };
uint8_t eb10061c002c[] = { 0xEB, 0x10, 0x06, 0x1C, 0x00, 0x2C, 0x16, 0x50 };
uint8_t eb1006480004[] = { 0xEB, 0x10, 0x06, 0x48, 0x00, 0x04, 0x57, 0x9E };
uint8_t eb10064C0038[] = { 0xEB, 0x10, 0x06, 0x4C, 0x00, 0x38, 0x16, 0x4E };
uint8_t eb100684002a[] = { 0xEB, 0x10, 0x06, 0x84, 0x00, 0x2A, 0x17, 0xBD };
uint8_t eb1006ae002a[] = { 0xEB, 0x10, 0x06, 0xAE, 0x00, 0x2A, 0x36, 0x75 };
uint8_t eb1006d80019[] = { 0xEB, 0x10, 0x06, 0xD8, 0x00, 0x19, 0x97, 0xBA };
uint8_t eb1008e50001[] = { 0xEB, 0x10, 0x08, 0xE5, 0x00, 0x01, 0x04, 0x94 };  //Info from CP1 to IOT for On/Off State.
uint8_t eb1008e60032[] = { 0xEB, 0x10, 0x08, 0xE6, 0x00, 0x32, 0xB4, 0x81 };  //Main info update from CP1 to IOT module.
uint8_t eb1008e60032_request[109];




#if defined(ARDUINO) && defined(__AVR__)
static PROGMEM const uint8_t table_crc_hi[] = {
#else
static const uint8_t table_crc_hi[] = {
#endif
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
  0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
  0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
  0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
  0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
  0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
  0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
  0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

#if defined(ARDUINO) && defined(__AVR__)
#include <avr/pgmspace.h>
static PROGMEM const uint8_t table_crc_lo[] = {
#else
static const uint8_t table_crc_lo[] = {
#endif
  0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
  0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
  0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
  0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
  0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
  0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
  0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
  0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
  0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
  0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
  0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
  0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
  0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
  0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
  0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
  0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
  0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
  0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
  0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
  0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
  0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
  0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
  0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
  0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
  0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
  0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};


void lockVariable() {
  xSemaphoreTake(msgSemaphore, portMAX_DELAY);
}

void unlockVariable() {
  xSemaphoreGive(msgSemaphore);
}




void setup() {
  Serial.begin(9600);
  Serial.println("OK");
  //Set Serial, Baud Rate, 8 bit no parity, RX,TX
  Serial1.begin(9600, SERIAL_8N1, SERIAL1_RX, SERIAL1_TX);
  Serial2.begin(9600, SERIAL_8N1, SERIAL2_RX, SERIAL2_TX);


  msgSemaphore = xSemaphoreCreateMutex();


  //Prefill messages used for webserver.
  memset(eb1008e60032_request, 0, 109);

  LanController::Setup();

  //Start the web server
  server.on("/", HTTP_GET, webRootResponse);
  server.on("/command", HTTP_POST, webCommandResponse);
  server.begin();

  //Run serial proxy on second core.
  xTaskCreatePinnedToCore(Core1Loop, "Core1Loop", 10000, NULL, 1, NULL, 1);
}

void loop() {
  server.handleClient();
}


void Core1Loop(void* parameter) {
  Serial.println("Serial relay started");
  while (true) {
    relaySerial(Serial1, Serial2, serial1buffer, serial1Index, s1previousMillis, 1);
    relaySerial(Serial2, Serial1, serial2buffer, serial2Index, s2previousMillis, 2);
    delay(1);  //Small delay to prevent CPU hogging.
  }
}

void relaySerial(HardwareSerial& inputSerial, HardwareSerial& outputSerial, uint8_t* buffer, int& index, unsigned long& previousMillis, int serialNumber) {

  while (inputSerial.available()) {
    // Relay byte.
    uint8_t incomingByte = inputSerial.read();
    outputSerial.write(incomingByte);

    // Gap Threshold of time between last byte, mark as message complete.
    unsigned long currentMillis = millis();
    if (index > 0 && currentMillis - previousMillis > GAPTHRESHOLD) {
      index = 0;
    }
    previousMillis = currentMillis;  //Save current time byte is received.

    if (index == MAXMSGSIZE) {
      if (SerialOutputModbus) {
        Serial.println("ERROR: Buffer limit reach - MAXMSGSIZE limit is a problem. Resync triggered.");
      }
      index = 0;
    }

    buffer[index++] = incomingByte;  //Store rx byte into buffer.

    if (ProcessMessage(buffer, index, serialNumber) == true) {
      index = 0;
    }
  }
}



void webRootResponse() {
  lockVariable();

  StaticJsonDocument<4096> hvacJson;
  hvacJson["module_name"] = "ESP32-HVAC-Control";
  hvacJson["uptime"] = getUptimeFormatted();
  hvacJson["system_power"] = SystemPowerInfo;
  hvacJson["system_mode"] = SystemMode;
  hvacJson["target_temp"] = TargetTempInfo;
  hvacJson["target_temp_zone2"] = TargetTemp2Info;
  hvacJson["evap_mode"] = EvapModeInfo;
  hvacJson["evap_fanspeed"] = EvapFanSpeedInfo;
  hvacJson["heater_mode"] = HeaterModeInfo;
  hvacJson["heater_fanspeed"] = HeaterFanSpeedInfo;
  hvacJson["heater_therm_temp"] = ThermisterTempInfo;
  hvacJson["heater_zone1_enabled"] = Zone1EnabledInfo;
  hvacJson["heater_zone2_enabled"] = Zone2EnabledInfo;
  hvacJson["zone1_temp_sensor"] = Zone1TempInfo;
  hvacJson["zone2_temp_sensor"] = Zone2TempInfo;
  hvacJson["panel_command_count"] = CommandInfo;
  hvacJson["automatic_clean_running"] = AutomaticCleanRunning;

  char eb1008char[109 * 4];
  eb1008char[0] = '\0';  // Initialize the string as empty
  for (size_t i = 0; i < 109; i++) {
    char buffer[4];
    sprintf(buffer, "%u", eb1008e60032_request[i]);
    strcat(eb1008char, buffer);
    if (i < 109 - 1) {
      strcat(eb1008char, ",");
    }
  }
  hvacJson["eb1008e60032_cp1data"] = eb1008char;


  unlockVariable();

  String jsonstring;
  serializeJsonPretty(hvacJson, jsonstring);
  server.send(200, "application/json", jsonstring);
}


void webCommandResponse() {
  if (server.hasArg("plain")) {         // Check if the body exists
    String body = server.arg("plain");  // Get the body of the POST request
    if (SerialOutputModbus) {
      Serial.println("Web request: " + body);
    }
    lockVariable();
    if (body.indexOf("power=on") != -1) {  // Check if "D1" is in the body
      SystemPower = true;
      if (SerialOutputModbus) {
        Serial.println("System Power set to True");
      }
    } else if (body.indexOf("power=off") != -1) {
      SystemPower = false;
      if (SerialOutputModbus) {
        Serial.println("System Power set to false");
      }

    } else if (body.indexOf("zone1=on") != -1) {
      HeaterZone1 = true;
      if (SerialOutputModbus) {
        Serial.println("Enabled Zone 1");
      }

    } else if (body.indexOf("zone1=off") != -1) {
      HeaterZone1 = false;
      if (SerialOutputModbus) {
        Serial.println("Disabled Zone 1");
      }
    } else if (body.indexOf("zone2=on") != -1) {
      HeaterZone2 = true;
      if (SerialOutputModbus) {
        Serial.println("Enabled Zone 2");
      }
    } else if (body.indexOf("zone2=off") != -1) {
      HeaterZone2 = false;
      if (SerialOutputModbus) {
        Serial.println("Disabled Zone 2");
      }

    } else if (body.indexOf("serial=on") != -1) {
      SerialOutputModbus = true;
      Serial.println("Serial Output Enabled");
    } else if (body.indexOf("serial=off") != -1) {
      SerialOutputModbus = false;
      Serial.println("Serial Output Disabled");
    } else if (body.startsWith("fanspeed=")) {
      int number = body.substring(9).toInt();
      if (number >= 1 && number <= 10) {
        FanSpeed = number;
      }
      if (SerialOutputModbus) {
        Serial.println("Fan speed set to " + String(FanSpeed));
      }

    } else if (body.startsWith("mode=")) {
      int number = body.substring(5).toInt();
      if (number >= 0 && number <= 5) {
        SystemMode = number;
      }
      if (SerialOutputModbus) {
        Serial.println("System mode set to " + String(SystemMode));
      }
    } else if (body.startsWith("temp=")) {
      int number = body.substring(5).toInt();
      if (number >= 0 && number <= 28) {
        TargetTemp = number;
      }
      if (SerialOutputModbus) {
        Serial.println("Target Temp set to " + String(TargetTemp));
      }
    } else if (body.startsWith("temp2=")) {
      int number = body.substring(6).toInt();
      if (number >= 0 && number <= 28) {
        TargetTemp2 = number;
      }
      if (SerialOutputModbus) {
        Serial.println("Target Temp set to " + String(TargetTemp2));
      }
    }
    sendCommand = true;  //Force command update
    unlockVariable();

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "No body received.");
    if (SerialOutputModbus) {
      Serial.println("Failed to send reply to command - no body received.");
    }
  }
}




bool ProcessMessage(uint8_t msgBuffer[], int msgLength, int SerialID) {
  if (msgLength < 4) {  //Min message length.
    return false;
  }
  uint16_t crcraw = (msgBuffer[msgLength - 2] << 8 | msgBuffer[msgLength - 1]);  //crc from message
  uint16_t expectedcrc = modbusCRC(msgBuffer, msgLength - 2);                    //calculated crc from message.
  if (crcraw == expectedcrc) {

    //Print message to serial port.
    if (SerialOutputModbus) {
      SerialPrintMessage(msgBuffer, msgLength, SerialID);
    }

    IoTModuleMessageProcess(msgBuffer, msgLength);  //Send confirmations for normal messages sent to IOT Module.

    EvapInfoUpdate(msgBuffer, msgLength);
    HeaterInfoUpdate(msgBuffer, msgLength);
    Panel1Info(msgBuffer, msgLength);
    Panel2Info(msgBuffer, msgLength);

    return true;
  }
  return false;
}


void EvapInfoUpdate(uint8_t* msgBuffer, int msgLength) {
  uint8_t evapInfoResponse[] = { 0x02, 0x03, 0x40 };
  if (checkPattern(msgBuffer, evapInfoResponse, 3)) {
    //Aircon unit info - Pump State, Active fan mode?
  }

  //Fan Speed
}

void HeaterInfoUpdate(uint8_t* msgBuffer, int msgLength) {
}


void Panel1Info(uint8_t* msgBuffer, int msgLength) {
  //On/Off.
  //Zone1 On/Off.
  //Temp Sensor
}

void Panel2Info(uint8_t* msgBuffer, int msgLength) {
  if (msgLength != 7) {
    return;
  }

  //Temp Sensor
  uint8_t panelInfoResponse[] = { 0x97, 0x03, 0x02 };
  if (checkPattern(msgBuffer, panelInfoResponse, 3)) {
    lockVariable();
    Zone2TempInfo = msgBuffer[3];
    unlockVariable();
  }
}

void SerialPrintMessage(uint8_t* msgBuffer, int msgLength, int SerialID) {
  Serial.print("S");
  Serial.print(SerialID);
  Serial.print(" ");
  for (int i = 0; i < (msgLength - 2); i++) {
    Serial.print(msgBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.print(msgBuffer[msgLength - 2], HEX);
  Serial.print(" ");
  Serial.print(msgBuffer[msgLength - 1], HEX);
  Serial.println();
}

static uint16_t modbusCRC(uint8_t* buffer, uint16_t buffer_length) {
  uint8_t crc_hi = 0xFF; /* high CRC byte initialized */
  uint8_t crc_lo = 0xFF; /* low CRC byte initialized */
  unsigned int i;        /* will index into CRC lookup */
  while (buffer_length--) {
    i = crc_hi ^ *buffer++; /* calculate the CRC  */
#if defined(ARDUINO) && defined(__AVR__)
    crc_hi = crc_lo ^ pgm_read_byte_near(table_crc_hi + i);
    crc_lo = pgm_read_byte_near(table_crc_lo + i);
#else
    crc_hi = crc_lo ^ table_crc_hi[i];
    crc_lo = table_crc_lo[i];
#endif
  }
  return (crc_hi << 8 | crc_lo);
}


uint8_t IOTModuleStatusRequest[] = { 0xEB, 0x03, 0x02, 0x58, 0x00, 0x12, 0x53, 0x66 };
void SendIOTModuleStatus() {
  uint8_t wifi = 0x00;  //WiFI Connected 00, Disconnected = 21
  uint8_t IOTModuleStatusResponse[] = { 0xEB,
                                        0x03, 0x24, 0x00, wifi, 0x52, 0x4D, 0x32, 0x48,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                        0x00, 0x00, 0x00, 0x00, 0x04, 0x00 };
  SendMessage(IOTModuleStatusResponse, 39, true);
}


void UpdateCommandMessage() {
  if (x4vl == 0x11 && SystemPower == true) {
    x4vl = 0x10;
    sendCommand = true;
    Serial.println("System Power turned on.");
  } else if (x4vl == 0x10 && SystemPower == false) {
    x4vl = 0x11;
    sendCommand = true;
    Serial.println("System Power turned off.");
  }

  uint8_t tmpCfanspeed = 0x41 + (FanSpeed * 2);
  uint8_t tmpHfanspeed = 0x21 + (FanSpeed * 2);


  if (SystemMode == 0)  //External Fan Mode.
  {
    SetXVal(&x5vl, 0x42);
    SetXVal(&x6vl, tmpCfanspeed);
    SetXVal(&x7vl, 0x04);
    SetXVal(&x8vl, 0x22);

  } else if (SystemMode == 1)  //Recycle Fan Mode
  {
    SetXVal(&x5vl, 0x02);
    SetXVal(&x6vl, tmpCfanspeed - 1);
    SetXVal(&x7vl, 0x04);
    SetXVal(&x8vl, tmpHfanspeed);

  } else if (SystemMode == 2)  //Cooler Mode Manual
  {
    SetXVal(&x5vl, 0x02);
    SetXVal(&x6vl, tmpCfanspeed);
    SetXVal(&x7vl, 0x04);
    SetXVal(&x8vl, 0x02);
  } else if (SystemMode == 3)  //Cooler Mode Auto (Temp) - not tested.
  {
    uint16_t tempVal = 0x2003 + (TargetTemp * 0x20);
    uint8_t highByte = (tempVal >> 8) & 0xFF;
    uint8_t lowByte = tempVal & 0xFF;

    SetXVal(&x5vl, highByte);
    SetXVal(&x6vl, lowByte);

    SetXVal(&x7vl, 0x04);
    SetXVal(&x8vl, 0x02);  //Disable heater.

    uint8_t zonetemp = 0x2 * TargetTemp;
    SetXVal(&x10v, zonetemp);
    SetXVal(&x12v, zonetemp);

    SetXVal(&x17v, 0);
    SetXVal(&x18v, TargetTemp);

  } else if (SystemMode == 4)  //Heater Mode
  {
    //Unsure if need to set cooler settings.
    //SetXVal(&x5vl, 0x02);
    //SetXVal(&x6vl, 0x42);

    //Heater setting
    uint16_t tempVal = 0x02C3 + (0x40 * TargetTemp);
    uint8_t highByte = (tempVal >> 8) & 0xFF;
    uint8_t lowByte = tempVal & 0xFF;
    SetXVal(&x7vl, highByte);
    SetXVal(&x8vl, lowByte);

    //delayed update occurs on IOT module.
    uint8_t zonetemp = 0x2 * TargetTemp;
    SetXVal(&x10v, zonetemp);
    SetXVal(&x12v, zonetemp);

    uint8_t tmpzoneval = 0x0;
    if (HeaterZone1) {
      tmpzoneval += 1;
    }
    if (HeaterZone2) {
      tmpzoneval += 2;
    }

    SetXVal(&x14v, tmpzoneval);
    SetXVal(&x17v, TargetTemp2);
    SetXVal(&x18v, TargetTemp);
  }
}

void SendCommandMessage() {

  UpdateCommandMessage();

  if (sendCommand) {
    Serial.println("Command Triggered from IOT Request");
    if (x2vl == 0xFF) {
      x2vl = 0x0;
    } else {
      x2vl++;
    }
    sendCommand = false;
  }

  uint8_t IOTCommandMessage[] = {
    0xEB, 0x03, 0x1A, 0x02, x2vl, x3vl, x4vl, x5vl,
    x6vl, x7vl, x8vl, 0x00, x10v, 0x00, x12v, 0x00,
    x14v, 0x00, 0x00, x17v, x18v, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00
  };  //length is 29
  SendMessage(IOTCommandMessage, 29, true);
}

void SetXVal(uint8_t* val, uint8_t newVal) {
  if (*val != newVal) {
    *val = newVal;
    sendCommand = true;
  }
}




void IoTModuleMessageProcess(uint8_t msgBuffer[], int msgLength) {
  if (msgLength < 8)  //Skip messages below required size for these functions..
  {
    return;
  }

  if (checkPatternConfirm(msgBuffer, eb1005d80023)) {
    return;
  } else if (checkPatternConfirm(msgBuffer, eb1005fb0021)) {
    return;
  } else if (checkPatternConfirm(msgBuffer, eb10061c002c)) {
    return;
  } else if (checkPatternConfirm(msgBuffer, eb1006480004)) {
    return;
  } else if (checkPatternConfirm(msgBuffer, eb10064C0038)) {
    return;
  } else if (checkPatternConfirm(msgBuffer, eb100684002a)) {
    return;
  } else if (checkPatternConfirm(msgBuffer, eb1006ae002a)) {
    return;
  } else if (checkPatternConfirm(msgBuffer, eb1006d80019)) {
    return;
  } else if (checkPatternConfirm(msgBuffer, eb1008e50001)) {
    //Get System Power State.
    uint16_t powermsgval = (msgBuffer[7] << 8 | msgBuffer[8]);
    SystemPowerInfo = powermsgval & 1;  //LSB = Power on state.
    powermsgval = powermsgval >> 4;     //Bitshift to the right for +1 per command update.

    //Check for Changes from panel.
    if (powermsgval != CommandInfo) {
      Serial.println("Settings change has occured from control panel");

      lockVariable();
      CommandInfo = powermsgval;
      SystemPower = (SystemPowerInfo == 1);

      HeaterZone1 = (Zone1EnabledInfo == 1);
      HeaterZone2 = (Zone2EnabledInfo == 1);
      TargetTemp = TargetTempInfo;
      TargetTemp2 = TargetTemp2Info;

      //System Mode.
      uint8_t evapmodetemp = EvapModeInfo & 0x0F;  //mask higher bits.
      if (evapmodetemp == 0x01) {
        //Cooler Mode - Off
        SystemMode = 2;
      } else if (evapmodetemp == 0x05) {
        //Cooler Mode - Manual
        SystemMode = 2;
      } else if (evapmodetemp == 0x07) {
        //Cooler Mode - Auto
        SystemMode = 3;
      } else if (evapmodetemp == 0x09) {
        //Fan - External
        SystemMode = 0;
      } else if (HeaterModeInfo == 0x01) {
        //Heater
        SystemMode = 4;
      } else if (HeaterModeInfo == 0x03) {
        //Fan - Recycle
        SystemMode = 1;
      }

      //Only update FanSpeed when using Fan Modes or Cooler Manual Mode.
      if (HeaterFanSpeedInfo > 0 && SystemMode == 1) {
        FanSpeed = HeaterFanSpeedInfo;
      } else if (SystemMode == 0 || SystemMode == 2) {
        FanSpeed = EvapFanSpeedInfo;
      }


      unlockVariable();

      //Update local variables for command message and disable send trigger.
      UpdateCommandMessage();
      sendCommand = false;
    }

    return;
  } else if (checkPatternConfirm(msgBuffer, eb1008e60032)) {

    lockVariable();  //Lock state, variables used by webserver request.

    if (msgLength == 109) {
      memcpy(eb1008e60032_request, msgBuffer, 109);
    }

    // (msgBuffer[13] >> 4); //Other nibble of Evap Unit info, unsure of usage.
    AutomaticCleanRunning = msgBuffer[11];
    EvapModeInfo = msgBuffer[12];
    EvapFanSpeedInfo = (msgBuffer[14] & 0x0F);  //mask higher nibble.
    HeaterModeInfo = msgBuffer[40];
    HeaterFanSpeedInfo = (msgBuffer[42] & 0x0F);
    TargetTempInfo = msgBuffer[44];  //Evap,Heater Temp Target
    TargetTemp2Info = msgBuffer[90];
    ThermisterTempInfo = msgBuffer[46];
    Zone1EnabledInfo = msgBuffer[80] & 1;         //Zone 1 Enabled (LSB)
    Zone2EnabledInfo = (msgBuffer[80] >> 1) & 1;  //Zone 2 Enabled (2nd LSB)
    //Zone2EnabledInfo = msgBuffer[82];  //Zone 2 Enabled
    Zone1TempInfo = msgBuffer[87];

    unlockVariable();
    //Zone2 captured from CP2 reports.
    return;
  }

  //Match initial request with CRC - 8 Bytes.
  if (checkPattern(msgBuffer, IOTModuleInfoRequest, 8)) {
    SendMessage(IOTModuleInfoResponse, 15, false);
    return;
  } else if (checkPattern(msgBuffer, IOTModuleStatusRequest, 8)) {
    SendIOTModuleStatus();
    return;
  } else if (checkPattern(msgBuffer, IOTModuleCommandRequest, 8)) {
    SendCommandMessage();
    return;
  }
}

bool checkPatternConfirm(uint8_t* msgBuffer, uint8_t* checkpattern) {
  //6 Data Bytes.
  for (int i = 0; i < 6; i++) {
    if (msgBuffer[i] != checkpattern[i]) {
      return false;
    }
  }
  SendMessage(msgBuffer, 8, false);
  return true;
}


bool checkPattern(uint8_t* msgBuffer, uint8_t* checkpattern, int checklength) {
  for (int i = 0; i < checklength; i++) {
    if (msgBuffer[i] != checkpattern[i]) {
      return false;
    }
  }
  return true;
}

void SendMessage(uint8_t* msgBuffer, int length, bool sendcrc) {
  if (SerialOutputModbus) {
    SerialPrintMessage(msgBuffer, length, 0);
  }
  for (uint8_t i = 0; i < length; i++) {
    Serial1.write(msgBuffer[i]);
    Serial2.write(msgBuffer[i]);
  }
  if (sendcrc) {
    uint16_t crcval = modbusCRC(msgBuffer, length);
    uint8_t highByte = (crcval >> 8) & 0xFF;
    uint8_t lowByte = crcval & 0xFF;
    Serial1.write(highByte);
    Serial2.write(highByte);
    Serial1.write(lowByte);
    Serial2.write(lowByte);
  }
}


String getUptimeFormatted() {
  uint64_t totalSeconds = esp_timer_get_time() / 1000000ULL;
  
  uint32_t hours = totalSeconds / 3600;
  uint32_t minutes = (totalSeconds % 3600) / 60;
  uint32_t seconds = totalSeconds % 60;
  
  char buffer[9]; // 8 characters for HH:MM:SS plus null terminator
  snprintf(buffer, sizeof(buffer), "%02lu:%02lu:%02lu", hours, minutes, seconds);
  
  return String(buffer);
}
