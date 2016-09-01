#include "as3933.h"
#include "FreqCount.h"

#define DEBUG 1

As3933::As3933(SPIClass &spi, byte ss): _spi(&spi), _ss(ss)
{

}

bool As3933::begin()
{
    FreqCount.begin(1000);
    pinMode(_ss, OUTPUT);
    digitalWrite(_ss, LOW);
    //SCLK low in idle mode, data sampled on falling edge of SCLK
    _spiSettings=SPISettings(2000000, MSBFIRST, SPI_MODE1);
    _spi->begin();
    reset();
    //use default register values to check if communication is OK.
    //error in datasheet: default values of R5 and R6 are swapped
    return (read(5) == 0x69) && (read(6) == 0x96);
}

/* Calibrate the antennas by adjusting the tuning caps of the AS3933.
 * The XTAL of the Arduino is used as a stable frequency reference.
 * See AS3933 datasheet p.53 and following.
 * \param antennaNr 1 to 3
 * \param freqSoll  in Hz: 125000 for 125KHz
 * \return  calibrated frequency
 */
unsigned long As3933::antennaTuning(byte antennaNr, unsigned long freqSoll)
{
    byte r16=0;
    unsigned long freqIst;
    byte tuningVal=0;

#ifdef DEBUG
    Serial.print("Tuning antenna : LF");Serial.println(antennaNr, DEC);
#endif
    bitSet(r16,antennaNr-1);//enable LC-output on DAT-pin
    for(char bit=4;bit>-1;bit--)
    {
#ifdef DEBUG
        Serial.print("Testing bit: ");Serial.println(bit, DEC);
#endif
        bitSet(tuningVal, bit);
        write(16+antennaNr,tuningVal);  //R17<4:0>, R18<4:0>, R19<4:0>: tuning cap values
        for(byte j=0;j<1;j++)
        {
            while(!FreqCount.available());
            freqIst=FreqCount.read();
#ifdef DEBUG
            Serial.print("\tFrequency: ");Serial.println(freqIst);
#endif
        }
        if(freqIst < freqSoll)
        {
            bitClear(tuningVal, bit);
        }
    }
    bitClear(r16,antennaNr-1);
    reset();
    return freqIst;
}

/* In case the internal RC-oscillator will be used as clock generator, this function can be used to calibrate it, using
 * the LC-oscillation with LF1P.  So be sure to tune LF1P first.
 *
 * It is possible to display the frequency of the clock generator on the CL_DAT pin writing R2<3:2>=11 and R16<7>=1.
 */
bool As3933::doRcOscSelfCalib()
{
    //Use RC-OSC as clock generator
    byte r1 = read(1);
    bitClear(r1,EN_XTAL);
    write(1,r1);
    write(Calib_RCO_LC);
    delay(5);
    byte r14=read(14);
    return bitRead(r14, RC_CAL_OK);
}

void As3933::reset()
{
    write(CLEAR_WAKE);
    write(RESET_RSSI);
}

bool As3933::setCorrelator(bool bEnable)
{
    byte a=read(0x1);
    if(bEnable)
    {
        bitSet(a,EN_WPAT);
    }else
    {
        bitClear(a,EN_WPAT);
    }
    write(0x1,a);
}

//Remark that the AS3933 datasheet swaps the definition of bits & symbols
//9 header bits         -> 9*2*32 = 576 periods -> 4.6ms -> set preamble length to 3.5ms
//2 symbols/bit
//32 periods/symbol     -> standard for EM4102, but it's hard to do on a slow AVR

void As3933::write(byte reg, byte data) {
    digitalWrite(_ss, HIGH);
    _spi->beginTransaction(_spiSettings);
    _spi->transfer(reg & 0x3F);
    _spi->transfer(data);
    _spi->endTransaction();
    digitalWrite(_ss, LOW);
}

void As3933::write(DIRECT_CMD directCmd)
{
    digitalWrite(_ss, HIGH);
    _spi->beginTransaction(_spiSettings);
    _spi->transfer(0xC0 | directCmd);
    _spi->endTransaction();
    digitalWrite(_ss, LOW);
}


byte As3933::read(byte reg) {
    byte retVal;
    digitalWrite(_ss, HIGH);
    _spi->beginTransaction(_spiSettings);
    _spi->transfer(reg | 0x40);
    retVal=_spi->transfer(0);
    _spi->endTransaction();
    digitalWrite(_ss, LOW);
    return retVal;
};

