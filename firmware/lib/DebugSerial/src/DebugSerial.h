/*  ____  ____      _    __  __  ____ ___
 * |  _ \|  _ \    / \  |  \/  |/ ___/ _ \
 * | | | | |_) |  / _ \ | |\/| | |  | | | |
 * | |_| |  _ <  / ___ \| |  | | |__| |_| |
 * |____/|_| \_\/_/   \_\_|  |_|\____\___/
 *                           research group
 *                             dramco.be/
 *
 *  KU Leuven - Technology Campus Gent,
 *  Gebroeders De Smetstraat 1,
 *  B-9000 Gent, Belgium
 *
 *         File: DebugSerial.h
 *      Created: 2019-11-13
 *       Author: Geoffrey Ottoy
 *      Version: 0.1
 *
 *  Description: Serial Interface for debugging
 *
 *
 *  License (T.B.D.)
 */

#ifndef __DEBUG_SERIAL_H__
#define __DEBUG_SERIAL_H__

#include <Arduino.h>

#ifdef SERIAL_DEBUG_OUTPUT
#warning "Debug output enabled. Disable to save another 10 uA."
#endif

#define DEBUG_BAUD  115200

class DebugSerial{
    public:
        DebugSerial();
        void begin(unsigned long baud=DEBUG_BAUD);
        void flush(void);

        void printHexBuf(uint8_t * buf, uint8_t size, bool newLine=true){
            if(size){
                this->print("0x");
            }
            for(uint8_t i=0; i<size; i++){
                if(buf[i]<0x10){
                    this->print("0");
                }
                this->print(buf[i], HEX);
            }
            if(newLine){
                this->println();
            }
        }

        size_t print(const __FlashStringHelper *);
        size_t print(const String &);
        size_t print(const char[]);
        size_t print(char);
        size_t print(unsigned char, int = DEC);
        size_t print(int, int = DEC);
        size_t print(unsigned int, int = DEC);
        size_t print(long, int = DEC);
        size_t print(unsigned long, int = DEC);
        size_t print(double, int = 2);

        size_t println(const __FlashStringHelper *);
        size_t println(const String &s);
        size_t println(const char[]);
        size_t println(char);
        size_t println(unsigned char, int = DEC);
        size_t println(int, int = DEC);
        size_t println(unsigned int, int = DEC);
        size_t println(long, int = DEC);
        size_t println(unsigned long, int = DEC);
        size_t println(double, int = 2);
        size_t println(void);
};

extern DebugSerial DEBUG;

#endif /* __DEBUG_SERIAL_H__ */
