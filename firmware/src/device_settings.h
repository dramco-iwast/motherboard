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

/*****************************************************************************************
 * CONFIGURE THIS!
 *****************************************************************************************/

#define MOTHERBOARD_ID				0x474F
#define LORA_MAX_PAYLOAD_SIZE		48
#define LORA_ACCUMULATE_THRESHOLD	30

/* LoRaWAN section ***********************************************************************/

/* OVER_THE_AIR_ACTIVATION:
 * 	- set to 1 to use the Over-the-Air activation procedure
 * 	- set to 0 to use the Personalization activation procedure
 */
#define OVER_THE_AIR_ACTIVATION		0

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

#if OVER_THE_AIR_ACTIVATION == 1
// Copy your settings here
/* 64-bit device ID */
#define LORAWAN_DEVICE_EUI			"YOUR_DEVICE_EUI_"
/* 64-bit application ID */
#define LORAWAN_APPLICATION_EUI     "YOUR_APP_EUI____"
/* 128-bit AES key */
#define LORAWAN_APPLICATION_KEY     "YOUR_SECRET_AES_APPLICATION_KEY_"
// Don't change
#define JOIN_MECHANISM				OTAA

#elif OVER_THE_AIR_ACTIVATION == 0
// Copy your settings here
/* 32-bit device address */
#define LORAWAN_DEVICE_ADDRESS     	"YOUR_DEVICE_ADDRESS_"
/* 128-bit AES key */
#define LORAWAN_NWKSKEY             "YOUR_SECRET_AES_NTW_SESSION_KEY_"
/* 128-bit AES key */
#define LORAWAN_APPSKEY             "YOUR_SECRET_AES_APP_SESSION_KEY_"
// Don't change
#define JOIN_MECHANISM				ABP
#else
#error "OVER_THE_AIR_ACTIVATION has the wrong value"
#endif
/*****************************************************************************************
 * END OF CONFIGURABLE SECTION
 *****************************************************************************************/



/*****************************************************************************************
 * DO NOT CHANGE !
 *****************************************************************************************/
#if OVER_THE_AIR_ACTIVATION == 1
#define LORA_INIT_MY_DEVICE                                         \
{	JOIN_MECHANISM,						/* Activation mechanism */	\
	DEFAULT_DATA_RATE,					/* Data Rate on start-up */ \
	ENABLE_ADR,							/* Use ADR or not (bool) */ \
	LORAWAN_DEVICE_EUI,					/* Device EUI */            \
	LORAWAN_APPLICATION_EUI,			/* Application EUI */       \
	LORAWAN_APPLICATION_KEY,			/* Application key */       \
	"",									/* Device address */ 		\
	"",									/* Network session key*/ 	\
	"",									/* App session key*/ 		\
}
#else
#define LORA_INIT_MY_DEVICE                                         \
{	JOIN_MECHANISM,						/* Activation mechanism */	\
	DEFAULT_DATA_RATE,					/* Data Rate on start-up */ \
	ENABLE_ADR,							/* Use ADR or not (bool) */ \
	"",                 				/* Device EUI */            \
	"",                					/* Application EUI */       \
	"",									/* Application key */       \
	LORAWAN_DEVICE_ADDRESS,				/* Device address */ 		\
	LORAWAN_NWKSKEY, 					/* Network session key*/ 	\
	LORAWAN_APPSKEY, 					/* App session key*/ 		\
}
#endif

#endif /* __DEVICE_SETTINGS_H__ */