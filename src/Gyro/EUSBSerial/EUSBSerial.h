/* EUSBSerial
 * Ethan Armstrong
 * warmst@uw.edu
 * -------------------------------------------
 * Extended USB Serial (EUSBSerial) is a class that extends
 * the functionality of the standard mbed USBSerial class
 * found in USBSerial.h. Features as follows:
 *     formatted print support (printf)
 *     newline terminated read support (readline)
 *     Redundant USB Connection monitoring and protection
 *
 * EUSBSerial implements QOL features while encapsulating
 * writing and reading functions within a seperate thread
 * This allows write and read operations to return an error
 * rather than block the main thread, or crash the STM
*/
#include "mbed.h"
#include "USBSerial.h"
#include <functional>

#ifndef MBED_CONF_EUSBSERIAL_MAX_PACKET_SIZE
#define MBED_CONF_EUSBSERIAL_MAX_PACKET_SIZE 1024
#endif

#ifndef _E_USB_SERIAL_H_
#define _E_USB_SERIAL_H_

class EUSBSerial {
public:
    EUSBSerial(uint16_t vid=0x1F00, uint16_t pid=0x2012);
    ~EUSBSerial();

    bool printf(const char* format, ...);
    bool write(const char* buf, size_t size);

    bool readline(char* buf, size_t size);

    size_t available();
    char _getc();

    bool connected();

private:
    const char* _format;
    va_list _args;
    size_t _size;
    bool _success;
    USBSerial pc;

    void _printf();
    void _write();
};

#endif //_E_USB_SERIAL_H_