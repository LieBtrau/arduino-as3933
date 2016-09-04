#include "as3933gen.h"

static byte shiftData=0;
static byte rfperiodCtr;
static volatile byte bitShift;
static byte dataHalfBits[7];
static const byte BYTE_COUNT=7;
static volatile byte bufIndex;                      //index of the element being active
static volatile byte isDone;

As3933Gen::As3933Gen(){}

void As3933Gen::begin(byte *pattern16)
{
    cli();
    setData(pattern16);
    shiftData=rfperiodCtr=bufIndex=isDone=0;
#ifdef ARDUINO_ARCH_AVR
    //Waveform generation mode 15: Fast PWM with top at OCR1A
    bitSet(TCCR1B, WGM13);
    bitSet(TCCR1B, WGM12);
    bitSet(TCCR1A, WGM11);
    bitSet(TCCR1A, WGM10);
    //Set OC1B on Compare Match, clear OC1B at BOTTOM (inverting mode)
    bitSet(TCCR1A, COM1B1);
    bitSet(TCCR1A, COM1B0);
    OCR1A=OCR1AVAL;
    //Set duty cycle to 50%
    OCR1B=OCR1AVAL >> 1;
    //Enable Timer1 overflow interrupt
    bitSet(TIMSK1, TOIE1);
    //Timer0 disable interrupts
    _timsk0=TIMSK0;
    TIMSK0=0;
    //Set clock source: No prescaling -> start timer
    bitSet(TCCR1B, CS10);
    bitClear(TCCR1B, CS11);
    bitClear(TCCR1B, CS12);
#endif
    sei();
}

void As3933Gen::end()
{
#ifdef ARDUINO_ARCH_AVR
    bitClear(TCCR1A, COM1B1);
    bitClear(TCCR1A, COM1B0);
    bitClear(TIMSK1,TOIE1);
    TIMSK0=_timsk0;
#endif
}

void As3933Gen::setData(byte* pattern16)
{
    //Carrier burst = unmodulated carrier
    //  Minimum carrier burst length in scanning mode (datasheet AS3933 fig.53) = 80 RC periods + 16 RF periods = 2560us + 128us = 2688us
    //  Maximum carrier burst length = 155 RC periods (datasheet AS3933 p.44) = 4960us
    //  => Carrier burst length = 3840us = 15 bits = 15*8 RC periods = 120 RC periods
    dataHalfBits[0]=0xFF;
    dataHalfBits[1]=0xFE;

    //Separation bit
    //  => 1bit = 0, implemented as last bit of carrier burst

    //Preamble = 100% modulated carrier 10101...010
    //  See datasheet AS3933, p.46
    //  Minimum preamble length = 6 bits (i.e. 101010)
    //  Maximum preample length = 14 bits
    //  => Preamble = 8bits
    dataHalfBits[2]=0xAA;

    //Pattern
    //Manchester encode 2byte value to 4byte array Manchester code:
    // "1" symbol -> 2 bits: HL
    // "0" symbol -> 2 bits: LH
    for(byte j=0;j<16;j++)
    {
        dataHalfBits[3 + ((j>>2)^1)] |= bitRead(pattern16[j>>3],j&7) ?  1<<(1+((j&3)<<1)) : 1<<((j&3)<<1);
    }
}

bool As3933Gen::isTxDone()
{
    return isDone==2;
}

#ifdef ARDUINO_ARCH_AVR
ISR(TIMER1_OVF_vect)
{
    if (!rfperiodCtr++)
    {
        if (shiftData & bitShift)
        {
            //bitSet(DDRB, DDB2);//too slow
            DDRB |= 0x04;   //enable output
        }
        else
        {
            //bitClear(DDRB, DDB2);//too slow
            DDRB &= 0xFB;   //set as input
        }
        bitShift >>= 1;
        if(!bitShift)
        {
            shiftData=dataHalfBits[bufIndex];
            if(bufIndex<BYTE_COUNT)
            {
                bufIndex++;
                bitShift=0x80;
            }else
            {
                isDone = isDone < 2 ? isDone+1 : 2;
            }
        }
    }
    rfperiodCtr &= RF_PERIOD_CTR;
}
#endif
