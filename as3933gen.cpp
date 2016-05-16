#include "as3933gen.h"

static As3933Gen *activeGen = NULL;
static byte shiftData=3;
static byte rfperiodCtr=0;
static volatile byte bitShift=0x80;
static byte buf[2];
static bool bufFull[2]={false,false};
static byte index=0;


As3933Gen::As3933Gen()
{
    memset(buf,0x0,2);
}

void As3933Gen::begin()
{
#ifdef ARDUINO_AVR_PROTRINKET3
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
#endif
    activeGen=this;
}

bool As3933Gen::push(byte value)
{
    if(!bufFull[0])
    {
        bufFull[0]=true;
        buf[0]=value;
        return true;
    }
    if(!bufFull[1])
    {
        bufFull[1]=true;
        buf[1]=value;
        return true;
    }
    return false;
}


//void As3933Gen::update()
//{
//    //  if (!rfperiodCtr++)
//    //  {
//    //  }
//    //  rfperiodCtr &= 0x7; //todo, should be 0x1F // (#bits/RF period) - 1
//}



#ifdef ARDUINO_AVR_PROTRINKET3
ISR(TIMER1_OVF_vect)
{
    bitSet(PORTB,PB3);
    if (shiftData & bitShift)
    {
        bitSet(DDRB, DDB2);
    }
    else
    {
        bitClear(DDRB, DDB2);
    }
    bitShift >>= 1;
    if(!bitShift)
    {
        bufFull[index]=false;
        index ^= 0x01;
        if(bufFull[index])
        {
            bitShift=0x80;
            shiftData=buf[index];
        }
    }
    bitClear(PORTB,PB3);
}
#endif
