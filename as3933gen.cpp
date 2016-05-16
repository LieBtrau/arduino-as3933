#include "as3933gen.h"

static As3933Gen *activeGen = NULL;
static byte shiftData;
static byte rfperiodCtr=0;
static volatile byte bitShift;

As3933Gen::As3933Gen()
{

}

void As3933Gen::begin()
{
  //Set OC1B pin as output (PORTB.2)
  pinMode(10, OUTPUT);
  //Waveform generation mode 15: Fast PWM with top at OCR1A
  bitSet(TCCR1B, WGM13);
  bitSet(TCCR1B, WGM12);
  bitSet(TCCR1A, WGM11);
  bitSet(TCCR1A, WGM10);
  //Set OC1B on Compare Match, clear OC1B at BOTTOM (inverting mode)
  bitSet(TCCR1A, COM1B1);
  bitSet(TCCR1A, COM1B0);
  //Set compare match register for 125KHz output frequency = F_CPU / 125K - 1 = 12M / 125K - 1 (Pro Trinket 3V)
  OCR1A=95;
  //Set duty cycle to 50%
  OCR1B=47;
  //Enable Timer1 overflow interrupt
  bitSet(TIMSK1, TOIE1);
  //Set clock source: No prescaling -> start timer
  bitSet(TCCR1B, CS10);
  bitClear(TCCR1B, CS11);
  bitClear(TCCR1B, CS12);
  sei();
  activeGen=this;
}

void As3933Gen::generate(byte data)
{
  shiftData=data;
  bitShift=0x80;
}

bool As3933Gen::isBusy()
{
    return bitShift;
}

void As3933Gen::update()
{
  if (!rfperiodCtr++)
  {
    if (shiftData & bitShift)
    {
      bitSet(DDRB, DDB2);
    }
    else
    {
      bitClear(DDRB, DDB2);
    }
    bitShift >>= 1;
  }
  rfperiodCtr &= 0x3; //todo, should be 0x1F // (#bits/RF period) - 1
}

ISR(TIMER1_OVF_vect)
{
  activeGen->update();
}
