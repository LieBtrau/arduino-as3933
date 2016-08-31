#ifndef AS3933GEN
#define AS3933GEN

#include "Arduino.h"

//AS3933 data
//f(RC) = f(carr)/4 = 125K/4 = 31.25Khz
//Symbol rate
//  -(on EM4102 it's 64 RF periods per tagdata bit) = 1953 symbols/s -> 512us/symbol
//  -will depend on RLC-oscillation setup time
//  -MAX symbol rate = f(RC)/ R7<min> / bitspersymbol = 31.25KHz / 4 / 2 = 3.9K symbols/s
//  -MIN symbol rate = f(RC)/ R7<max> / bitspersymbol = 31.25KHz / 32 / 2 = 488 symbols/s
//Correlator: bit (=half symbol) duration = 32 RF periods = 8 RC periods
const byte RF_PERIOD_CTR=31; // = (#bits per RF period) - 1

#ifdef ARDUINO_AVR_PROTRINKET3FTDI || defined(ARDUINO_AVR_PROTRINKET3)
    //Set compare match register for 125KHz output frequency = F_CPU / 125K - 1 = 12M / 125K - 1 (Pro Trinket 3V)
    const byte OCR1AVAL=95;
#elif defined(ARDUINO_AVR_UNO)
    //Set compare match register for 125KHz output frequency = F_CPU / 125K - 1 = 16M / 125K - 1 (Pro Trinket 3V)
    const byte OCR1AVAL=127;
#else
    #error Your board is not supported yet
#endif
class As3933Gen
{
public:
  As3933Gen();
  void begin(byte* pattern16);
  void end();
  bool isTxDone();
private:
  void setData(byte* pattern16);
  bool push(byte value);
  byte _timsk0;
};
#endif
