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
 *         File: RN2483.h
 *      Created: 2019-10-21
 *       Author: Geoffrey Ottoy
 *      Version: 0.1
 *
 *  Description: Implementation of the RN2483 class.
 *      Based on https://github.com/DRAMCO/EFM32-RN2483-LoRa-Node
 *
 *  License (T.B.D.)
 */

#ifndef __RN2483_MODEM_H__
#define __RN2483_MODEM_H__

#include <Arduino.h>
#include "LoRaWAN.h"

#define RN2483_COMMAND_BUFFER_SIZE  80
#define RN2483_RECEIVE_BUFFER_SIZE  64

#define RN2483_RESET_PIN            5 

#define RN2483_BAUD					57600
#define SerialRN					Serial1


typedef enum powerSettings{
    RN2483_POWER_14DBM = 1,
    RN2483_POWER_11DBM = 2,
    RN2483_POWER_8DBM = 3,
    RN2483_POWER_4DBM = 4,
    RN2483_POWER_2DBM = 5
} RN2483_Power_t;

typedef enum rn2483_statuses{
	MAC_OK,
	INVALID_PARAM,
	JOIN_ACCEPTED,
	JOIN_DENIED,
	NOT_JOINED,
	NO_FREE_CH,
	SILENT,
	FRAME_COUNTER_ERR_REJOIN_NEEDED,
	BUSY,
	MAC_PAUSED,
	INVALID_DATA_LEN,
	MAC_TX_OK,
	MAC_RX,
	MAC_ERR,
	RADIO_TX_OK,
	RADIO_ERR,
	UNKOWN_ERR,
	RN_TX_TIMEOUT,
	RN_RX_TIMEOUT,
	DATA_RETURNED
}RN2483_Status_t;

/* The RN2483 class ...
 */
class RN2483_Modem {
public:
    // Constructor
    RN2483_Modem(void);

    void init(void);
    
    void test(void);

	RN2483_Status_t macReset(void);
	RN2483_Status_t getHardwareEUI(void);
	RN2483_Status_t getSystemVersion(void);
	RN2483_Status_t getApplicationEUI(void);
	RN2483_Status_t setDeviceEUI(char * eui);
	RN2483_Status_t setApplicationEUI(char * eui);
	RN2483_Status_t setApplicationKey(char * key);
	RN2483_Status_t setApplicationSessionKey(char * key);
	RN2483_Status_t setNetworkSessionKey(char * key);
	RN2483_Status_t setDeviceAddress(char * address);
	RN2483_Status_t setOutputPower(uint8_t pwr);
	RN2483_Status_t disableAdaptiveDataRate(void);
	RN2483_Status_t disableAutomaticReplies(void);
	RN2483_Status_t setDataRate(uint8_t dr);
	RN2483_Status_t getDataRate(int8_t *dr);
	RN2483_Status_t enableAdaptiveDataRate(void);
	RN2483_Status_t setBatteryLevel(uint8_t battery);
	RN2483_Status_t joinOTAA(void);
	RN2483_Status_t joinABP(void);
	RN2483_Status_t setDefaultOperation(LoRaSettings_t settings);
	RN2483_Status_t setup(LoRaSettings_t settings);
	RN2483_Status_t setupOTAA(LoRaSettings_t settings);
	RN2483_Status_t setupABP(LoRaSettings_t settings);

	RN2483_Status_t transmitUnconfirmed(uint8_t * data, uint8_t payloadSize);

private:
    //void initSerial(void);
    //void resetModem(void);
    //void breakCondition(void);

	RN2483_Status_t processMacCommand(bool secondResponse=false);

    char commandBuffer[RN2483_COMMAND_BUFFER_SIZE];
    char receiveBuffer[RN2483_RECEIVE_BUFFER_SIZE];
	uint8_t bytesReceived;

protected:

};

extern RN2483_Modem rn2483;

#endif /* __RN2483_MODEM_H__ */
