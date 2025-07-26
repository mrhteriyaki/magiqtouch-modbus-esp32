# MagIQTouch Modbus

This project is an alternative to the official WiFi Module for the MagiqTouch system.  
It provides local access and control with a simple [REST API](docs/Api.md).

### Home Assistant Integration: [https://github.com/mrhteriyaki/magiqtouch-modbus-esp32-ha](https://github.com/mrhteriyaki/magiqtouch-modbus-esp32-ha)

### Tested Configuration and Limitations: [Link](docs/Tested.md)

### Hardware List: [Link](docs/HardwareList.md)

### API Documentation: [Link](docs/Api.md)

### Configuration Steps:
- Download and Install [Arduino IDE](https://www.arduino.cc/en/software/)
- Download the folder from this repository ArduinoControlLAN-AirconControl  
(For wired ethernet esp32 boards use ArduinoControlLAN-AirconControlEthernet)
- Under Tools, Board manager install the package "esp32 by Espressif Systems"  
- Set your WiFi Network name and password you want the device to connect with in the NetworkSettings.h file. (Not required for Wired Ethernet boards)
- Set the board type in Tools -> Board -> esp32 -> Board Type   
eg: FireBeetle-ESP32 for the DF Robot Firebeetle or ESP32 Dev Module for most generic boards
- Upload the code to the ESP32 (Sketch -> Upload).
- Reset the device once complete and check serial output for the IP address assigned by the DHCP Server for successful link.  
 Ideally reserve this DHCP Lease on your router / DHCP Server so the IP does not change.
- Browse to the IP in your browser and confirm a JSON formatted response is displayed similar to [this example](docs/Api.md).
- Wire the hardware as per diagram below (Note Ethernet boards use alternate pins)  

![diagram](Images/diagram.png)

### For ethernet boards:
IO5  / RX2 -> RS485 Module 1 TX  
IO17 / TX2 -> RS485 Module 1 RX  
IO14 -> RS485 Module 2 TX  
IO15 -> RS485 Module 2 RX  


## RS485 to RJ45 Connector Wiring
The cables from the control panel and control board use a 6P6C male connector so that would be ideal.  
However I did not have one available in testing so for this will use a regular 8P8C Wall jack.

| RS485 Module | RJ45 Wire (A Wiring) |
|--------------|-----------|
| 5V (Control Panel Port Only) | Solid Green and Solid Orange |
| GND | Brown/White and Blue/White. |
| RS485 A | Orange/White |
| RS485 B | Solid Blue |

The standard 8P8C / RJ45 female ethernet wall jack can be used to connect to the cables to the device.  
The connector to the system board only requires A,B serial wires and ground.  
The connector to the control panel requires A,B, GND and 5V to power the panel.  

The 5v power should also be connected to the 5V Pin on the ESP32, to power both RS485 modules and one RJ45 ports for the control panel.  
The pinout on the rear of an RJ45 jack can vary with A/B positions, so make sure to match up the colours not positions in the image below. 

![RJ45](Images/rj45.PNG)


Once you have wired up your device prepared, you should be able to connect it inbetween the heater unit control board and primary control panel.

If you are not replacing an existing WiFi module, you'll need to add it as a module into the system using these steps.  
[Instructions for New Module](docs/NewModule.md)

Here is my mess wired up inside a 3d printed housing, STL files for 3d printing available in the Housing-CAD-STL folder.  
(Image reflects older wiring with 9/10 pins, has been moved to use alternate pins since photo was taken.)  

![insidebox](Images/inside_box.jpg)

Mounted inside the return air wallspace:  
![mounted](Images/mounted.jpg)


## More Technical Notes:

### Control Panel Wiring / Pin Information: [Link](docs/ControllerWiring.md)

### MODBUS Information [Link](docs/Modbus.md)




