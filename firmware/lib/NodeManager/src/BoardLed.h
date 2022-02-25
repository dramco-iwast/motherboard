#ifndef __BOARD_LED_H__
#define __BOARD_LED_H__

#include <Arduino.h>

#define BOARD_LED_PIN   11

typedef enum ledActions{ON, OFF, TOGGLE, FLASH} LedAction_t;

class BoardLed {
public:
    // Constructor
    BoardLed(uint8_t pin=BOARD_LED_PIN);
    
    void init(void);
    void BoardLed_Action(LedAction_t action, uint8_t times = 1);

private:
    uint8_t _pin;
    bool _onVal;
};

#endif /*__BOARD_LED_H__*/
