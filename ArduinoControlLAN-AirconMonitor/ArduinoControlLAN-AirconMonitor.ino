//Firebeetle ESP32 Aircon Interface

//Web Server with functions:
//Digital Pin Control Output - D2/D3.
//PWM - Not ready.
//Serial Relay

//Board Library esp32 by Espressif Systems in use.
//Update to major version 3 breaks serial2.
//2.0.17 tested working without serial issue.


//Libraries.
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

//Max message size for modbus, reduce due to ram usage.
#define MAXMSGSIZE 512
#define MAXSLAVEID 30
#define GAPTHRESHOLD 50

const char* ssid = "WiFiNetworkName";      // Change this to your WiFi SSID
const char* password = "WirelessPasskey123";  // Change this to your WiFi password

WebServer server(80);

SemaphoreHandle_t msgSemaphore;

unsigned long s1previousMillis = 0;
unsigned char serial1buffer[MAXMSGSIZE];
int serial1Index = 0;

unsigned long s2previousMillis = 0;
unsigned char serial2buffer[MAXMSGSIZE];
int serial2Index = 0;


class ResponseData {
public:

  unsigned char rList[MAXSLAVEID][MAXMSGSIZE];
  unsigned int rLength[MAXSLAVEID];
  unsigned char sIndex[MAXSLAVEID];
  int UsedSlaveId = -1;

  ResponseData() {
    for (int i = 0; i < MAXSLAVEID; i++) {
      rLength[i] = 0;
      sIndex[i] = 0;
    }
  }
};


ResponseData S1_3;
ResponseData S1_16;
ResponseData S2_3;
ResponseData S2_16;




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
  //Controller.Setup();

  Serial1.begin(9600);  //Pins 16/17
  Serial2.begin(9600);  //Pins 9/10


  msgSemaphore = xSemaphoreCreateMutex();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  //Start the web server
  server.on("/", HTTP_GET, webRootResponse);
  server.begin();



  //scheduler.addTask(webServerTask);
  //webServerTask.enable();

  //scheduler.addTask(serialTask);
  //serialTask.enable();

  //scheduler.addTask(serialTask2);
  //serialTask2.enable();


  // Set up a second core for the serial loop
  xTaskCreatePinnedToCore(
    Core1Loop,    // Function to implement the task
    "Core1Loop",  // Name of the task
    10000,        // Stack size in words
    NULL,         // Task input parameter
    1,            // Priority of the task
    NULL,         // Task handle.
    1             // Core where the task should run
  );
}

void loop() {

  runWebServer();
  delay(2);
}

unsigned long GAPTEST = 5000;
unsigned long testpMill = 0;

void Core1Loop(void* parameter) {
  Serial.println("Serial relay started");
  bool resyncS1 = true;
  bool resyncS2 = true;
  while (true) {
    //serialLoop();
    relaySerial(Serial1, Serial2, serial1buffer, serial1Index, s1previousMillis, 1, resyncS1);
    relaySerial(Serial2, Serial1, serial2buffer, serial2Index, s2previousMillis, 2, resyncS2);

    unsigned long currentMillis = millis();
    if (currentMillis - testpMill > GAPTEST) {
      testpMill = currentMillis;
    }

  }
}



void relaySerial(HardwareSerial& inputSerial, HardwareSerial& outputSerial, unsigned char* buffer, int& index, unsigned long& previousMillis, int serialNumber, bool& reSync) {

  while (inputSerial.available()) {
    // Relay byte.
    unsigned char incomingByte = inputSerial.read();
    outputSerial.write(incomingByte);

    // Gap Threshold of time between last byte, mark as message complete.
    unsigned long currentMillis = millis();
    //if (reSync == true) {
    if (index > 0 && currentMillis - previousMillis > GAPTHRESHOLD) {
      //Time between last byte recieved greater than gap threshold.
      //Serial.println("Data discarded - time limit without matching CRC. Bytes: " + String(index));
      Serial.print("E" + String(serialNumber) + " , ");
      for (int i = 0; i < index; i++) {
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
      }
      Serial.println();

      index = 0;
    }
    //}
    previousMillis = currentMillis;  //Save current time byte is received.


    if (index == MAXMSGSIZE) {
      Serial.println("ERROR: Buffer limit reach - MAXMSGSIZE limit is a problem. Resync triggered.");
      index = 0;
      reSync = true;
    }

    buffer[index++] = incomingByte;  //Store rx byte into buffer.

    if (ProcessMessage(buffer, index, serialNumber) == true) {
      reSync = false;
      index = 0;
    }
  }
}

String AddInfoString(ResponseData* RDATA, int SerialID, int FunctionID) {
  String pageContent = "<br>Serial " + String(SerialID) + "<br>";
  pageContent += "Function " + String(FunctionID) + " Responses:<br>";
  for (int i = 0; i <= RDATA->UsedSlaveId; i++) {
    char hexString[3];
    sprintf(hexString, "%02X", RDATA->sIndex[i]);
    pageContent += String(hexString);
    for (int index = 0; index < RDATA->rLength[i]; index++) {
      sprintf(hexString, "%02X", RDATA->rList[i][index]);
      pageContent += "," + String(hexString);
    }
    pageContent += "<br>";
  }
  return pageContent;
}


void webRootResponse() {
  lockVariable();
  String pageContent = "<html>";



  pageContent += AddInfoString(&S1_3, 1, 3);
  pageContent += AddInfoString(&S1_16, 1, 16);
  pageContent += AddInfoString(&S2_3, 2, 3);
  pageContent += AddInfoString(&S2_16, 2, 16);


  unlockVariable();

  pageContent += "</html>";

  //Serial.println("Sending web data");
  //Serial.println(pageContent);

  server.sendHeader("Content-Type", "text/html");
  server.send(200, "text/html", pageContent);
}

void runWebServer() {
  server.handleClient();
}



bool ProcessMessage(unsigned char msgBuffer[], int msgLength, int SerialID) {
  if (msgLength < 4) {  //Min message length.
    return false;
  }
  uint16_t crcraw = (msgBuffer[msgLength - 2] << 8 | msgBuffer[msgLength - 1]);  //crc from message
  uint16_t expectedcrc = modbusCRC(msgBuffer, msgLength - 2);                    //calculated crc from message.
  if (crcraw == expectedcrc) {
    //Valid message.


    //Print message to serial port.
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



    //Check if message is a request to IOT Module and send response.
    //if (msgBuffer[0] == 0xEB && msgBuffer[1] == 0x03 && msgBuffer[2] == 0x1A) {
    //}



    //return true, below code for webserver disabled.
    return true;

    //Copy data to responseList, exclude Slave ID and CRC.

    lockVariable();
    //SlaveID = msgBuffer[0]
    //Function = msgBuffer[1]
    if (msgLength > MAXMSGSIZE) {
      Serial.println("Alert - Message exceeding max message size limit has been ignored.");
    } else {

      if (SerialID == 1) {
        if (msgBuffer[1] == 3)  //Function 3
        {
          SaveMessage(&S1_3, msgBuffer, msgLength);
        } else if (msgBuffer[1] == 16)  //Function 16
        {
          SaveMessage(&S1_16, msgBuffer, msgLength);
        }
      } else if (SerialID == 2) {

        if (msgBuffer[1] == 3)  //Function 3
        {
          SaveMessage(&S2_3, msgBuffer, msgLength);
        } else if (msgBuffer[1] == 16)  //Function 16
        {
          SaveMessage(&S2_16, msgBuffer, msgLength);
        }
      }
    }
    unlockVariable();
    return true;
  }
  return false;
}

void SaveMessage(ResponseData* RPD, unsigned char* mBuf, int mLen) {
  unsigned char slaveId = mBuf[0];
  int slaveIndex = -1;
  for (int i = 0; i <= RPD->UsedSlaveId; i++) {
    if (RPD->sIndex[i] == slaveId)  //Find Slave ID in list.
    {
      slaveIndex = i;
      break;
    }
  }

  if (slaveIndex == -1) {
    if (RPD->UsedSlaveId == MAXSLAVEID) {
      Serial.println("Max Slave ID reached - increase MAXSLAVEID");
      return;
    }
    //Allocate SlaveIDIndex element to slave id.
    slaveIndex = ++RPD->UsedSlaveId;
    RPD->sIndex[slaveIndex] = slaveId;

    /*
    Serial.print("Serial index ");
    Serial.print(slaveIndex);
    Serial.print(" ");
    Serial.print(ident);
    Serial.print(" new slave id: ");
    Serial.println(slaveId);
    */
  }

  for (int i = 0; i < (mLen - 3); i++) {
    RPD->rList[slaveIndex][i] = mBuf[(i + 1)];
  }
  RPD->rLength[slaveIndex] = (mLen - 3);  //-2 for CRC, -1 for ID.
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