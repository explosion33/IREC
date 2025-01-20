#include "mbed.h"
#include "BNO055.h"
#include <map>
#include "func.h" 

DigitalOut rst(PA_5); // set reset pin
class BNO055 {
    private:
        I2C* i2c;
        bool owned;
    public: 
        BNO055(PinName SDA, PinName SCL){ // explicit constructor
            owned = true;
            this->i2c = new I2C(SDA, SCL);
        }

        BNO055(I2C* i2c){ // constructor using another i2c
            owned = false;
            this->i2c = i2c;
        }

        ~BNO055(){ // destructor
            if (owned)
                delete this->i2c;
        }
        
        int readData(uint8_t addr, char* data, uint8_t len){
            int err = i2c->read(addr, data, len);
            return err;
        }

        int writeData(uint8_t addr, char* data, uint8_t len){
            int err = i2c->write(addr, data, len);
            return err;
        }

        void setPWR(PWRMode mode) {
            char modeData = 0x00;

            switch(mode) {
                case PWRMode::Normal:
                    modeData = 0x00;
                    break;
                case PWRMode::LowPower:
                    modeData = 0x01;
                    break;
                case PWRMode::Suspend:
                    modeData = 0x02;
                    break;
            }

            writeData(BNO055_PWR_MODE, &modeData, 1);
        }

        char getOPMode(){
            setPage(0);
            char mode = 0;
            readData(BNO055_OPR_MODE, &mode, 1);
            return mode;
        }

        void setOPMode(char mode){
            setPage(0);
            writeData(BNO055_OPR_MODE, &mode, 1);
            if (mode == BNO055_OPERATION_MODE_CONFIG) {
                wait(19);
            } else {
                wait(7);
            }
        }

        void setACC(char GRange, char Bandwidth, char OPMode){
            setPage(0);
            char config = GRange | Bandwidth | OPMode;
            writeData(BNO055_ACC_CONFIG, &config, 1);
            wait(20);
        }

        void setGYR(char Range, char Bandwidth, char OPMode){
            setPage(0);
            char config0 = Range | Bandwidth;
            char config1 = OPMode;
            writeData(BNO055_GYRO_CONFIG_0, &config0, 1);
            wait(20);
            writeData(BNO055_GYRO_CONFIG_1, &config1, 1);
            wait(20);
        }

        void setMAG(char Rate, char OPMode, char Power){
            setPage(0);
            char config = Rate | OPMode | Power;
            writeData(BNO055_MAG_CONFIG, &config, 1);
            wait(20);
        }

        void setPage(uint8_t page) {
            char pageChar = static_cast<char>(page);
            writeData(BNO055_PAGE_ID, &pageChar, 1);
        }

        void setCLK(bool external = false){
            setPage(0);
            char tmp = 0x00;
            readData(BNO055_SYS_TRIGGER, &tmp, 1);
            tmp |= external ? 0x80 : 0x00;
            writeData(BNO055_SYS_TRIGGER, &tmp, 1);
            wait(700);
        }
        
        void setUnit(bool acc, bool angular, bool euler, bool temp, bool fusion){
            setPage(0);
            char config = 0x00;

            config |= acc;
            config |= (angular << 1);
            config |= (euler << 2);
            config |= (temp << 4);
            config |= (fusion << 7);
            writeData(BNO055_UNIT_SEL, &config, 1);
            wait(20);
        }

        void reset(){ // reset via sys_trigger
            char resetVal = 0x20;
            writeData(BNO055_SYS_TRIGGER, &resetVal, 1);
            wait(700);
        }

        void nReset(){ // reset via nreset pin
            rst = 0;
            wait(500);
            rst = 1;
            wait(500);
        }

        void setAxes(Axes newX, Axes newY, Axes newZ, bool xNeg = 0, bool yNeg = 0, bool zNeg = 0){ 
            // set axes using AXIS_MAP_CONFIG, AXIS_MAP_SIGN
            char axes = getAxes(newX, newY, newZ);
            writeData(BNO055_AXIS_MAP_CONFIG, &axes, 1);
            wait(20);
            
        }

        char getAxes(Axes newX, Axes newY, Axes newZ){
            map<Axes, char> axesMap = {{Axes::X, 0x00}, {Axes::Y, 0x01}, {Axes::Z, 0x02}};
            char axes = 0x00;
            axes |= axesMap[newX];
            axes |= axesMap[newY] << 2;
            axes |= axesMap[newZ] << 4;
            return axes;
        }

        char getAxesSign(bool xNeg, bool yNeg, bool zNeg){ // set xNeg, yNeg, zNeg true if you want to make axes sign negative
            char sign = 0x00;
            sign |= (xNeg) ? 0x01 : 0x00;
            sign |= (yNeg) ? 0x01 : 0x00;
            sign |= (zNeg) ? 0x01 : 0x00 ;
            return sign;
        }
        void setACCOffset(){
            return;
        }

        void setMAGOffset(){
            return;
        }

        void setGYRPOffset(){
            return;
        }

        void setRadius(){
            return;
        }

        void get_SysErr() {
            setPage(0);
            char err = 0x00;
            readData(BNO055_SYS_ERR, &err, 1);

            switch (err) {
                case 0x00:
                    printf("No error");
                    break;
                case 0x01:
                    printf("Peripheral initialization error");
                    break;
                case 0x02:
                    printf("System initialization error");
                    break;
                case 0x03:
                    printf("Self test result failed");
                    break;
                case 0x04:
                    printf("Register map value out of range");
                    break;
                case 0x05:
                    printf("Register map address out of range");
                    break;
                case 0x06:
                    printf("Register map write error");
                    break;
                case 0x07:
                    printf("BNO low power mode not available for selected operation mode");
                    break;
                case 0x08:
                    printf("Accelerometer power mode not available");
                    break;
                case 0x09:
                    printf("Fusion algorithm configuration error");
                    break;
                case 0x0A:
                    printf("Sensor configuration error");
                    break;
                default:
                    printf("Unknown error code: 0x%02X", (unsigned char)err);
                    break;
            }
            printf("\n");
        }

        void get_SysStatus() {
            setPage(0);
            char status = 0x00;

            readData(BNO055_SYS_STATUS, &status, 1);

            switch (status) {
                case 0x00:
                    printf("System idle");
                    break;
                case 0x01:
                    printf("System Error");
                    break;
                case 0x02:
                    printf("Initializing peripherals");
                    break;
                case 0x03:
                    printf("System Initialization");
                    break;
                case 0x04:
                    printf("Executing self test");
                    break;
                case 0x05:
                    printf("Sensor fusion algorithm running");
                    break;
                case 0x06:
                    printf("System running without fusion algorithm");
                    break;
                default:
                    printf("Unknown system status: 0x%02X", (unsigned char)status);
                    break;
            }
            printf("\n");
        }


        BNO055Result checkCalibration(){
            char calib_state = 0;
            readData(BNO055_CALIB_STAT, &calib_state, 1);
            
            if ((calib_state >> 6 & 0x03) != 0x03){
                return BNO055Result::SysErr;
            } else if ((calib_state >> 4 & 0x03) != 0x03){
                return BNO055Result::GyrErr;
            } else if ((calib_state >> 2 & 0x03) != 0x03){
                return BNO055Result::AccErr;
            } else if ((calib_state & 0x03) != 0x03){
                return BNO055Result::MagErr;
            } else {
                return BNO055Result::Ok;
            }
        }
        
        // todo: read calibration function


        void runSelfTest(){
            char set;
            readData(BNO055_SYS_TRIGGER, &set, 1);
            set |= 0x01;
            writeData(BNO055_SYS_TRIGGER, &set, 1);
            wait(20);

        }

        BNO055Result readSelfTest(){
            setPage(0);
            char res = 0;
            readData(BNO055_ST_RESULT, &res, 1);
            char sys = (res >> 3) & 0x01;
            char gyr = (res >> 2) & 0x01;
            char acc = (res >> 1) & 0x01;
            char mag = res & 0x01;
            if (sys != 1){
                return BNO055Result::SysErr;
            } else if (gyr != 1){
                return BNO055Result::GyrErr;
            } else if (acc != 1){
                return BNO055Result::AccErr;
            } else if (mag != 1){
                return BNO055Result::MagErr;
            } else {
                return BNO055Result::Ok;
            }
        }

        BNO055Result setup(){
            reset();

            // Find BNO055
            char id = 0;
            readData(BNO055_CHIP_ID, &id, 1);
            if (id != BNO055_ID){
                printf("Can't find BNO055");
                return BNO055Result::SysErr;
            }
            BNO055Result res = readSelfTest();
            if (res != BNO055Result::Ok){
                printf("POST Error");
                return res;
            }
            setPage(0);
            // Additional startup configuration can be added here
            

            return BNO055Result::Ok;
        }

        BNO055Result stop(){
            setPWR(PWRMode::Suspend);
            return BNO055Result::Ok;
        }

        bno055_vector_t bno055_getVector(std::uint8_t vec) {
            setPage(0);

            char buffer[8] = {0}; // quaternion needs 8

            if (vec == BNO055_VECTOR_QUATERNION) {
                readData(vec, buffer, 8);
            } else {
                readData(vec, buffer, 6);
            }

            double scale = 1.0;
            if (vec == BNO055_VECTOR_MAGNETOMETER) {
                scale = magScale;
            } 
            else if (vec == BNO055_VECTOR_ACCELEROMETER ||
                    vec == BNO055_VECTOR_LINEARACCEL ||
                    vec == BNO055_VECTOR_GRAVITY) {
                scale = accelScale;
            } 
            else if (vec == BNO055_VECTOR_GYROSCOPE) {
                scale = angularRateScale;
            } 
            else if (vec == BNO055_VECTOR_EULER) {
                scale = eulerScale;
            } 
            else if (vec == BNO055_VECTOR_QUATERNION) {
                scale = quaScale;
            }

            bno055_vector_t xyz{};
            xyz.w = 0.0;

            if (vec == BNO055_VECTOR_QUATERNION) {
                xyz.w = static_cast<int16_t>((buffer[1] << 8) | buffer[0]) / scale;
                xyz.x = static_cast<int16_t>((buffer[3] << 8) | buffer[2]) / scale;
                xyz.y = static_cast<int16_t>((buffer[5] << 8) | buffer[4]) / scale;
                xyz.z = static_cast<int16_t>((buffer[7] << 8) | buffer[6]) / scale;
            } else {
                xyz.x = static_cast<int16_t>((buffer[1] << 8) | buffer[0]) / scale;
                xyz.y = static_cast<int16_t>((buffer[3] << 8) | buffer[2]) / scale;
                xyz.z = static_cast<int16_t>((buffer[5] << 8) | buffer[4]) / scale;
            }

            return xyz;
        }

        bno055_vector_t bno055_getVectorAccelerometer() {
            return bno055_getVector(BNO055_VECTOR_ACCELEROMETER);
        }
        bno055_vector_t bno055_getVectorMagnetometer() {
            return bno055_getVector(BNO055_VECTOR_MAGNETOMETER);
        }
        bno055_vector_t bno055_getVectorGyroscope() {
            return bno055_getVector(BNO055_VECTOR_GYROSCOPE);
        }
        bno055_vector_t bno055_getVectorEuler() {
            return bno055_getVector(BNO055_VECTOR_EULER);
        }
        bno055_vector_t bno055_getVectorLinearAccel() {
            return bno055_getVector(BNO055_VECTOR_LINEARACCEL);
        }
        bno055_vector_t bno055_getVectorGravity() {
            return bno055_getVector(BNO055_VECTOR_GRAVITY);
        }
        bno055_vector_t bno055_getVectorQuaternion() {
            return bno055_getVector(BNO055_VECTOR_QUATERNION);
        }
};