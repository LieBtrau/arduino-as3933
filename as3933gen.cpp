#include "as3933gen.h"

static byte shiftData;
static byte rfperiodCtr;
static volatile byte bitShift;
//Pingpong buffer variables
static byte pingpongbuf[2];             //contains pingpong data
static bool bufFull[2]={false,false};   //elements become true when respective pingpong element contains data
static byte index;                      //index of the ping or pong element being active


As3933Gen::As3933Gen()
{
    shiftData=rfperiodCtr=index=0;
    bitShift=0x80;
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
    //Timer0 disable interrupts
    _timsk0=TIMSK0;
    TIMSK0=0;
    //Set clock source: No prescaling -> start timer
    bitSet(TCCR1B, CS10);
    bitClear(TCCR1B, CS11);
    bitClear(TCCR1B, CS12);
    sei();
#endif
}

void As3933Gen::end()
{
    bitClear(TIMSK1,TOIE1);
    TIMSK0=_timsk0;
}

bool As3933Gen::push(byte value)
{
    if(!bufFull[0])
    {
        bufFull[0]=true;
        pingpongbuf[0]=value;
        return true;
    }
    if(!bufFull[1])
    {
        bufFull[1]=true;
        pingpongbuf[1]=value;
        return true;
    }
    return false;
}


#ifdef ARDUINO_AVR_PROTRINKET3
ISR(TIMER1_OVF_vect)
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
        if(!bitShift)
        {
            //Byte completely shifted out.
            bufFull[index]=false;
            //Switch between ping & pong
            index ^= 0x01;
            //Shift out a new byte if available
            if(bufFull[index])
            {
                bitShift=0x80;
                shiftData=pingpongbuf[index];
            }
        }
    }
    rfperiodCtr &= 0x1F;// = (#bits per RF period) - 1
}
#endif
