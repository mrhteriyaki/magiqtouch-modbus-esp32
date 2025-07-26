[Back to Readme](../README.md)


## Web API Access and Control:

A GET Request to the module will return status info in JSON format.
Example of returned output.
```json
{
  "module_name": "ESP32-HVAC-Control",
  "uptime": "00:03:00",
  "system_power": 0,
  "system_mode": 0,
  "target_temp": 20,
  "target_temp_zone2": 13,
  "evap_mode": 169,
  "evap_fanspeed": 0,
  "heater_mode": 0,
  "heater_fanspeed": 0,
  "heater_therm_temp": 15,
  "heater_zone1_enabled": 0,
  "heater_zone2_enabled": 0,
  "zone1_temp_sensor": 157,
  "zone2_temp_sensor": 21,
  "panel_command_count": 0,
  "automatic_clean_running": 0,
  "eb1008e60032_cp1data": "235,16,8,230,0,50,100,0,0,0,199,0,169,1,64,0,60,0,0,0,0,0,2,0,2,0,15,0,52,5,241,20,126,0,0,0,0,0,0,0,0,12,0,0,20,0,15,2,205,1,116,0,0,0,0,0,0,0,0,0,0,4,4,4,4,4,4,0,4,0,0,0,80,0,80,0,0,0,0,0,0,0,0,0,0,0,0,157,20,21,13,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,146,43"
}
```

Sending commands is simple using a POST request in plain/text to the path /command EG: http://192.168.20.112/command
The body should have a single command in the format setting=value

List of available commands, where x is the value.

| Command       | Available Values       | Purpose       |
|-----------------|----------------|----------------|
| power=x| on/off| System Power On or Off|
| zone1=x| on/off | Enable/Disable Zone 1 for Heater|
| zone2=x| on/off | Enable/Disable Zone 2 for Heater|
| fanspeed=x| 1-10 | Set Fan Speed for Fan, Cooler|
| mode=x| 0-4| Device Mode<br>0 = Fan Mode (External)<br>1 = Fan Mode (Recycle)<br>2 = Cooler (Manual)<br>3 = Cooler (Auto)<br>4 = Heater|
| temp=x| 0-28| Temperature Target for Cooler, Heater (Zone 1)
| temp2=x| 0-28| Temperature Target for Heater Zone 2|
| serial=x| on/off| Enables output of Modbus messages to Serial<br>Works with ModBusLogger tool on Serial message mode.|

