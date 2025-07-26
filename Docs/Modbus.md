[Back to Readme](../README.md)

## MODBUS:
The [Modbus Codes.xlsx](../Modbus%20Codes.xlsx) file contains information about the modbus messages used on the network.
Messages are sent in pairs with a request and response message.
The document formats these by the Slave ID, Function Code, Request, Response with notes about the information found in these message pairs.

All devices are on the same single network, the primary / zone 1 control panel is the authorative device that sends the requests, all other devices respond.
Data that is requested from the control panel is typically sent with function 0x03 (read request) and information sent (write request) is with 0x10.

Known Slave IDs:
|Slave ID| Related Purpose|
|-----------------|----------------|
|0x01 | Unknown|
|0x02 | Evap Unit|
|0x8C (140)| Unknown|
|0x8D (141)| Heater Unit|
|0x97 (151)| Control Panel 2|
|0xA5 (165)| Unknown|
|0xD4 (212)| Zone Controller by elimination � may also be shared heater|
|0xEB (235)| IOT WiFi Module|
|0xF4 (244)| Unknown|
|0xFA (250)| Unknown|
|0xFE (254)| Unknown|
