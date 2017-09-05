/* AS3933 functionality
 * Connections:
 * AS3933       ProTrinket 3V
 * 1            10
 * 2            13
 * 3            11
 * 4            12
 * 5            3V
 * 6            GND
 * 15           5
 *
 * TODO: add https://github.com/PaulStoffregen/FreqCount as submodule.
 */
#ifndef AS3933_H
#define AS3933_H

#include <SPI.h>

class As3933
{
public:
    typedef enum
    {
        LM_STANDARD,
        LM_SCANNING,
        LM_ON_OFF
    }LISTENING_MODE;
    typedef enum
    {
        OO_1ms,
        OO_2ms,
        OO_4ms,
        OO_8ms
    }ON_OFF_TIME;
    typedef enum
    {
        FDT_NONE,
        FDT_SMALL,
        FDT_AVERAGE,
        FDT_BIG
    }FREQ_DET_TOL;
    typedef enum
    {
        AGC_DOWNONLY,
        AGC_UP_DOWN
    }AGC_MODE;
    typedef enum
    {
        GR_NONE,
        GR_MIN4DB,
        GR_MIN8DB,
        GR_MIN12DB,
        GR_MIN16DB,
        GR_MIN20DB,
        GR_MIN24DB
    }GAIN_REDUCTION;
    typedef enum
    {
        DR_NONE,
        DR_1K,
        DR_3K,
        DR_9K,
        DR_27K
    }DAMP_RESISTOR;
    typedef enum
    {
        SR_4096,
        SR_2184,
        SR_1490,
        SR_1130,
        SR_910,
        SR_762,
        SR_655,
        SR_512
    }SYMBOL_RATE;
    typedef enum
    {
        PR_800us,
        PR_1150us,
        PR_1550us,
        PR_1900us,
        PR_2300us,
        PR_2650us,
        PR_3000us,
        PR_3500us
    }PREAMBLE;
    typedef enum
    {
        WK_FREQ_DET_ONLY,
        WK_SINGLE_PATTERN
    }WAKEUP;
    As3933(SPIClass &spi, byte ss);
    bool begin(unsigned long freq);
    unsigned long antennaTuning(byte antennaNr);
    bool doRcOscSelfCalib();
    void doOutputClockGeneratorFrequency(bool bOutputEnabled);
    bool setNrOfActiveAntennas(byte number);
    bool setListeningMode(LISTENING_MODE lm);
    bool setListeningModeOnOffTime(ON_OFF_TIME oot);
    bool setFrequencyDetectionTolerance(FREQ_DET_TOL fdt);
    bool setAgc(AGC_MODE m, GAIN_REDUCTION gr);
    bool setAntennaDamper(DAMP_RESISTOR dr);
    bool setSymbolRate(SYMBOL_RATE sr);
    bool setPreambleLength(PREAMBLE pr);
    bool setBitDuration(byte rcRatio);
    bool setWakeUpProtocol(WAKEUP wk);
    bool setWakeUpPattern(byte* pattern16);
    void reset();
private:
    typedef enum
    {
        CLEAR_WAKE=0,
        RESET_RSSI=1,
        Calib_RCosc=2,
        clear_false=3,
        PRESET_DEFAULT=4,
        Calib_RCO_LC=5
    }DIRECT_CMD;
    void write(DIRECT_CMD directCmd);
    void write(byte reg, byte data);
    byte read(byte reg);
    bool setOperatingFrequencyRange();
    const byte CLOCK_GEN_DIS=7;//R16.7
    const byte BAND_SEL2=7;//R8.7
    const byte BAND_SEL1=6;//R8.6
    const byte BAND_SEL0=5;//R8.5
    const byte T_HBIT=0x1F;//R7<4:0>
    const byte RC_CAL_OK=7;//R14,7
    const byte T_OFF1=7;//R4.7
    const byte T_OFF0=6;//R4.6
    const byte D_RES_1=5;//R4.5
    const byte D_RES_0=4;//R4.4
    const byte GR_3=3;//R4.3
    const byte GR_2=2;//R4.2
    const byte GR_1=1;//R4.1
    const byte GR_0=0;//R4.0
    const byte FS_SCL_2=5;//R3.5
    const byte FS_SCL_1=4;//R3.4
    const byte FS_SCL_0=3;//R3.3
    const byte FS_ENV_2=2;//R3.2
    const byte FS_ENV_1=1;//R3.1
    const byte FS_ENV_0=0;//R3.0
    const byte DISPLAY_CLK=0x0C;//R2<3:2>
    const byte S_WU1_1=1;//R2.1
    const byte S_WU1_0=0;//R2.0
    const byte AGC_UD=5;//R1.5
    const byte ATT_ON=4;//R1.4
    const byte EN_PAT2=2;//R1.2
    const byte EN_WPAT=1;//R1,1
    const byte EN_XTAL=0;//R1,0
    const byte PAT32=7;//R0.7
    const byte ON_OFF=5;//R0,5
    const byte MUX_123=4;//R0,4
    const byte EN_A2=3;//R0,3
    const byte EN_A3=2;//R0,2
    const byte EN_A1=1;//R0,1
    SPIClass* _spi;
    SPISettings _spiSettings;
    byte _ss;
    byte _dat;
    unsigned long _freq;
    LISTENING_MODE _lm;
    bool _bCorrelatorEnabled;
};

#endif // AS3933_H
