#include "as3933gen.h"

static byte shiftData=0;
static byte rfperiodCtr;
static volatile byte bitShift;
static byte dataBuf[7];
static const byte BYTE_COUNT=7;
static byte bufIndex;                      //index of the element being active


As3933Gen::As3933Gen(byte *pattern16)
{
    //Carrier burst = unmodulated carrier
    //  Minimum carrier burst length in scanning mode = 80 RC periods + 16 RF periods = 2560us + 128us = 2688us
    //  Maximum carrier burst length = 155 RC periods = 4960us
    dataBuf[0]=0xFF;
    dataBuf[1]=0xFE;
    //    //Separation bit
    //    //  => 1bit

    //Preamble = 100% modulated carrier 10101...010
    //  Minimum preamble length = 6bits (i.e. 101010)
    //  Maximum preample length = 14bits
    //  => Preamble = 8bits (forms 8bits when prepending the separation bit)
    dataBuf[2]=0xAA;
    //Pattern
    //Manchester encode 2byte value to 4byte array Manchester code
    for(byte j=0;j<16;j++)
    {
        dataBuf[3 + ((j>>2)^1)] |= bitRead(pattern16[j>>3],j&7) ?  1<<(1+((j&3)<<1)) : 1<<((j&3)<<1);
    }
}

void As3933Gen::begin()
{
    shiftData=rfperiodCtr=bufIndex=0;
#ifdef ARDUINO_AVR_PROTRINKET3
    //Set OC1B pin as output (PORTB.2)
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
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
#ifdef ARDUINO_AVR_PROTRINKET3
    bitClear(TIMSK1,TOIE1);
    TIMSK0=_timsk0;
#endif
}

#ifdef ARDUINO_AVR_PROTRINKET3
ISR(TIMER1_OVF_vect)
{
//    bitSet(PORTB, DDB3);
    if (!rfperiodCtr++)
    {
        if (shiftData & bitShift)
        {
            //bitSet(DDRB, DDB2);
            DDRB |= 0x04;
        }
        else
        {
            //bitClear(DDRB, DDB2);
            DDRB &= 0xFB;
        }
        bitShift >>= 1;
        if(!bitShift)
        {
            shiftData=dataBuf[bufIndex];
            if(bufIndex<BYTE_COUNT)
            {
                bufIndex++;
                bitShift=0x80;
            }
        }
    }
    rfperiodCtr &= 0x1F;// = (#bits per RF period) - 1
//    bitClear(PORTB, DDB3);
}
#endif
