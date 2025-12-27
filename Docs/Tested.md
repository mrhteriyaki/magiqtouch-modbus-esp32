
[Back to Readme](../README.md)

## Tested and working with:  
- Evaporative Cooler (Breezair EZQ155)  
- Gas Heater (Braemar TQ430N)  
- 2 Zones  
- 2 Wall Controllers running software revision R719.  
 

## Known Limitations  
- Refrigerated Cooling not tested or supported.  
- Zones 3 & 4 not implemented. (Values are likely next in order from existing zone data)  
- Zone 1 temp sensor does not report correctly on non-temperature target modes  
  (Incorrectly reports temperature on app when using Fan modes or Cooler mode by fan speed).  
  Temp reporting issue occur with official module and app, not a limitation of this project.  
- Has only been tested with controller version R719.  
- WiFi controller status on control panel does not reflect actual ESP WiFi signal strength.  
- This project does not support the RF 'Wireless' Magiqtouch Controller as they do not support the official WiFi module.  
- On rare occasion module reports Fan / Cooler mode incorrectly as change is not sent by Controller messages.  
 (Example: Mode is swapped from Fan to Cooler on control panel, WiFi module will still think mode is on external fan)  