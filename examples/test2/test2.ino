/* Output on pin 10
 */

//Includes for libraries must be done in *.ino file
#include "SPI.h"
#include <FreqCount.h>

//Includes needed for this file
#include <as3933gen.h>

byte pattern[]={0x12, 0x34};
As3933Gen as;


// the setup function runs once when you press reset or power the board
void setup()
{
}

// the loop function runs over and over again forever
void loop()
{
    as.begin(pattern);
    while(!as.isTxDone());
    as.end();
    delay(10);
}


