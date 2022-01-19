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
 *         File: device_settings.h
 *      Created: 2018-03-21
 *       Author: Geoffrey Ottoy
 *
 *  Description: Change to your desired settings.
 */

#ifndef __DEVICE_SETTINGS_H__
#define __DEVICE_SETTINGS_H__


// TODO: reorganise settings file

#define IWAST_VERSION "IWAST v2022_January_001\0"

/*****************************************************************************************
 * CONFIGURE THIS!
 *****************************************************************************************/

//#define MOTHERBOARD_ID				0x474F
#define LORA_MAX_PAYLOAD_SIZE		48
#define LORA_ACCUMULATE_THRESHOLD	30

/* LoRaWAN section ***********************************************************************/

/*
 * SET default data rate
 * A higher SF yields a longer air time and thus energy
 * but extends the range
 *
 * Options:
 * SF12_BW125
 * SF11_BW125
 * SF10_BW125
 * SF9_BW125
 * SF8_BW125
 * SF7_BW125
 *
 * Try changing the SF to see its impact on the energy consumption
 */
#define DEFAULT_DATA_RATE			SF11_BW125
#define ENABLE_ADR					true
#define CONFIRMED_MESSAGES_ALLOWED  true

/*****************************************************************************************
 * END OF CONFIGURABLE SECTION
 *****************************************************************************************/



/*****************************************************************************************
 * DO NOT CHANGE !
 *****************************************************************************************/

// Copy your settings here
/* 64-bit device ID */
#define LORAWAN_DEVICE_EUI			"4957415354200000" // last two bytes are set by the configurator tool
/* 64-bit application ID */
#define LORAWAN_APPLICATION_EUI     "4957415354444556" // fixed
/* 128-bit AES key */
#define LORAWAN_APPLICATION_KEY     "00000000000000000000000000000000" // key is set by the configurator tool
// Don't change
#define JOIN_MECHANISM				OTAA

#define EUI_LENGTH 16
#define KEY_LENGTH 32

#define LORA_INIT_MY_DEVICE                                         \
{	JOIN_MECHANISM,						/* Activation mechanism */	\
	DEFAULT_DATA_RATE,					/* Data Rate on start-up */ \
	(ENABLE_ADR && CONFIRMED_MESSAGES_ALLOWED),	/* Use ADR or not (bool) */ \
	LORAWAN_DEVICE_EUI,					/* Device EUI */            \
	LORAWAN_APPLICATION_EUI,			/* Application EUI */       \
	LORAWAN_APPLICATION_KEY,			/* Application key */       \
	"",									/* Device address */ 		\
	"",									/* Network session key*/ 	\
	"",									/* App session key*/ 		\
}

#endif /* __DEVICE_SETTINGS_H__ */