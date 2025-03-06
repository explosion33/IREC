#include "encoder.h"
#include "mbed.h"

class encoder_i2c : public encoder {
    private:
        I2C* i2c;
        char addr;

        int readData(char regaddr, char* data, uint8_t len);
        int writeData(char regaddr, char data, uint8_t len);
        
    public:

}