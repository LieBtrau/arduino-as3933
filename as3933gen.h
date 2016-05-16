#ifndef AS3933GEN
#define AS3933GEN

#include "Arduino.h"

//AS3933 data
//f(RC) = f(carr)/4 = 125K/4 = 31.25Khz
//Frequency detector needs 2 * N periods of f(RC) = 8 periods of 31.25KHz = 256us
//Data slicer needs 32 * f(carr) periods = 32 periods of 125KHz = 256us
//Minimum carrier burst length in scanning mode = 80 RC periods + 16 RF periods = 2560us + 128us = 2688us
//Maximum carrier burst length = 155 RC periods = 4.96ms
//Symbol rate (on EM4102 it's 64 RF periods per tagdata bit) = 1953 bytes/s
//Minimum preamble length: if 3.5ms -> 438 RF periods
//Correlator: bit (=half symbol) duration = 32 RF periods = 8 RC periods

class As3933Gen
{
public:
  As3933Gen();
  void begin();
  void generate(byte data);
  bool isBusy();
  void update();
private:
};
#endif
