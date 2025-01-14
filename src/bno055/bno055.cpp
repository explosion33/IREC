#include "mbed.h"
#include "BNO055.h"
#include <map>
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

        void wait(float time_ms) {
            ThisThread::sleep_for(time_ms); // todo: fix this 
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

        void runSelfTest(){
            char set;
            readData(BNO055_SYS_TRIGGER, &set, 1);
            set |= 0x01;
            writeData(BNO055_SYS_TRIGGER, &set, 1);
            wait(20);

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


        // read sensor data

};