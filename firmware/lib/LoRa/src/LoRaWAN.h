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

#ifndef __LORA_WAN_H__
#define __LORA_WAN_H__

#include <Arduino.h>
#include "rn2xx3.h"

#define LORA_UNCONFIMED				false
#define LORA_CONFIRMED				true

#define LORA_EUI_LENGTH				16
#define LORA_KEY_LENGTH				32
#define LORA_DEVICE_ADDRESS_LENGTH	8

#define RN2483_RESET_PIN            5 
#define RN2483_TX_PIN            	0 // RX of SAMD
#define RN2483_RX_PIN            	1 // TX of SAMD

#define RN2483_BAUD					57600
#define SerialRN					Serial1

#define LORA_LED					11

typedef enum lora_statuses{
	JOINED,
	SUCCESS,
	ERROR
} LoRaStatus_t;
// convert to
//  ||
//  \/
typedef enum lora_states{
	ACTIVE,
	MODEM_SLEEPING,
	FAIL
} LoRaState_t;

typedef enum activation_methods{
	OTAA,
	ABP
} LoRaActivationMethod_t;

// EU data rates
typedef enum data_rates{
	SF12_BW125 = 0,
	SF11_BW125 = 1,
	SF10_BW125 = 2,
	SF9_BW125 = 3,
	SF8_BW125 = 4,
	SF7_BW125 = 5
} LoRaDataRate_t;

typedef struct{
	LoRaActivationMethod_t activationMethod;
	LoRaDataRate_t dataRate;
	char deviceEUI[LORA_EUI_LENGTH+1];
	char applicationEUI[LORA_EUI_LENGTH+1];
	char applicationKey[LORA_KEY_LENGTH+1];
	char deviceAddress[LORA_DEVICE_ADDRESS_LENGTH+1];
	char networkSessionKey[LORA_KEY_LENGTH+1];
	char applicationSessionKey[LORA_KEY_LENGTH+1];
} LoRaSettings_t;

class LoRaWAN{
    public:
        LoRaWAN(void);

		void begin(LoRaSettings_t s);
		void join(void);
		void sendData(uint8_t * packet, uint8_t size);

		void sleep(void);
		void wake(void);

    private:
		LoRaSettings_t settings;
		LoRaState_t state;

		rn2xx3 * modem;

		String hexToString(uint8_t * bin, uint8_t binsz){
			char hex_str[] = "0123456789abcdef";
			uint8_t i;

			String result = "";
			result.reserve(binsz*2+1);

			for (i = 0; i < binsz; i++){
				result += hex_str[(bin[i] >> 4) & 0x0F];
				result += hex_str[(bin[i]     ) & 0x0F];
			}

			result+="\0";
			return result;
		};
};

extern LoRaWAN lora;

#endif /* __LORA_WAN_H__ */