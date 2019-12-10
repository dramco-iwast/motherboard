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
#include "DebugSerial.h"

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

    SerialRN.begin(RN2483_BAUD);

    pinMode(LORA_LED, OUTPUT);      // configure "lora communication indicator" led pin to output
    digitalWrite(LORA_LED, HIGH);   // "lora communication indicator" led off

    // initialize uart for communication with the modem
    this->modem = new rn2xx3(SerialRN, RN2483_RESET_PIN, RN2483_RX_PIN, RN2483_TX_PIN);

    this->modem->reset();
    delay(100); //wait for the RN2xx3's startup message
    SerialRN.flush();

    DEBUG.println("RN2xx3 firmware version:");
    DEBUG.println(this->modem->sysver());

    this->modem->setFrequencyPlan(TTN_EU);
}

void LoRaWAN::join(void){
    digitalWrite(LORA_LED, LOW);    // "lora communication indicator" led on
    bool join_result;

    String addr = String(this->settings.deviceAddress);
    String AppSKey = String(this->settings.applicationSessionKey);
    String NwkSKey = String(this->settings.networkSessionKey);

    join_result = this->modem->initABP(addr, AppSKey, NwkSKey);

    while(!join_result){
        DEBUG.println("Unable to join. Are your keys correct, and do you have TTN coverage?");
        delay(60000); //delay a minute before retry
        join_result = this->modem->init();
    }

    // change data rate here (set to SF7 in initABP)
    this->modem->setDR(this->settings.dataRate);
    DEBUG.println("Successfully joined TTN");
    digitalWrite(LORA_LED, HIGH);   // "lora communication indicator" led off
}

void LoRaWAN::sendData(uint8_t * packet, uint8_t size){
    String payload = this->hexToString(packet, size);
    DEBUG.print("Sending data: ");
    DEBUG.println(payload);

    digitalWrite(LORA_LED, LOW);    // "lora communication indicator" led on
    this->modem->tx(payload);
    digitalWrite(LORA_LED, HIGH);   // "lora communication indicator" led off

    delete &payload;
}

void LoRaWAN::sleep(){
    this->modem->sleep(86400000); // sleep for a day, we will wake up earlier
}

void LoRaWAN::wake(){
    this->modem->autobaud();

    SerialRN.readStringUntil('\n');
    SerialRN.flush();
}