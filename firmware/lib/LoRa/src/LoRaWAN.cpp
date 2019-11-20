/*  ____  ____      _    __  __  ____ ___
 * |  _ \|  _ \    / \  |  \/  |/ ___/ _ \
 * | | | | |_) |  / _ \ | |\/| | |  | | | |
 * | |_| |  _ <  / ___ \| |  | | |__| |_| |
 * |____/|_| \_\/_/   \_\_|  |_|\____\___/
 *                           research group
 *                             dramco.be/
 *
 *   KU Leuven - Technology Campus Gent,
 *   Gebroeders De Smetstraat 1,
 *   B-9000 Gent, Belgium
 *
 *         File: LoraWAN.h
 *      Created: 2019-10-21
 *       Author: Geoffrey Ottoy
 *      Version: 0.1
 *
 *  Description: 
 *
 *  License (T.B.D.)
 */

#include "LoRaWAN.h"
#include "RN2483_Modem.h"

LoRaWAN lora;

LoRaWAN::LoRaWAN(void){

}

void LoRaWAN::begin(LoRaSettings_t s){
    // copy settings
    this->settings.activationMethod = s.activationMethod;
    this->settings.dataRate = s.dataRate;
    memcpy(this->settings.deviceAddress, s.deviceAddress, LORA_DEVICE_ADDRESS_LENGTH);
    this->settings.deviceAddress[LORA_DEVICE_ADDRESS_LENGTH] = '\0';
    memcpy(this->settings.deviceEUI, s.deviceEUI, LORA_EUI_LENGTH);
    this->settings.deviceEUI[LORA_EUI_LENGTH] = '\0';
    memcpy(this->settings.applicationEUI, s.applicationEUI, LORA_EUI_LENGTH);
    this->settings.applicationEUI[LORA_EUI_LENGTH] = '\0';
    memcpy(this->settings.applicationKey, s.applicationKey, LORA_KEY_LENGTH);
    this->settings.applicationKey[LORA_KEY_LENGTH] = '\0';
    memcpy(this->settings.networkSessionKey, s.networkSessionKey, LORA_KEY_LENGTH);
    this->settings.networkSessionKey[LORA_KEY_LENGTH] = '\0';
    memcpy(this->settings.applicationSessionKey, s.applicationSessionKey, LORA_KEY_LENGTH);
    this->settings.applicationSessionKey[LORA_KEY_LENGTH] = '\0';

    // initialize uart for communication with the modem
    rn2483.init();
}

void LoRaWAN::join(void){
    RN2483_Status_t result = rn2483.setupABP(this->settings);
    if(result != JOIN_ACCEPTED){
        this->status = ERROR;
    }
    else{
        this->status = JOINED;
    }

    rn2483.sleep(3600000);
}

void LoRaWAN::sendData(uint8_t * packet, uint8_t size){
    rn2483.breakCondition();

    if(this->status == JOINED){
        // send data
        rn2483.transmitUnconfirmed(packet, size);
    }
    else{
        // retry join
        rn2483.joinABP();

        // send data
        rn2483.transmitUnconfirmed(packet, size);
    }

    // sleep
    rn2483.sleep(3600000);
}