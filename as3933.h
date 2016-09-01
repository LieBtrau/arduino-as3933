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
 */
#ifndef AS3933_H
#define AS3933_H

#include <SPI.h>

class As3933
{
public:
    As3933(SPIClass &spi, byte ss);
    bool begin();
    bool setCorrelator(bool bEnable);
    unsigned long antennaTuning(byte antennaNr, unsigned long freqSoll);
    bool doRcOscSelfCalib();
    void doOutputClockGeneratorFrequency(bool bOutputEnabled);
    void reset();
private:
    const byte DISPLAY_CLK=0x0C;//R2<3:2>
    const byte CLOCK_GEN_DIS=7;//R16.7
    const byte RC_CAL_OK=7;//R14,7
    const byte EN_WPAT=1;//R1,1
    const byte EN_XTAL=0;//R1,0
    typedef enum
    {
        CLEAR_WAKE=0,
        RESET_RSSI=1,
        Calib_RCO_LC=5
    }DIRECT_CMD;
    SPIClass* _spi;
    SPISettings _spiSettings;
    byte _ss;
    byte _dat;
    void write(DIRECT_CMD directCmd);
    void write(byte reg, byte data);
    byte read(byte reg);
};

#endif // AS3933_H
