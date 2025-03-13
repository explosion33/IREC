#ifndef TMP102_H
#define TMP102_H

#include "mbed.h"

#define TMP102_TEMP_REG  0x00
#define TMP102_CONFIG    0x01
#define TMP102_TLOW      0x02
#define TMP102_THIGH     0x03

class tmp102 {
public:
    // Constructor with I2C pin definitions
    tmp102(PinName SDA, PinName SCL, char addr = 0x90);
    
    // Constructor with an existing I2C instance
    tmp102(I2C* i2c, char addr = 0x90);

    // Destructor
    ~tmp102();

    // Read temperature as a raw 12 or 13-bit signed integer
    int16_t getTemp();

    // Get temperature in Celsius
    float getTempCelsius();

    // Get temperature in Fahrenheit
    float getTempFahrenheit();

    // Puts the device into low-power shutdown mode
    void shutDown();

    // Turns the device back on from shutdown mode
    void turnOn();

    // Sets comparator mode
    void setComparator();

    // Sets interrupt mode
    void setInterrupt();

    // Sets polarity (0: active low, 1: active high)
    void setPolarity(int polarity);

    // Sets the fault queue (number of consecutive faults before triggering alert)
    void setFaultQueue(char faults);

    // Gets the current resolution setting
    uint8_t getResolution();

    // Performs a one-shot temperature measurement
    int16_t oneShot();

    // Sets the extended mode (0: 12-bit, 1: 13-bit)
    void setEM(int EM);

    // Reads the alert pin status
    int readAlert();

    // Sets the conversion rate
    void setConversion(char rate);

    // Sets the high temperature threshold
    void setHigh(uint16_t high);

    // Sets the low temperature threshold
    void setLow(uint16_t low);

    // Placeholder for additional setup procedures
    void setup();

    // Resets the sensor
    void reset();

//private:
    I2C* i2c;           // Pointer to I2C instance
    char addr;          // I2C address of the TMP102
    int extendedMode;   // 0: 12-bit mode, 1: 13-bit mode
    int polarity;       // Alert polarity
    bool owned;         // Determines if the instance owns the I2C object

    // Helper function to read data from TMP102
    int readData(char regaddr, char* data, uint8_t len);

    // Helper function to write data to TMP102
    int writeData(char regaddr, char data[2], uint8_t len);
};

#endif // TMP102_H
