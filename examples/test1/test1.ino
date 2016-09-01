/*
 */

//Includes for libraries must be done in *.ino file
#include "SPI.h"
#include <FreqCount.h>

//Includes needed for this file
#include <as3933.h>

As3933 asTag(SPI,10);


// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(9600);
    Serial.println("test1: Checking AS3933 functionality");
    if(!asTag.begin())
    {
        Serial.println("Communication with AS3933 fails.");
    }
    unsigned long ant1freqOsc= asTag.antennaTuning(1,125000);
    if(ant1freqOsc<124000 || ant1freqOsc>126000)
    {
        Serial.println("Calibration for antenna 1 fails");
        return;
    }
    if(!asTag.doRcOscSelfCalib())
    {
        Serial.println("RC-oscillator not correctly calibrated.");
    }
    Serial.println("RC-oscillator OK");
}

// the loop function runs over and over again forever
void loop()
{
}
