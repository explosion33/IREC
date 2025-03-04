#include "mbed.h"


#define TMP102_TEMP_REG 0x00
#define TMP102_CONFIG 0x01
#define TMP102_TLOW 0x02
#define TMP102_THIGH 0x03


class tmp102{
public:
    uint16_t getTemp();
    uint8_t getResolution();
    void setup();
    void reset();
    void shutDown();
    void turnOn();
    int readAlert();

    tmp102(PinName SDA, PinName SCL, char addr);
    tmp102(I2C* i2c, char addr);
    ~tmp102();
//private:
    I2C* i2c;
    char addr;
    int extendedMode;
    int polarity;
    bool owned;

    int readData(char regaddr, char* data, uint8_t len);
    int writeData(char regaddr, char data[2], uint8_t len);

    void setComparator();
    void setInterrupt();
    void setPolarity(int polarity);
    void setFaultQueue(char faults);
    uint16_t oneShot();
    void setEM(int EM);
    void setConversion(char rate);
    void setHigh(uint16_t high);
    void setLow(uint16_t low);
    
};