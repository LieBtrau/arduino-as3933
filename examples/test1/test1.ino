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
        return;
    }
    unsigned long ant1freqOsc= asTag.antennaTuning(1,125000);
    if(ant1freqOsc<124000 || ant1freqOsc>126000)
    {
        Serial.println("Calibration for antenna 1 fails");
        return;
    }
    unsigned long ant2freqOsc= asTag.antennaTuning(2,125000);
    if(ant2freqOsc<124000 || ant2freqOsc>126000)
    {
        Serial.println("Calibration for antenna 2 fails");
        return;
    }
    unsigned long ant3freqOsc= asTag.antennaTuning(3,125000);
    if(ant3freqOsc<124000 || ant3freqOsc>126000)
    {
        Serial.println("Calibration for antenna 3 fails");
        return;
    }

    if(!asTag.doRcOscSelfCalib())
    {
        Serial.println("RC-oscillator not correctly calibrated.");
        return;
    }
    Serial.println("RC-oscillator OK");
    //Measure clock generator frequency on CL_DAT pin.
    //For 125KHz carrier frequency, the clock generator frequency will be 125KHz/4 = 31.25KHz,
    //See AS3933 datasheet p.49
    asTag.doOutputClockGeneratorFrequency(true);
}

// the loop function runs over and over again forever
void loop()
{
}
