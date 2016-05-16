#include "as3933.h"

As3933::As3933(SPIClass &spi, byte ss, byte dat): _spi(&spi), _ss(ss), _dat(dat)
{

}

void As3933::begin()
{
    pinMode(_ss, OUTPUT);
    digitalWrite(_ss, LOW);
    _spiSettings=SPISettings(2000000, MSBFIRST, SPI_MODE1);
    _spi->begin();
    reset();
}

void As3933::calAntenna(long freq)
{
    /*
    foreach antenna
        //binary search algorithm
        for i=bit 4 downto 0
            set bit i
            enable LC-output on DAT pin
            measure frequency on DAT pin
            if frequency < freq
                clear bit i
    clear RSSI measurement
    clear WAKE state
//*/
    byte r16 = read(16);
    r16 &= 0xF8;
    write(16, r16);
    write(CLEAR_WAKE);
    write(RESET_RSSI);
}

bool As3933::calRcOscillatorSelf()
{
    byte r1 = read(1);
    bitClear(r1,EN_XTAL);
    write(1,r1);
    //It is possible to display the frequency of the clock generator on the CL_DAT pin writing R2<3:2>=11 and R16<7>=1.
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

