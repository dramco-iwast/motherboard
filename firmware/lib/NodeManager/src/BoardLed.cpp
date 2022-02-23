#include "BoardLed.h"

BoardLed::BoardLed(uint8_t pin){
#ifdef BOARD_V1
    this->_onVal = false;
#else
    this->_onVal = true;
#endif
    this->_pin = pin;
}

void BoardLed::init(void){
    pinMode(this->_pin, OUTPUT);
    this->BoardLed_Action(OFF);
}

/**
 * @brief turn led on/off or flash a couple of times
 * 
 * @param mode 
 * @param times 
 */
void BoardLed::BoardLed_Action(LedAction_t mode, uint8_t times){
  switch(mode){
    case ON:{
        digitalWrite(this->_pin, this->_onVal);
    } break;
    case OFF:{
        digitalWrite(this->_pin, !(this->_onVal));
    }
    break;
    case TOGGLE:{
        digitalWrite(this->_pin, !digitalRead(this->_pin));
    } break;
    case FLASH:{
        for(uint8_t i=0;i<times;i++){
            digitalWrite(this->_pin, this->_onVal);
            delay(100);
            digitalWrite(this->_pin, !this->_onVal);
            delay(100);
        }
        digitalWrite(this->_pin, !this->_onVal);
    } break;
    default:{
        return;
    }
  }
}