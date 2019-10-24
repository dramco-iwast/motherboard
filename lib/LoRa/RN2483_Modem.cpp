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
 *         File: RN2483.cpp
 *      Created: 2019-10-21
 *       Author: Geoffrey Ottoy
 *      Version: 0.1
 *
 *  Description: Header file for RN2483.cpp. 
 *      More info ...
 *
 *  License (T.B.D.)
 */

#include "RN2483_Modem.h"
#include "wiring_private.h"

RN2483_Modem rn2483; // use rn23483 in main app;

static bool hexToString(uint8_t * bin, uint8_t binsz, char **result ){
	char hex_str[] = "0123456789abcdef";
	uint8_t i;

	if (!(*result = (char *)malloc(binsz * 2 + 1))){
		return false;
	}

	(*result)[binsz * 2] = 0;

	if (!binsz){
		return false;
	}

	for (i = 0; i < binsz; i++){
	  (*result)[i * 2 + 0] = hex_str[(bin[i] >> 4) & 0x0F];
	  (*result)[i * 2 + 1] = hex_str[(bin[i]     ) & 0x0F];
	}

	return true;
}

RN2483_Modem::RN2483_Modem(void){
}

void RN2483_Modem::init(void){ // Setup with autobaud
	memset(this->commandBuffer, '\0', RN2483_COMMAND_BUFFER_SIZE);

	pinMode(RN2483_RESET_PIN, OUTPUT);
	digitalWrite(RN2483_RESET_PIN, HIGH);
	delay(100);

	SerialRN.begin(RN2483_BAUD);
	delay(250);
}

// reset




RN2483_Status_t RN2483_Modem::macReset(void){
	sprintf(this->commandBuffer, "mac reset 868\r\n");
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::getHardwareEUI(void){
	// TODO: return device eui
	sprintf(this->commandBuffer, "mac get deveui\r\n");
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::getSystemVersion(void){
	// TODO: return system version
	sprintf(this->commandBuffer, "sys get ver\r\n");
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::getApplicationEUI(void){
	// TODO: return appEUI
	sprintf(this->commandBuffer, "sys get appeui\r\n");
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::setDeviceEUI(char * eui){
	sprintf(this->commandBuffer, "mac set deveui %s\r\n", eui);
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::setApplicationEUI(char * eui){
	sprintf(this->commandBuffer, "mac set appeui %s\r\n", eui);
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::setApplicationKey(char * key){
	sprintf(this->commandBuffer, "mac set appkey %s\r\n", key);
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::setApplicationSessionKey(char * key){
	sprintf(this->commandBuffer, "mac set appskey %s\r\n", key);
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::setNetworkSessionKey(char * key){
	sprintf(this->commandBuffer, "mac set nwkskey %s\r\n", key);
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::setDeviceAddress(char * address){
	sprintf(this->commandBuffer, "mac set devaddr %s\r\n", address);
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::setOutputPower(uint8_t pwr){
	sprintf(this->commandBuffer, "mac set pwridx %i\r\n", pwr);
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::disableAdaptiveDataRate(void){
	sprintf(this->commandBuffer, "mac set adr off\r\n");
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::disableAutomaticReplies(void){
	sprintf(this->commandBuffer, "mac set ar off\r\n");
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::setDataRate(uint8_t dr){
	sprintf(this->commandBuffer, "mac set dr %i\r\n", dr);
	return processMacCommand();
}

RN2483_Status_t RN2483_Modem::getDataRate(int8_t *dr){
	sprintf(this->commandBuffer, "mac get dr\r\n");
	RN2483_Status_t status = this->processMacCommand();
	if(status != DATA_RETURNED){
		return status;
	}

	// convert ascii to unsigned
	*dr = this->receiveBuffer[0] - '0';
	// final data check
	if(*dr >= 0 && *dr <= 7){
		return MAC_OK;
	}
	else{
		return MAC_ERR;
	}
}

RN2483_Status_t RN2483_Modem::enableAdaptiveDataRate(void){
	sprintf(this->commandBuffer, "mac set adr on\r\n");
	return this->processMacCommand();
}

RN2483_Status_t RN2483_Modem::setBatteryLevel(uint8_t battery){
	sprintf(this->commandBuffer, "mac set bat %i\r\n", battery);
	return this->processMacCommand();
}

RN2483_Status_t RN2483_Modem::joinOTAA(void){
	sprintf(this->commandBuffer, "mac join otaa\r\n");
	return this->processMacCommand(true);
}

RN2483_Status_t RN2483_Modem::joinABP(void){
	sprintf(this->commandBuffer, "mac join abp\r\n");
	return this->processMacCommand(true);
}

RN2483_Status_t RN2483_Modem::setDefaultOperation(LoRaSettings_t settings){
	RN2483_Status_t status = this->setOutputPower(RN2483_POWER_14DBM);
	if(status != MAC_OK){
		return status;
	}
	status = this->disableAutomaticReplies();
	if(status != MAC_OK){
		return status;
	}
	status = this->disableAdaptiveDataRate();
	if(status != MAC_OK){
		return status;
	}
	status = this->setDataRate(settings.dataRate);
	if(status != MAC_OK){
		return status;
	}
	status = this->enableAdaptiveDataRate();
	if(status != MAC_OK){
		return status;
	}
	return this->setBatteryLevel(254);
}

RN2483_Status_t RN2483_Modem::setup(LoRaSettings_t settings){
	if(settings.activationMethod == OTAA){
		return this->setupOTAA(settings);
	}
	else{
		return this->setupABP(settings);
	}
}

RN2483_Status_t RN2483_Modem::setupOTAA(LoRaSettings_t settings){
	RN2483_Status_t status = this->macReset();
	if(status != MAC_OK){
		return status;
	}
	status = this->setDeviceEUI(settings.deviceEUI);
	if(status != MAC_OK){
		return status;
	}
	status = this->setApplicationEUI(settings.applicationEUI);
	if(status != MAC_OK){
		return status;
	}
	status = this->setApplicationKey(settings.applicationKey);
	if(status != MAC_OK){
		return status;
	}
	status = this->setDefaultOperation(settings);
	if(status != MAC_OK){
		return status;
	}
	return this->joinOTAA();
}

RN2483_Status_t RN2483_Modem::setupABP(LoRaSettings_t settings){
	//Serial.println("Setup ABP");
	//Serial.println(" - mac reset");
	RN2483_Status_t status = this->macReset();
	if(status != MAC_OK){
		return status;
	}

	//Serial.println(" - set dev addr");
	status = this->setDeviceAddress(settings.deviceAddress);
	if(status != MAC_OK){
		return status;
	}
	
	//Serial.println(" - set nwk ses key");
	status = this->setNetworkSessionKey(settings.networkSessionKey);
	if(status != MAC_OK){
		return status;
	}
	
	//Serial.println(" - set app ses key");
	status = this->setApplicationSessionKey(settings.applicationSessionKey);
	if(status != MAC_OK){
		return status;
	}

	
	//Serial.println(" - set default op");
	status = this->setDefaultOperation(settings);
	if(status != MAC_OK){
		return status;
	}
	
	//Serial.println(" - try join");
	return this->joinABP();
}

RN2483_Status_t RN2483_Modem::transmitUnconfirmed(uint8_t * data, uint8_t payloadSize){
	char * encodedPayload;
	if(!hexToString(data, payloadSize, &encodedPayload )){
		return MAC_ERR;
	}
	//StringToHexString(data, payloadSize/2, &decodedPayload);
	sprintf(commandBuffer, "mac tx uncnf 1 %s\r\n", encodedPayload);
	free(encodedPayload);
	return this->processMacCommand(true);
}


// send a command to the modem and process the response
RN2483_Status_t RN2483_Modem::processMacCommand(bool secondResponse){
	uint8_t timeout = 255;
	this->bytesReceived = 0;
	memset(this->receiveBuffer, '\0', RN2483_RECEIVE_BUFFER_SIZE);

	// send command
	SerialRN.print(this->commandBuffer);
	//Serial.println("command sent");
	//Serial.println(this->commandBuffer);

	delay(100);

	// read response
	while(timeout--){
		// read chars and put them in a buffer
		while(SerialRN.available()){
			char c = SerialRN.read();
			if(this->bytesReceived<RN2483_RECEIVE_BUFFER_SIZE){
				this->receiveBuffer[this->bytesReceived++] = c;
			}
		}

		if(strstr(this->receiveBuffer, "ok") != NULL){
			//Serial.println("ok");

			//Serial.print("buffer: ");
			//Serial.println(this->receiveBuffer);

			if(secondResponse){
				// analyze response
				if(strstr(receiveBuffer, "accepted")){
					return JOIN_ACCEPTED;
				} else if(strstr(receiveBuffer, "denied")){
					return JOIN_DENIED;
				} else if(strstr(receiveBuffer, "mac_tx_ok")){
					return MAC_TX_OK;
				} else if(strstr(receiveBuffer, "mac_rx")){
					// TODO: capture data
					return MAC_RX;
				} else if(strstr(receiveBuffer, "mac_err")){
					return MAC_ERR;
				} else if(strstr(receiveBuffer, "invalid_data_len")){
					return INVALID_DATA_LEN;
				} else if(strstr(receiveBuffer, "radio_tx_ok")){
					return RADIO_TX_OK;
				} else if(strstr(receiveBuffer, "radio_err")){
					return RADIO_ERR;
				}
				return UNKOWN_ERR;
			}
			else {
				return MAC_OK;
			}
		}
		else{
			//Serial.println("foutje");
			//Serial.print("buffer: ");
			//Serial.println(this->receiveBuffer);
			if(strstr(this->receiveBuffer, "invalid_param")){
				return INVALID_PARAM;
			} else if(strstr(this->receiveBuffer, "not_joined")){
				return NOT_JOINED;
			} else if(strstr(this->receiveBuffer, "no_free_ch")){
				return NO_FREE_CH;
			} else if(strstr(this->receiveBuffer, "silent")){
				return SILENT;
			} else if(strstr(this->receiveBuffer, "frame_counter_err_rejoin_needed")){
				return FRAME_COUNTER_ERR_REJOIN_NEEDED;
			} else if(strstr(this->receiveBuffer, "busy")){
				return BUSY;
			} else if(strstr(this->receiveBuffer, "mac_paused")){
				return MAC_PAUSED;
			} else if(strstr(this->receiveBuffer, "invalid_data_len")){
				return INVALID_DATA_LEN;
			}

			return DATA_RETURNED; // unknow error?
		}
	}

	//Serial.println("timeout");
	return RN_RX_TIMEOUT;
}

