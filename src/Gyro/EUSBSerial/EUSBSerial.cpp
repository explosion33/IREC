#include "EUSBSerial.h"

//MBED_CONF_EUSBSERIAL_MAX_PACKET_SIZE


EUSBSerial::EUSBSerial(uint16_t vid, uint16_t pid) : pc(false, vid, pid) {

}

EUSBSerial::~EUSBSerial() {
    pc.disconnect();
}

bool EUSBSerial::printf(const char* format, ...) {
    if (!pc.connected())
        return false;
    
    Thread t1;
    Timer t;

    va_list args;

    va_start(args, format);
    this->_format = format;
    this->_args = args;
    va_end(args);

    t1.start([this]() { _printf(); });
    t.start();

    while (!this->_success) {
        if (t.read_ms() > 500) {
            pc.disconnect();
            return false;
        }
    }

    t1.join();

    return true;
}


void EUSBSerial::_printf() {
    this->_success = false;

    char buffer[MBED_CONF_EUSBSERIAL_MAX_PACKET_SIZE];

    size_t n = vsnprintf (buffer, MBED_CONF_EUSBSERIAL_MAX_PACKET_SIZE, _format, _args);
    
    if (n > 0 && n < MBED_CONF_EUSBSERIAL_MAX_PACKET_SIZE) {
        pc.write(buffer, n);
        this->_success = true;
    }
}


bool EUSBSerial::write(const char* buf, size_t size) {
    if (!pc.connected())
        return false;
    
    Thread t1;
    Timer t;

    this->_format = buf;
    this->_size = size;

    t1.start([this]() { _write(); });
    t.start();

    while (!this->_success) {
        if (t.read_ms() > 500) {
            pc.disconnect();
            return false;
        }
    }

    t1.join();
    return true;
}

void EUSBSerial::_write() {
    this->_success = false;

    pc.write(this->_format, this->_size);

    this->_success = true;
}

char EUSBSerial::_getc() {
    return pc._getc();
}

size_t EUSBSerial::available() {
    return pc.available();
}

bool EUSBSerial::connected() {
    return this->write("\n", 1);
}

bool EUSBSerial::readline(char* buf, size_t size) {
    Timer t;
    t.start();

    size_t i = 0;
    while (pc.available()) {
        if (i == size)
            return false;

        buf[i] = this->_getc();
        if (buf[i] == '\n') {
            buf[i] = 0;
            return true;
        }
        i ++;

        if (t.read_ms() > 500)
            return false;
    }

    return false;
}