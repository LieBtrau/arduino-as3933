#ifndef AS3933GEN
#define AS3933GEN

#include "Arduino.h"
#include "RingBuffer.h"

//AS3933 data
//f(RC) = f(carr)/4 = 125K/4 = 31.25Khz
//Symbol rate (on EM4102 it's 64 RF periods per tagdata bit) = 1953 bytes/s
//Correlator: bit (=half symbol) duration = 32 RF periods = 8 RC periods

//Carrier burst = unmodulated carrier
//  Minimum carrier burst length in scanning mode = 80 RC periods + 16 RF periods = 2560us + 128us = 2688us = 10.5bits
//  Maximum carrier burst length = 155 RC periods = 4960us = 19.4bits
//  => Carrier burst = 16bits

//Separation bit
//  => 1bit

//Preamble = 100% modulated carrier 010101...01
//  Minimum preamble length = 6bits (i.e. 010101) = 6 * 32 * 8us = 1536us
//  Maximum preample length = 14bits = 14 * 32 * 8us = 3584us
//  => Preamble = 7bits (forms 8bits when prepending the separation bit)

//Pattern
//  R0<7> = 1 -> 32bit pattern (16symbols)

class As3933Gen
{
public:
  As3933Gen(byte* pattern16);
  void begin();
  void end();
  bool push(byte value);
private:
  byte _timsk0;
  byte _pattern[4];
};
#endif
