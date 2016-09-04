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
    if(!asTag.begin(125000))
    {
        Serial.println("Communication with AS3933 fails.");
        return;
    }
    unsigned long ant1freqOsc= asTag.antennaTuning(1);
    if(ant1freqOsc<124000 || ant1freqOsc>126000)
    {
        Serial.println("Calibration for antenna 1 fails");
        return;
    }
//    unsigned long ant2freqOsc= asTag.antennaTuning(2);
//    if(ant2freqOsc<124000 || ant2freqOsc>126000)
//    {
//        Serial.println("Calibration for antenna 2 fails");
//        return;
//    }
//    unsigned long ant3freqOsc= asTag.antennaTuning(3);
//    if(ant3freqOsc<124000 || ant3freqOsc>126100)
//    {
//        Serial.println("Calibration for antenna 3 fails");
//        return;
//    }
    if(!asTag.doRcOscSelfCalib())
    {
        Serial.println("RC-oscillator not correctly calibrated.");
        return;
    }
    Serial.println("RC-oscillator OK");

    //Measure clock generator frequency on CL_DAT pin.
    //For 125KHz carrier frequency, the clock generator frequency will be 125KHz/4 = 31.25KHz,
    //See AS3933 datasheet p.49
    //asTag.doOutputClockGeneratorFrequency(true);

    if(!asTag.setNrOfActiveAntennas(1) ||
        !asTag.setListeningMode(As3933::LM_STANDARD) ||
        !asTag.setFrequencyDetectionTolerance(As3933::FDT_BIG) ||
        !asTag.setAgc(As3933::AGC_UP_DOWN, As3933::GR_NONE) ||
        !asTag.setAntennaDamper(As3933::DR_NONE))
    {
        return;
    }

    //Set symbol rate, must match with the RFID generator: 32 RF periods / bit, with f(RF)=125KHz
    //2 symbols/bit -> 64 RF periods / symbol -> symbol rate = f(RF) / 64 = 125K / 64 = 1953 symbols/s
    //There's no exact match, but 2184 symbols/s will do
    if(!asTag.setSymbolRate(As3933::SR_2184))
    {
        return;
    }
    //Set preamble length, must match with the RFID generator: 8bits = 8 * 32 RF periods = 8 * 32 * 8us = 2048us
    if(!asTag.setPreambleLength(As3933::PR_1900us))
    {
        return;
    }
    //Set bit duration in number of RC-periods
    //Bit duration = 32 RF periods = 8 RC periods
    //RC duration = 4 RF periods (see datasheet fig.56, f(RF)=125KHz
    if(!asTag.setBitDuration(8))
    {
        return;
    }
    //Set wake up protocol
    if(!asTag.setWakeUpProtocol(As3933::WK_SINGLE_PATTERN))
    {
        return;
    }
    byte pattern[]={0x12, 0x34};
    if(!asTag.setWakeUpPattern(pattern))
    {
        return;
    }
    Serial.println("Everything set up.");
}

// the loop function runs over and over again forever
void loop()
{
}
