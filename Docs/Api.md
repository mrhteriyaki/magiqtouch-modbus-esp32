[Back to Readme](../README.md)


## Web API Access and Control:

A GET Request to the module will return status info in JSON format.
Example of returned output.
```json
{
  "module_name": "ESP32-AIRCON",
  "uptime": "0d 00:05:11",
  "system_power": 0,
  "system_mode": 0,
  "target_temp": 20,
  "target_temp_zone2": 22,
  "evap_mode": 169,
  "evap_fanspeed": 0,
  "heater_mode": 0,
  "heater_fanspeed": 0,
  "heater_therm_temp": 16,
  "heater_zone1_enabled": 0,
  "heater_zone2_enabled": 0,
  "zone1_temp_sensor": 157,
  "zone2_temp_sensor": 20,
  "panel_command_count": 0,
  "automatic_clean_running": 0
}
```

Sending commands is simple using a POST request in plain/text to the path /command EG: http://192.168.20.112/command
The body should have a single command in the format setting=value

List of available commands, where x is the value.

| Command       | Available Values       | Function       |
|-----------------|----------------|----------------|
| power=x| on/off| System Power On or Off|
| zone1=x| on/off | Enable/Disable Zone 1 for Heater|
| zone2=x| on/off | Enable/Disable Zone 2 for Heater|
| fanspeed=x| 1-10 | Set Fan Speed for Fan, Cooler|
| mode=x| 0-4| Device Mode<br>0 = Fan Mode (External)<br>1 = Fan Mode (Recycle)<br>2 = Cooler (Manual)<br>3 = Cooler (Auto)<br>4 = Heater|
| temp=x| 0-28| Temperature Target for Cooler, Heater (Zone 1)
| temp2=x| 0-28| Temperature Target for Heater Zone 2|
| serial=x| on/off| Enables output of Modbus messages to Serial<br>Works with ModBusLogger tool on Serial message mode.|

Additional endpoints are:
| URI Path   | Function       |
|-----------|-----------------|
| /raw | Provides the raw output from the primary information modbus message sent from the panel to the IOT module. |
| /wifi | WiFi Configuration Setup Page |
| /network | Network information |
| /wifiupdate | Used by the wifi page to save WiFi settings to the device. |
| /clearsavedwifi | This will clear the saved WiFi settings and restore the default broadcasting AP. |
