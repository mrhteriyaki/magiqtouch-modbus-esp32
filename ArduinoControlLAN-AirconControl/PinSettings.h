#if USING_WIRED_ETHERNET == 0

//Pin settings for regular ESP32 modules.

#define SERIAL1_RX 26
#define SERIAL1_TX 27
#define SERIAL2_RX 16
#define SERIAL2_TX 17

#elif USING_WIRED_ETHERNET == 1

//Pin settings for the Ethernet ESP - WT32-ETH01
#define SERIAL1_RX 5 //Pin 4 / RXD2
#define SERIAL1_TX 17 //Pin 5 / TXD2
#define SERIAL2_RX 14
#define SERIAL2_TX 15

#endif
