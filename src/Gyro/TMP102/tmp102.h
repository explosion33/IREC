#include "mbed.h"


#define TMP102_TEMP_REG = 0x00
#define TMP102_CONFIG = 0x01
#define TMP102_TLOW = 0x10
#define TMP102_THIGH = 0x11


class tmp102{
public:
    double getTemp();
    void getResolution();
    void setup();
    void reset();
    void shutDown();
    void readAlert();

private:
    I2C* i2c;
    char addr;

    tmp102(PinName SDA, PinName SCL, char addr);

    int readData(char regaddr, char* data, uint8_t len);
    int writeData(char regaddr, char data, uint8_t len);

    void setThermostat();
    void setPolarity();
    void setFaultQueue();
    void oneShot();
    void setEM();
    void setConversion();
    void setHigh();
    void setLow();
    
};