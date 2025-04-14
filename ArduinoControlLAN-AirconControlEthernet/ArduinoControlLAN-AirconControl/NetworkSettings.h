#ifndef ETHSEC_H
#define ETHSEC_H

//Testing with WT ESP32-ETH01 v1.4
//Flashing process is using the TX/RX/IOO/EN Pins at the top of the board.
//To program without full programmer, Ground EN and IOO, release EN then IOO.


#define SERIAL1_RX 5 //Pin 4 / RXD2
#define SERIAL1_TX 17 //Pin 5 / TXD2
#define SERIAL2_RX 14
#define SERIAL2_TX 15



class LanController {
public:
  static void Setup();
};

#endif