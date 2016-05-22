//Includes for libraries must be done in *.ino file
#include "SPI.h"

//Includes needed for this file
#include <as3933gen.h>
#include <as3933.h>

byte pattern[]={0x12, 0x34};
As3933Gen as(pattern);

// the setup function runs once when you press reset or power the board
void setup()
{
    pinMode(11, OUTPUT);
    as.begin();
}

// the loop function runs over and over again forever
void loop()
{
//    if(!as.isBusy())
//    {
//        as.generate(a);
//        bitClear(PORTB,PB3);
//    }
}


