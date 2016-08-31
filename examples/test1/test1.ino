/*
 */

//Includes for libraries must be done in *.ino file
#include "SPI.h"
#include <FreqCount.h>

//Includes needed for this file
#include <as3933.h>

As3933 asTag(SPI,10,11);


// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(9600);
    asTag.begin();
    asTag.calAntenna(125000);
    if(!asTag.calRcOscillatorSelf())
    {
        Serial.println("RC-oscillator not correctly calibrated.");
    }

}

// the loop function runs over and over again forever
void loop()
{
}
