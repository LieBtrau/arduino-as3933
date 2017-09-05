#include "as3933.h"
#include "FreqCount.h"

#define DEBUG 1

As3933::As3933(SPIClass &spi, byte ss): _spi(&spi), _ss(ss)
{

}

bool As3933::begin(unsigned long freq)
{
    FreqCount.begin(1000);
    pinMode(_ss, OUTPUT);
    digitalWrite(_ss, LOW);
    //SCLK low in idle mode, data sampled on falling edge of SCLK
    _spiSettings=SPISettings(2000000, MSBFIRST, SPI_MODE1);
    _spi->begin();
    reset();
    delay(300);
    //use default register values to check if communication is OK.
    //error in datasheet: default values of R5 and R6 are swapped
    byte r5=read(5);
    byte r6=read(6);
    //h4w
    if((r5!=0x69) || (r6!= 0x96))
    {
        Serial.println("Reading data from AS3933 fails: ");
        Serial.print("r5: "); Serial.println(r5, HEX);
        Serial.print("r6: "); Serial.println(r6, HEX);
        return false;
    }
    _freq=freq;
    if(!setOperatingFrequencyRange())
    {
        return false;
    }
    return true;
}

//Set number of RC-periods per bit
bool As3933::setBitDuration(byte rcRatio)
{
    if(--rcRatio>0x1F)
    {
        return false;
    }
    byte r7=read(7);
    r7&=0xE0;
    r7|=rcRatio;
    write(7,r7);
    return true;
}

//Set slow envelope detector
bool As3933::setPreambleLength(PREAMBLE pr)
{
    byte r3=read(3);
    switch(pr)
    {
    case PR_800us:
        bitClear(r3,FS_SCL_0);
        bitClear(r3,FS_SCL_1);
        bitClear(r3,FS_SCL_2);
        break;
    case PR_1150us:
        bitSet(r3,FS_SCL_0);
        bitClear(r3,FS_SCL_1);
        bitClear(r3,FS_SCL_2);
        break;
    case PR_1550us:
        bitClear(r3,FS_SCL_0);
        bitSet(r3,FS_SCL_1);
        bitClear(r3,FS_SCL_2);
        break;
    case PR_1900us:
        bitSet(r3,FS_SCL_0);
        bitSet(r3,FS_SCL_1);
        bitClear(r3,FS_SCL_2);
        break;
    case PR_2300us:
        bitClear(r3,FS_SCL_0);
        bitClear(r3,FS_SCL_1);
        bitSet(r3,FS_SCL_2);
        break;
    case PR_2650us:
        bitSet(r3,FS_SCL_0);
        bitClear(r3,FS_SCL_1);
        bitSet(r3,FS_SCL_2);
        break;
    case PR_3000us:
        bitClear(r3,FS_SCL_0);
        bitSet(r3,FS_SCL_1);
        bitSet(r3,FS_SCL_2);
        break;
    case PR_3500us:
        bitSet(r3,FS_SCL_0);
        bitSet(r3,FS_SCL_1);
        bitSet(r3,FS_SCL_2);
        break;
    default:
        return false;
    }
    write(3,r3);
    return true;
}

//Configure the fast envelope detector
bool As3933::setSymbolRate(SYMBOL_RATE sr)
{
    byte r3=read(3);
    switch (sr)
    {
    case SR_4096:
        bitClear(r3,FS_ENV_0);
        bitClear(r3,FS_ENV_1);
        bitClear(r3,FS_ENV_2);
        break;
    case SR_2184:
        bitSet(r3,FS_ENV_0);
        bitClear(r3,FS_ENV_1);
        bitClear(r3,FS_ENV_2);
        break;
    case SR_1490:
        bitClear(r3,FS_ENV_0);
        bitSet(r3,FS_ENV_1);
        bitClear(r3,FS_ENV_2);
        break;
    case SR_1130:
        bitSet(r3,FS_ENV_0);
        bitSet(r3,FS_ENV_1);
        bitClear(r3,FS_ENV_2);
        break;
    case SR_910:
        bitClear(r3,FS_ENV_0);
        bitClear(r3,FS_ENV_1);
        bitSet(r3,FS_ENV_2);
        break;
    case SR_762:
        bitSet(r3,FS_ENV_0);
        bitClear(r3,FS_ENV_1);
        bitSet(r3,FS_ENV_2);
        break;
    case SR_655:
        bitClear(r3,FS_ENV_0);
        bitSet(r3,FS_ENV_1);
        bitSet(r3,FS_ENV_2);
        break;
    case SR_512:
        bitSet(r3,FS_ENV_0);
        bitSet(r3,FS_ENV_1);
        bitSet(r3,FS_ENV_2);
        break;
    default:
        return false;
    }
    write(3,r3);
    return true;
}

bool As3933::setAntennaDamper(DAMP_RESISTOR dr)
{
    byte r1=read(1);
    byte r4=read(4);
    switch(dr)
    {
    case DR_NONE:
        bitClear(r1,ATT_ON);
        break;
    case DR_1K:
        bitSet(r1,ATT_ON);
        bitClear(r4,D_RES_0);
        bitClear(r4,D_RES_1);
        break;
    case DR_3K:
        bitSet(r1,ATT_ON);
        bitSet(r4,D_RES_0);
        bitClear(r4,D_RES_1);
        break;
    case DR_9K:
        bitSet(r1,ATT_ON);
        bitClear(r4,D_RES_0);
        bitSet(r4,D_RES_1);
        break;
    case DR_27K:
        bitSet(r1,ATT_ON);
        bitSet(r4,D_RES_0);
        bitSet(r4,D_RES_1);
        break;
    default:
        return false;
    }
    write(1,r1);
    write(4,r4);
    return true;
}

bool As3933::setFrequencyDetectionTolerance(FREQ_DET_TOL fdt)
{
    byte r2=read(2);
    switch(fdt)
    {
    case FDT_NONE:
        bitSet(r2,S_WU1_0);
        bitSet(r2,S_WU1_1);
        break;
    case FDT_SMALL:
        bitClear(r2,S_WU1_0);
        bitSet(r2,S_WU1_1);
        break;
    case FDT_AVERAGE:
        bitSet(r2,S_WU1_0);
        bitClear(r2,S_WU1_1);
        break;
    case FDT_BIG:
        bitClear(r2,S_WU1_0);
        bitClear(r2,S_WU1_1);
        break;
    default:
        return false;
    }
    write(2,r2);
    return true;
}

bool As3933::setAgc(AGC_MODE m, GAIN_REDUCTION gr)
{
    byte r1=read(1);
    switch(m)
    {
    case AGC_DOWNONLY:
        bitClear(r1,AGC_UD);
        break;
    case AGC_UP_DOWN:
        bitSet(r1,AGC_UD);
        break;
    default:
        return false;
    }
    write(1,r1);
    byte r4=read(4);
    switch(gr)
    {
    case GR_NONE:
        bitClear(r4, GR_0);
        bitClear(r4, GR_1);
        bitClear(r4, GR_2);
        bitClear(r4, GR_3);
        break;
    case GR_MIN4DB:
        bitClear(r4, GR_1);
        bitSet(r4, GR_2);
        bitClear(r4, GR_3);
        break;
    case GR_MIN8DB:
        bitSet(r4, GR_1);
        bitSet(r4, GR_2);
        bitClear(r4, GR_3);
        break;
    case GR_MIN12DB:
        bitClear(r4, GR_1);
        bitClear(r4, GR_2);
        bitSet(r4, GR_3);
        break;
    case GR_MIN16DB:
        bitSet(r4, GR_1);
        bitClear(r4, GR_2);
        bitSet(r4, GR_3);
        break;
    case GR_MIN20DB:
        bitClear(r4, GR_1);
        bitSet(r4, GR_2);
        bitSet(r4, GR_3);
        break;
    case GR_MIN24DB:
        bitSet(r4, GR_1);
        bitSet(r4, GR_2);
        bitSet(r4, GR_3);
        break;
    default:
        return false;
    }
    write(4,r4);
    return true;
}


/* Calibrate the antennas by adjusting the tuning caps of the AS3933.
 * The XTAL of the Arduino is used as a stable frequency reference.
 * See AS3933 datasheet p.53 and following.
 * \param antennaNr 1 to 3
 * \param freqSoll  in Hz: 125000 for 125KHz
 * \return  calibrated frequency
 */
unsigned long As3933::antennaTuning(byte antennaNr)
{
    byte r16=read(16);
    unsigned long freqIst;
    byte tuningVal=0;
#ifdef DEBUG
    Serial.print("Tuning antenna : LF");Serial.println(antennaNr, DEC);
#endif
    bitSet(r16,antennaNr-1);//enable LC-output on DAT-pin
    write(16, r16);
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
        if(freqIst < _freq)
        {
            bitClear(tuningVal, bit);
        }
    }
    bitClear(r16,antennaNr-1);
    write(16, r16);
    return freqIst;
}

/* In case the internal RC-oscillator will be used as clock generator, this function can be used to calibrate it, using
 * the LC-oscillation with LF1P.  So be sure to tune LF1P first.
 *
 * It is possible to display the frequency of the clock generator: "doOutputClockGeneratorFrequency(true);"
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

//It is possible to display the frequency of the clock generator on the CL_DAT pin writing R2<3:2>=11 and R16<7>=1.
void As3933::doOutputClockGeneratorFrequency(bool bOutputEnabled)
{
    byte r2=read(2);
    byte r16=read(16);
    if(bOutputEnabled)
    {
        r2|=DISPLAY_CLK;
        bitSet(r16, CLOCK_GEN_DIS);
    }else
    {
        r2&=~DISPLAY_CLK;
        bitClear(r16, CLOCK_GEN_DIS);
    }
    write(2,r2);
    write(16,r16);
}

//Set number of active antennas:
//number = 1 -> LF1P active
//number = 2 -> LF1P and LF3P active
//number = 3 -> LF1P, LF2P and LF3P active
bool As3933::setNrOfActiveAntennas(byte number)
{
    byte r0=read(0);
    switch(number)
    {
    case 1:
        bitSet(r0,EN_A1);
        bitClear(r0,EN_A2);
        bitClear(r0,EN_A3);
        break;
    case 2:
        bitSet(r0,EN_A1);
        bitClear(r0,EN_A2);
        bitSet(r0,EN_A3);
        break;
    case 3:
        bitSet(r0,EN_A1);
        bitSet(r0,EN_A2);
        bitSet(r0,EN_A3);
        break;
    default:
        return false;
    }
    write(0,r0);
    return true;
}

bool As3933::setListeningMode(As3933::LISTENING_MODE lm)
{
    byte r0=read(0);
    switch(lm)
    {
    case LM_STANDARD:
        bitClear(r0,LM_ON_OFF);
        bitClear(r0,MUX_123);
        break;
    case LM_SCANNING:
        bitClear(r0,LM_ON_OFF);
        bitSet(r0,MUX_123);
        break;
    case LM_ON_OFF:
        bitSet(r0,LM_ON_OFF);
        bitClear(r0,MUX_123);
        break;
    default:
        return false;
    }
    _lm=lm;
    write(0,r0);
    return true;
}

bool As3933::setListeningModeOnOffTime(As3933::ON_OFF_TIME oot)
{
    if(_lm!=LM_ON_OFF)
    {
        return false;
    }
    byte r4=read(4);
    switch(oot)
    {
    case OO_1ms:
        bitClear(r4,T_OFF0);
        bitClear(r4,T_OFF1);
        break;
    case OO_2ms:
        bitSet(r4,T_OFF0);
        bitClear(r4,T_OFF1);
        break;
    case OO_4ms:
        bitClear(r4,T_OFF0);
        bitSet(r4,T_OFF1);
        break;
    case OO_8ms:
        bitSet(r4,T_OFF0);
        bitSet(r4,T_OFF1);
        break;
    default:
        return false;
    }
    write(4,r4);
    return true;
}

bool As3933::setOperatingFrequencyRange()
{
    if(_freq<15000 || _freq>150000)
    {
        return false;
    }
    byte r8=read(8);
    if(_freq>=15000 && _freq<23000)
    {
        bitSet(r8,BAND_SEL0);
        bitSet(r8,BAND_SEL1);
        bitSet(r8,BAND_SEL2);
    }
    if(_freq>=23000 && _freq<40000)
    {
        bitSet(r8,BAND_SEL0);
        bitSet(r8,BAND_SEL1);
        bitClear(r8,BAND_SEL2);
    }
    if(_freq>=40000 && _freq<65000)
    {
        bitClear(r8,BAND_SEL0);
        bitSet(r8,BAND_SEL1);
        bitClear(r8,BAND_SEL2);
    }
    if(_freq>=65000 && _freq<95000)
    {
        bitSet(r8,BAND_SEL0);
        bitClear(r8,BAND_SEL1);
        bitClear(r8,BAND_SEL2);
    }
    if(_freq>=95000 && _freq<=150000)
    {
        bitClear(r8,BAND_SEL0);
        bitClear(r8,BAND_SEL1);
        bitClear(r8,BAND_SEL2);
    }
    write(8,r8);
    return true;
}

//Reset all AS3933 registers to default values
void As3933::reset()
{
    write(PRESET_DEFAULT);
    _bCorrelatorEnabled=true;
}

bool As3933::setWakeUpProtocol(WAKEUP wk)
{
    byte r1=read(0x1);
    switch(wk)
    {
    case WK_FREQ_DET_ONLY:
        bitClear(r1,EN_WPAT);
        bitClear(r1,EN_PAT2);
        _bCorrelatorEnabled=false;
        break;
    case WK_SINGLE_PATTERN:
        bitSet(r1,EN_WPAT);
        bitClear(r1,EN_PAT2);
        _bCorrelatorEnabled=true;
        break;
    default:
        return false;
    }
    write(0x1,r1);
    return true;
}

bool As3933::setWakeUpPattern(byte *pattern16)
{
    if(!_bCorrelatorEnabled)
    {
        return false;
    }
    //The pattern passed into this function is symbol representation.  It corresponds to 32bits in Manchester.
    byte r0=read(0);
    bitSet(r0, PAT32);
    write(r0);
    write(5, pattern16[0]);
    write(6, pattern16[1]);
    return true;
}

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

