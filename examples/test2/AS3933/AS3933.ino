#include <as3933gen.h>
#include <as3933.h>

/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://www.arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */

As3933Gen as;
byte a=0;

// the setup function runs once when you press reset or power the board
void setup() 
{
  as.begin();
}

// the loop function runs over and over again forever
void loop() 
{
  if(!as.isBusy())
  {
    as.generate(a++);
  }
}


