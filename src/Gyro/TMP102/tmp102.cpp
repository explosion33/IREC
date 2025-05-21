#include "mbed.h"
#include "func.h"
#include "tmp102.h"

/**
 * Constructor: Creates a new I2C instance for the TMP102 sensor.
 * @param SDA - Serial Data Line pin.
 * @param SCL - Serial Clock Line pin.
 * @param addr - I2C address of the TMP102 sensor.
 */
tmp102::tmp102(PinName SDA, PinName SCL, char addr){
    i2c = new I2C(SDA, SCL);
    tmp102::addr = addr;
    extendedMode = 0;
    polarity = 0;
    owned = true;
}

/**
 * Constructor: Uses an existing I2C instance.
 * @param i2c - Pointer to an existing I2C object.
 * @param addr - I2C address of the TMP102 sensor.
 */
tmp102::tmp102(I2C* i2c, char addr){
    tmp102::i2c = i2c;
    tmp102::addr = addr;
    extendedMode = 0;
    polarity = 0;
    owned = false;
}

/**
 * Destructor: Deletes the I2C object only if owned by this class instance.
 */
tmp102::~tmp102(){
    if (owned){ 
        delete i2c;
    }
}

/**
 * Reads `len` bytes from the TMP102 starting at `regaddr`.
 * @param regaddr - Register address to read from.
 * @param data - Buffer to store the read data.
 * @param len - Number of bytes to read.
 * @return Status of the I2C read operation.
 */
int tmp102::readData(char regaddr, char* data, uint8_t len) {
    i2c->write(addr, &regaddr, 1);
    return i2c->read(addr, data, len);
}

/**
 * Writes `len` bytes of data to the specified register.
 * @param regaddr - Register address to write to.
 * @param data - Data buffer (2 bytes expected).
 * @param len - Number of bytes to write (typically 2).
 * @return Status of the I2C write operation.
 */
int tmp102::writeData(char regaddr, char data[2], uint8_t len) {
    char buffer[3];
    buffer[0] = regaddr;
    buffer[1] = data[0];
    buffer[2] = data[1];
    return i2c->write(addr, buffer, 3);
}

/**
 * Retrieves the raw temperature from the TMP102 sensor.
 * Returns a 12-bit or 13-bit signed value depending on extendedMode.
 * @return Raw temperature value.
 */
int16_t tmp102::getTemp(){
    char temp[2];
    readData(TMP102_TEMP_REG, temp, 2);

    int16_t rawTemp;
    if (extendedMode){
        rawTemp = (static_cast<int16_t>(temp[0]) << 5) | (static_cast<int16_t>(temp[1]) >> 3);
        if (rawTemp & 0x1000) rawTemp |= 0xE000; // Sign-extend 13-bit
    } else {
        rawTemp = (static_cast<int16_t>(temp[0]) << 4) | (static_cast<int16_t>(temp[1]) >> 4);
        if (rawTemp & 0x0800) rawTemp |= 0xF000; // Sign-extend 12-bit
    }
    return rawTemp;
}

/**
 * Converts raw temperature data to Celsius.
 * @return Temperature in Celsius.
 */
float tmp102::getTempCelsius() {
    return getTemp() * 0.0625f;
}

/**
 * Converts temperature in Celsius to Fahrenheit.
 * @return Temperature in Fahrenheit.
 */
float tmp102::getTempFahrenheit() {
    return (getTempCelsius() * 1.8f) + 32.0f;
}

/**
 * Puts the TMP102 into shutdown mode (low power).
 */
void tmp102::shutDown(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] |= 0x01;
    writeData(TMP102_CONFIG, config, 2);
}

/**
 * Wakes the TMP102 from shutdown mode.
 */
void tmp102::turnOn(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] &= 0xFE;
    writeData(TMP102_CONFIG, config, 2);
}

/**
 * Sets the sensor to comparator mode.
 */
void tmp102::setComparator(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] &= 0xFD;
    writeData(TMP102_CONFIG, config, 2);
}

/**
 * Sets the sensor to interrupt mode.
 */
void tmp102::setInterrupt(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] |= 0x02;
    writeData(TMP102_CONFIG, config, 2);
}

/**
 * Sets the polarity of the alert pin.
 * @param polarity - 0 = active low, 1 = active high.
 */
void tmp102::setPolarity(int polarity){
    tmp102::polarity = polarity;
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    if (polarity){
        config[0] |= 0x04;
    } else {
        config[0] &= 0xFB;
    }
    writeData(TMP102_CONFIG, config, 2);
}

/**
 * Sets the fault queue value (number of faults before alert).
 * @param faults - Number of consecutive faults (0–3).
 */
void tmp102::setFaultQueue(char faults){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] = (config[0] & ~0x18) | ((faults & 0x03) << 3);
    writeData(TMP102_CONFIG, config, 2);
}

/**
 * Gets the current resolution setting of the sensor.
 * @return Resolution bits (2-bit value).
 */
uint8_t tmp102::getResolution(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    return static_cast<uint8_t>((config[0] >> 5) & 0x03);
}

/**
 * Performs a one-shot temperature conversion.
 * @return Latest temperature reading.
 */
int16_t tmp102::oneShot(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[0] |= 0x80;
    writeData(TMP102_CONFIG, config, 2);
    ThisThread::sleep_for(10ms);
    return getTemp();
}

/**
 * Sets the extended mode (13-bit readings).
 * @param EM - 1 for extended mode, 0 for normal mode.
 */
void tmp102::setEM(int EM){
    if (EM == extendedMode) return;
    extendedMode = EM;
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[1] ^= (1 << 4);
    writeData(TMP102_CONFIG, config, 2);
}

/**
 * Reads the alert status of the TMP102.
 * @return 1 if alert is active, 0 otherwise (respects polarity setting).
 */
int tmp102::readAlert(){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    return (config[1] & 0x20) ? polarity : !polarity;
}

/**
 * Sets the conversion rate of the TMP102.
 * @param rate - 2-bit value indicating rate (0–3).
 */
void tmp102::setConversion(char rate){
    char config[2];
    readData(TMP102_CONFIG, config, 2);
    config[1] = (config[1] & ~0xC0) | ((rate & 0x03) << 6);
    writeData(TMP102_CONFIG, config, 2);
}

/**
 * Sets the high temperature threshold for alerts.
 * @param high - Threshold in raw 12/13-bit format.
 */
void tmp102::setHigh(uint16_t high){
    char val[2];
    val[0] = static_cast<char>((high >> 8) & 0xFF);
    val[1] = static_cast<char>(high & 0xFF);
    writeData(TMP102_THIGH, val, 2);
}

/**
 * Sets the low temperature threshold for alerts.
 * @param low - Threshold in raw 12/13-bit format.
 */
void tmp102::setLow(uint16_t low){
    char val[2];
    val[0] = static_cast<char>((low >> 8) & 0xFF);
    val[1] = static_cast<char>(low & 0xFF);
    writeData(TMP102_TLOW, val, 2);
}

/**
 * Sensor setup function
 */
void tmp102::setup(){
}

/**
 * Issues a software reset to the TMP102.
 */
void tmp102::reset(){
    char reset = 0x06;
    i2c->write(0x00, &reset, 2);
}
