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
 *         File: DebugSerial.cpp
 *      Created: 2019-11-13
 *       Author: Geoffrey Ottoy
 *      Version: 0.1
 *
 *  Description: Serial Interface for debugging
 *
 *
 *  License (T.B.D.)
 */

#include "DebugSerial.h"
#include "wiring_private.h"

DebugSerial DEBUG;

#ifdef SERIAL_DEBUG_OUTPUT
Uart _dbSerialSercom4(&sercom4, A2, A1, SERCOM_RX_PAD_1, UART_TX_PAD_0);
#endif

DebugSerial::DebugSerial(){
}

void DebugSerial::begin(unsigned long baud){
#ifdef SERIAL_DEBUG_OUTPUT
    _dbSerialSercom4.begin(baud);

    pinPeripheral(A2, PIO_SERCOM_ALT); //Assign RX function to pin 0
    pinPeripheral(A1, PIO_SERCOM_ALT); //Assign TX function to pin 1
#endif
}

void DebugSerial::flush(){
#ifdef SERIAL_DEBUG_OUTPUT
    _dbSerialSercom4.flush();
#endif
}

size_t DebugSerial::print(const __FlashStringHelper *ifsh){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(reinterpret_cast<const char *>(ifsh));
#else
    return -1;
#endif
}

size_t DebugSerial::print(const String &s){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(s);
#else
    return -1;
#endif
}

size_t DebugSerial::print(const char str[]){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(str);
#else
    return -1;
#endif
}

size_t DebugSerial::print(char c){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(c);
#else
    return -1;
#endif
}

size_t DebugSerial::print(unsigned char b, int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(b, base);
#else
    return -1;
#endif
}

size_t DebugSerial::print(int n, int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(n, base);
#else
    return -1;
#endif
}

size_t DebugSerial::print(unsigned int n, int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(n, base);
#else
    return -1;
#endif
}

size_t DebugSerial::print(long n , int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(n, base);
#else
    return -1;
#endif
}

size_t DebugSerial::print(unsigned long n, int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(n, base);
#else
    return -1;
#endif
}

size_t DebugSerial::print(double n, int digits){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.print(n, digits);
#else
    return -1;
#endif
}


size_t DebugSerial::println(const __FlashStringHelper *ifsh){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(reinterpret_cast<const char *>(ifsh));
#else
    return -1;
#endif
}

size_t DebugSerial::println(const String &s){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(s);
#else
    return -1;
#endif
}

size_t DebugSerial::println(const char str[]){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(str);
#else
    return -1;
#endif
}

size_t DebugSerial::println(char c){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(c);
#else
    return -1;
#endif
}

size_t DebugSerial::println(unsigned char b, int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(b, base);
#else
    return -1;
#endif
}

size_t DebugSerial::println(int n, int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(n, base);
#else
    return -1;
#endif
}

size_t DebugSerial::println(unsigned int n, int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(n, base);
#else
    return -1;
#endif
}

size_t DebugSerial::println(long n , int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(n, base);
#else
    return -1;
#endif
}

size_t DebugSerial::println(unsigned long n, int base){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(n, base);
#else
    return -1;
#endif
}

size_t DebugSerial::println(double n, int digits){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println(n, digits);
#else
    return -1;
#endif
}

size_t DebugSerial::println(void){
#ifdef SERIAL_DEBUG_OUTPUT
    return _dbSerialSercom4.println();
#else
    return -1;
#endif
}

#ifdef SERIAL_DEBUG_OUTPUT
// Attach the interrupt handler to the SERCOM
void SERCOM4_Handler(){
  _dbSerialSercom4.IrqHandler();
}
#endif