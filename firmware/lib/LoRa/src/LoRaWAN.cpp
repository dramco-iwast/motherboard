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
    // empty constructor
}

/* Initialize LoRaWAN modem according to LoRaSettings_t s.
 * This will only intialize the hardware. No communication yet.
 */
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

    // configure "lora communication indicator" led pin to output
    pinMode(LORA_LED, OUTPUT);
    // "lora communication indicator" led off
    digitalWrite(LORA_LED, HIGH);

    // currently, only ABP is supported, so check and block forever if OTAA is selected
    if(this->settings.activationMethod == OTAA){
        DEBUG.println("OTAA is not supported!");
        while(1){
            digitalWrite(LORA_LED, !digitalRead(LORA_LED));
            delay(500);
        }
    }

    // initialize serial port for communication with rn2483
    SerialRN.begin(RN2483_BAUD);

    // initialize uart for communication with the modem
    this->modem = new rn2xx3(SerialRN, RN2483_RESET_PIN, RN2483_RX_PIN, RN2483_TX_PIN);

    // hard reset (using rn2483 reset pin)
    this->modem->reset();
    delay(100); //wait for the RN2xx3's startup message
    SerialRN.flush();

    DEBUG.println("RN2xx3 firmware version:");
    DEBUG.println(this->modem->sysver());

    this->modem->setFrequencyPlan(TTN_EU);
}

/* Join a LoRaWAN network (ABP only)
 * LoRaWAN::begin() needs to be called first.
 */
void LoRaWAN::join(void){
    // "lora communication indicator" led on
    digitalWrite(LORA_LED, LOW);
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
    // "lora communication indicator" led off
    digitalWrite(LORA_LED, HIGH);
}

/* Send "raw data" bytes (in "packet" with length "size") over the LoRaWAN network
 * No resends on FAIL
 * Only unconfirmed messages
 */
void LoRaWAN::sendData(uint8_t * packet, uint8_t size){
    // "lora communication indicator" led on
    digitalWrite(LORA_LED, LOW);

    // transmit "raw bytes" (unconfirmed)
    TX_RETURN_TYPE rv = this->modem->txBytes(packet, size);
#ifdef SERIAL_DEBUG_OUTPUT
    switch (rv){
        case TX_FAIL:{
            DEBUG.println("tx fail");
        } break;

        case TX_SUCCESS:{
            DEBUG.println("tx success");
        } break;
    
        default:{
            DEBUG.println("tx rx");
        } break;
    }
#endif
    
    // "lora communication indicator" led off
    digitalWrite(LORA_LED, HIGH);
}

/* Put modem (hardware) in sleep to reduce power consumption
 * The duration is now 1 week, but we assume a forced wake-up within that period
 */
void LoRaWAN::sleep(){
    DEBUG.println("Lora SLEEP");
    this->modem->sleep(604800000); // sleep for a week, we will wake up earlier
}

/* Force the modem to wake up from sleep
 */
void LoRaWAN::wake(){
    DEBUG.print("Lora WAKE ");
    this->modem->autobaud();

    SerialRN.readStringUntil('\n');
    SerialRN.flush();
    DEBUG.println("success");
}