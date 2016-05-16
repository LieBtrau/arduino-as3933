#ifndef AS3933_H
#define AS3933_H

#include <SPI.h>

class As3933
{
public:
    As3933(SPIClass &spi, byte ss, byte dat);
    void begin();
    bool setCorrelator(bool bEnable);
    void calAntenna(long freq);
    bool calRcOscillatorSelf();
    void reset();
private:
    static const byte RC_CAL_OK=7;//R14,7
    static const byte EN_WPAT=1;//R1,1
    static const byte EN_XTAL=0;//R1,0
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
