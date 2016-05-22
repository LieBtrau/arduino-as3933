#ifndef AS3933GEN
#define AS3933GEN

#include "Arduino.h"
#include "RingBuffer.h"

//AS3933 data
//f(RC) = f(carr)/4 = 125K/4 = 31.25Khz
//Symbol rate
//  -(on EM4102 it's 64 RF periods per tagdata bit) = 1953 symbols/s -> 512us/symbol
//  -will depend on RLC-oscillation setup time
//  -MAX symbol rate = f(RC)/ R7<min> / bitspersymbol = 31.25KHz / 4 / 2 = 3.9K symbols/s
//  -MIN symbol rate = f(RC)/ R7<max> / bitspersymbol = 31.25KHz / 32 / 2 = 488 symbols/s
//Correlator: bit (=half symbol) duration = 32 RF periods = 8 RC periods

class As3933Gen
{
public:
  As3933Gen(byte* pattern16);
  void begin();
  void end();
  void send();
private:
  bool push(byte value);
  byte _timsk0;
};
#endif
