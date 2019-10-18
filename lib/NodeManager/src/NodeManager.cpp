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
 *         File: NodeManager.cpp
 *      Created: 2019-10-16
 *       Author: Geoffrey Ottoy
 *      Version: 0.1
 *
 *  Description: IWAST node manager
 *      needs more info
 *
 *
 *  License (T.B.D.)
 */

#include <FlashAsEEPROM.h>
#include "NodeManager.h"

#define MAX_NR_SENSORS                  8

#define SENSOR_CONFIG_I2C_OFFSET        2
#define SENSOR_CONFIG_TYPE_OFFSET       3
#define SENSOR_CONFIG_NRSET_OFFSET      4
#define SENSOR_CONFIG_HEADER_SIZE       5
#define SENSOR_CONFIG_POL_OFFSET        (SENSOR_CONFIG_HEADER_SIZE)
#define SENSOR_CONFIG_TE_OFFSET         (SENSOR_CONFIG_HEADER_SIZE+2)
#define SENSOR_CONFIG_TL_OFFSET         (SENSOR_CONFIG_HEADER_SIZE+3)
#define SENSOR_CONFIG_TH_OFFSET         (SENSOR_CONFIG_HEADER_SIZE+5)
#define SENSOR_CONFIG_SETTTINGS_SIZE    7

#define NR_SENSORS_OFFSET               0

bool wakeup = false;

static void rtcCallback(void){
    wakeup = true;
}

void DUMP_EEPROM(int size){
    Serial.println("EEPROM DUMP");
    for(int i=0; i<size; i++){
        Serial.print("A: ");
        Serial.print(i, HEX);
        Serial.print(" - ");
        Serial.println(EEPROM.read(i), HEX);
    }
}

NodeManager::NodeManager(uint16_t id){

//#warning "NodeManager class uses Serial."

    this->id = id;
    sprintf(this->idStr, "%04X", id);

    memset(this->atCommand, '\0', AT_COMMAND_MAX_SIZE);
    this->atFill = 0;
    this->commandReceived = false;
    this->sensorConfigSettings = NULL;

    this->atTimerEnabled = true;
    this->lowPowerOperation = false;
    this->startLowPower = true;
    this->rtc = new RTCZero();
}

void NodeManager::begin(void){
    Serial.println("Initializing NodeManager...");

    if(this->readSensorConfig()){
        Serial.println("Previous configuration found.");
        // print config
        this->printSensorConfig();
    }
    else{
        // write config for debug purposes
        Serial.println("No configuration stored yet.");
        EEPROM.write(0, 2);

        SensorConfig_t c;
        c.i2cAddress = 0x60;
        c.type = 0x68;
        c.nrSettings = 2;

        MetricSettings_t s[2];
        s[0].pollInterval = 3600;
        s[0].thresholdEnabled = 0;
        s[0].thresholdLow = 0;
        s[0].thresholdHigh = 0;
        s[1].pollInterval = 600;
        s[1].thresholdEnabled = 1;
        s[1].thresholdLow = 5;
        s[1].thresholdHigh = 1000;
        c.settings = s;

        int size = 0;
        this->storeSensorConfig(1, c, &size);
        Serial.print("bytes stored: ");
        Serial.println(size);

        c.i2cAddress = 0x61;
        c.type = 0x21;
        c.nrSettings = 0;
        c.settings = NULL;

        this->storeSensorConfig(1+size, c, &size);
        Serial.print("bytes stored: ");
        Serial.println(size);

        DUMP_EEPROM(32);
    }

    this->rtc->begin();
    this->rtc->attachInterrupt(rtcCallback);
    this->atStartTime = millis();
    Serial.println("Done\n");
}

void NodeManager::loop(void){
    // disable at commands after 10000
    if(this->atTimerEnabled){
        if(millis() - this->atStartTime > 10000){
            this->lowPowerOperation = true;
            this->atTimerEnabled = false;
        }
    }

    // low power operation
    if(this->lowPowerOperation){
        if(this->startLowPower){
            this->startLowPower = false;
            // start rtc
        }
        else{

        }
        delay(1000); // nothing low-power here
        Serial.println("sleep");
    }
    // parse at commands
    else{
        this->processAtCommands();
    }
}

void NodeManager::processAtCommands(void){
    // receive characters
    while(Serial.available()) {
        // get the new byte:
        char atChar = (char)Serial.read();
        // add it to the buffer
        if(this->atFill < AT_COMMAND_MAX_SIZE){
            this->atCommand[this->atFill++] = atChar;
        }
        // if the incoming character is a newline, set a flag so the main loop can
        // do something about it:
        if(atChar == '\n') {
            this->commandReceived = true;
        }
    }

    // process command buffer
    if(this->commandReceived) {
        bool commandProcessed = false;
        // check for at command
        if(strstr(this->atCommand, "AT+")){
            char * specific = this->atCommand+3;

            // Ping motherboard
            if(strstr(specific, "CLS")){
                Serial.println("OK");
                commandProcessed = true;
                this->lowPowerOperation = true;
                this->atTimerEnabled = false;
            }

            // Ping motherboard
            if(strstr(specific, "PNG?")){
                Serial.print("+PNG: ");
                Serial.println(this->idStr);
                commandProcessed = true;
            }

            // List sensors 
            if(strstr(specific, "LS?")){
                Serial.print("+LS: ");
                char atOut[5];
                for(uint8_t i=0; i<this->nrSensors; i++){
                    if(i>0){
                        Serial.print(" ");
                    }
                    sprintf(atOut, "%02X%02X", (uint8_t)(i+1), this->sensorConfigSettings[i].type);
                    Serial.print(atOut);
                }
                Serial.println();
                commandProcessed = true;
            }

            // Get sensor poll interval 
            if(strstr(specific, "POL?")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                // check if requested info is available
                if(id < this->nrSensors){
                    if(metric<this->sensorConfigSettings[id].nrSettings){
                        // poll interval for (id, metric)
                        Serial.print("+POL: ");
                        Serial.println(this->sensorConfigSettings[id].settings[metric].pollInterval);
                        commandProcessed = true;
                    }
                }
            }

            // Set sensor poll interval 
            if(strstr(specific, "POL=")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                char * nextNr = strchr(argStr+3, ' ');
                uint16_t pol = strtol(nextNr, NULL, 10);

                bool status = false;
                if(id < this->nrSensors){
                    if(metric<this->sensorConfigSettings[id].nrSettings){
                        this->sensorConfigSettings[id].settings[metric].pollInterval = pol;
                        if(this->storeSensorConfigField(id, pol, sensorConfPollInterval, metric)){
                            Serial.println("OK");
                            status = true;
                        }
                    }
                }
                
                commandProcessed = status;
            }

            // Get sensor threshold settings
            if(strstr(specific, "TH?")){
                char * argStr = specific + 3;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                // check if requested info is available
                if(id < this->nrSensors){
                    if(metric<this->sensorConfigSettings[id].nrSettings){
                        // threshol settings for (id, metric)
                        Serial.print("+TH: ");
                        Serial.print(this->sensorConfigSettings[id].settings[metric].thresholdEnabled);
                        Serial.print(" ");
                        Serial.print(this->sensorConfigSettings[id].settings[metric].thresholdLow);
                        Serial.print(" ");
                        Serial.println(this->sensorConfigSettings[id].settings[metric].thresholdHigh);
                        commandProcessed = true;
                    }
                }
            }

            // Set sensor threshold enabled 
            if(strstr(specific, "TE=")){
                char * argStr = specific + 3;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                char * nextNr = strchr(argStr+3, ' ');
                uint8_t enabled = strtol(nextNr, NULL, 10);

                bool status = false;
                if(id < this->nrSensors){
                    if(metric<this->sensorConfigSettings[id].nrSettings){
                        this->sensorConfigSettings[id].settings[metric].thresholdEnabled = enabled;
                        if(this->storeSensorConfigField(id, enabled, sensorConfThresholdEnabled, metric)){
                            Serial.println("OK");
                            status = true;
                        }
                    }
                }
                
                commandProcessed = status;
            }

            // Set sensor low threshold level 
            // Set sensor threshold enabled 
            if(strstr(specific, "TLL=")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                char * nextNr = strchr(argStr+3, ' ');
                uint16_t tll = strtol(nextNr, NULL, 10);

                bool status = false;
                if(id < this->nrSensors){
                    if(metric<this->sensorConfigSettings[id].nrSettings){
                        this->sensorConfigSettings[id].settings[metric].thresholdLow = tll;
                        if(this->storeSensorConfigField(id, tll, sensorConfThresholdLow, metric)){
                            Serial.println("OK");
                            status = true;
                        }
                    }
                }
                
                commandProcessed = status;
            }

            // Set sensor low threshold level 
            if(strstr(specific, "TLH=")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                char * nextNr = strchr(argStr+3, ' ');
                uint16_t tlh = strtol(nextNr, NULL, 10);

                bool status = false;
                if(id < this->nrSensors){
                    if(metric<this->sensorConfigSettings[id].nrSettings){
                        this->sensorConfigSettings[id].settings[metric].thresholdHigh = tlh;
                        if(this->storeSensorConfigField(id, tlh, sensorConfThresholdHigh, metric)){
                            Serial.println("OK");
                            status = true;
                        }
                    }
                }
                
                commandProcessed = status;
            }
        }

        if(!commandProcessed){
            Serial.println("ERROR");
        }

        this->atTimerEnabled = false;

        // clear the buffer
        memset(this->atCommand, '\0', AT_COMMAND_MAX_SIZE);
        this->atFill = 0;
        this->commandReceived = false;
    }
}


/***/
// store complete sensor configuration / will overwrite existing configs
bool NodeManager::storeSensorConfig(uint8_t nrSensors, SensorConfig_t * config){
    if(nrSensors > MAX_NR_SENSORS){
        return false;
    }
    if(config == NULL){
        return false;
    }
    EEPROM.write(NR_SENSORS_OFFSET, nrSensors);

    int offset = 1;
    int bytesStored = 0;
    for(uint8_t i=0; i<nrSensors; i++){
        storeSensorConfig(offset, config[i], &bytesStored);
        offset += bytesStored;
    }

    EEPROM.commit();

    return true;
}

// store configuration of a single sensor
bool NodeManager::storeSensorConfig(int offset, SensorConfig_t config, int * bytesStored){
    int space_needed = SENSOR_CONFIG_HEADER_SIZE + SENSOR_CONFIG_SETTTINGS_SIZE*config.nrSettings;
    if(space_needed + offset > EEPROM.length()){
        return false;
    }

    int size = 2;
    int memPtr = offset;
    uint8_t msb, lsb;

    EEPROM.write(memPtr + size++, config.i2cAddress);
    EEPROM.write(memPtr + size++, config.type);
    EEPROM.write(memPtr + size++, config.nrSettings);
    
    for(int i=0; i<config.nrSettings; i++){
        // write i-th sensor metric poll interval
        msb = (uint8_t)(config.settings[i].pollInterval >> 8);
        lsb = (uint8_t)(config.settings[i].pollInterval & 0xFF);
        EEPROM.write(memPtr + size++, msb);
        EEPROM.write(memPtr + size++, lsb);

        // write i-th sensor metric threshold enabled
        EEPROM.write(memPtr + size++, config.settings[i].thresholdEnabled);

        // write i-th sensor metric threshold level low
        msb = (uint8_t)(config.settings[i].thresholdLow >> 8);
        lsb = (uint8_t)(config.settings[i].thresholdLow & 0xFF);
        EEPROM.write(memPtr + size++, msb);
        EEPROM.write(memPtr + size++, lsb);

        // write i-th sensor metric threshold level high
        msb = (uint8_t)(config.settings[i].thresholdHigh >> 8);
        lsb = (uint8_t)(config.settings[i].thresholdHigh & 0xFF);
        EEPROM.write(memPtr + size++, msb);
        EEPROM.write(memPtr + size++, lsb);
    }

    *bytesStored = size;
    msb = (uint8_t)((offset+size) >> 8);
    lsb = (uint8_t)((offset+size) & 0xFF);
    EEPROM.write(offset, msb);
    EEPROM.write(offset + 1, lsb);

    EEPROM.commit();

    return true;
}

bool NodeManager::storeSensorConfigField(uint8_t sensorId, uint8_t value, SensorConfigField_t field, uint8_t metric){
    int offset = this->getConfigFieldOffset(sensorId, field, metric);
    if(offset == -1){
        return false;
    }
    if(offset<EEPROM.length()){
        this->storeSensorConfigField(offset, value);
        return true;
    }
    return false;
}

bool NodeManager::storeSensorConfigField(uint8_t sensorId, uint16_t value, SensorConfigField_t field, uint8_t metric){
    int offset = this->getConfigFieldOffset(sensorId, field, metric);
    if(offset == -1){
        return false;
    }
    if(offset<EEPROM.length() - 1){
        this->storeSensorConfigField(offset, value);
        return true;
    }
    return false;
}


// store a single configuration field (1 byte)
void NodeManager::storeSensorConfigField(int offset, uint8_t value){
    EEPROM.write(offset, value);
    EEPROM.commit();
}

// store a single configuration field (2 bytes)
void NodeManager::storeSensorConfigField(int offset, uint16_t value){
    uint8_t msb, lsb;
    msb = (uint8_t)(value >> 8);
    lsb = (uint8_t)(value & 0xFF);
    EEPROM.write(offset, msb);
    EEPROM.write(offset+1, lsb);
    EEPROM.commit();
}

int NodeManager::getConfigFieldOffset(uint8_t sensorId, SensorConfigField_t field){
    int baseOffset = 1;
    int fieldOffset;

    uint8_t msb, lsb;

    // check id is in range
    if(!(sensorId < this->nrSensors)){
        return -1;
    }

    // loop over sensors (ignore loop if sensorId = 0 -> first sensor, baseOffset = 1)
    for(int i=0; i<sensorId; i++){
        // read offset of next sensor
        msb = EEPROM.read(baseOffset);
        lsb = EEPROM.read(baseOffset+1);
        baseOffset = (int)((msb<<8) | lsb);
    }

    switch(field){
        case sensorConfBase:{
            fieldOffset = 0;
        } break;
        case sensorConfI2cAddress:{
            fieldOffset = SENSOR_CONFIG_I2C_OFFSET;
        } break;
        case sensorConfType:{
            fieldOffset = SENSOR_CONFIG_TYPE_OFFSET;
        } break;
        default: return -1;
    }

    return baseOffset + fieldOffset;
}

int NodeManager::getConfigFieldOffset(uint8_t sensorId, SensorConfigField_t field, uint8_t metric){
    int baseOffset = 1;
    int fieldOffset;

    uint8_t msb, lsb;

    // check id is in range
    if(!(sensorId < this->nrSensors)){
        return -1;
    }

    // loop over sensors (ignore loop if sensorId = 0 -> first sensor, baseOffset = 1)
    for(int i=0; i<sensorId; i++){
        // read offset of next sensor
        msb = EEPROM.read(baseOffset);
        lsb = EEPROM.read(baseOffset+1);
        baseOffset = (int)((msb<<8) | lsb);
    }

    // read metric sections size
    int nrMetrics = (int)EEPROM.read(baseOffset+4);

    // determine extra offset
    if( !(metric < nrMetrics) ){
        return -1;
    }
    switch(field){
        case sensorConfPollInterval:{
            fieldOffset = SENSOR_CONFIG_POL_OFFSET + metric * SENSOR_CONFIG_SETTTINGS_SIZE;
        } break;
        case sensorConfThresholdEnabled:{
            fieldOffset = SENSOR_CONFIG_TE_OFFSET + metric * SENSOR_CONFIG_SETTTINGS_SIZE;
        } break;
        case sensorConfThresholdLow:{
            fieldOffset = SENSOR_CONFIG_TL_OFFSET + metric * SENSOR_CONFIG_SETTTINGS_SIZE;
        } break;
        case sensorConfThresholdHigh:{
            fieldOffset = SENSOR_CONFIG_TH_OFFSET + metric * SENSOR_CONFIG_SETTTINGS_SIZE;
        } break;
        default: return -1;
    }

    return baseOffset + fieldOffset;
}

// read the complete configuration memory
bool NodeManager::readSensorConfig(void){
    if(!EEPROM.isValid()){
        return false;
    }
    if(this->sensorConfigSettings != NULL){
        return false;
    }

    this->nrSensors = EEPROM.read(NR_SENSORS_OFFSET);

    this->sensorConfigSettings = (SensorConfig_t *) malloc(sizeof(SensorConfig_t) * this->nrSensors);
    if(this->sensorConfigSettings == NULL){
        Serial.println("Malloc failed.");
    }

    int offset;
    for(int i=0; i<this->nrSensors; i++){
        offset = getConfigFieldOffset(i, sensorConfBase);

        uint8_t msb, lsb;
        // read i-th sensor i2c address
        this->sensorConfigSettings[i].i2cAddress = EEPROM.read(offset+SENSOR_CONFIG_I2C_OFFSET);
        
        // read i-th sensor type address
        this->sensorConfigSettings[i].type = EEPROM.read(offset+SENSOR_CONFIG_TYPE_OFFSET);

        // read i-th sensor nr metric settings
        this->sensorConfigSettings[i].nrSettings = EEPROM.read(offset+SENSOR_CONFIG_NRSET_OFFSET);

        // allocate space to store the settings
        this->sensorConfigSettings[i].settings = (MetricSettings_t *) malloc(sizeof(MetricSettings_t) * this->sensorConfigSettings[i].nrSettings);
        if(this->sensorConfigSettings[i].settings == NULL){
            Serial.println("Malloc settings failed.");
            return false;
        }

        // read i-th sensor metric settings
        for(int j=0; j<this->sensorConfigSettings[i].nrSettings; j++){
            // poll interval
            msb = EEPROM.read(offset + SENSOR_CONFIG_POL_OFFSET + j * SENSOR_CONFIG_SETTTINGS_SIZE);
            lsb = EEPROM.read(offset + SENSOR_CONFIG_POL_OFFSET + 1 + j * SENSOR_CONFIG_SETTTINGS_SIZE);
            this->sensorConfigSettings[i].settings[j].pollInterval = (uint16_t)((msb<<8) | lsb);
            
            // threshold enabled
            this->sensorConfigSettings[i].settings[j].thresholdEnabled = EEPROM.read(offset + SENSOR_CONFIG_TE_OFFSET + j * SENSOR_CONFIG_SETTTINGS_SIZE);

            // threshold level low
            msb = EEPROM.read(offset + SENSOR_CONFIG_TL_OFFSET + j * SENSOR_CONFIG_SETTTINGS_SIZE);
            lsb = EEPROM.read(offset + SENSOR_CONFIG_TL_OFFSET + 1 + j * SENSOR_CONFIG_SETTTINGS_SIZE);
            this->sensorConfigSettings[i].settings[j].thresholdLow = (uint16_t)((msb<<8) | lsb);

            // threshold level high
            msb = EEPROM.read(offset + SENSOR_CONFIG_TH_OFFSET + j * SENSOR_CONFIG_SETTTINGS_SIZE);
            lsb = EEPROM.read(offset + SENSOR_CONFIG_TH_OFFSET + 1 + j * SENSOR_CONFIG_SETTTINGS_SIZE);
            this->sensorConfigSettings[i].settings[j].thresholdHigh = (uint16_t)((msb<<8) | lsb);
        }
    }

    return true;
}

void NodeManager::printSensorConfig(void){
    Serial.print("Nr sensors: ");
    Serial.println(this->nrSensors);

    for(int i=0; i<nrSensors; i++){
        Serial.print(" * Sensor ");
        Serial.println(i);
        Serial.print("    - i2c: ");
        Serial.println(this->sensorConfigSettings[i].i2cAddress, HEX);
        Serial.print("    - type: ");
        Serial.println(this->sensorConfigSettings[i].type, HEX);
        Serial.print("    - metrics: ");
        Serial.println(this->sensorConfigSettings[i].nrSettings);
        for(int j=0; j<this->sensorConfigSettings[i].nrSettings; j++){
            Serial.print("    - m");
            Serial.print(j);
            Serial.print(" ");
            Serial.print(this->sensorConfigSettings[i].settings[j].pollInterval);
            Serial.print(" ");
            Serial.print(this->sensorConfigSettings[i].settings[j].thresholdEnabled);
            Serial.print(" ");
            Serial.print(this->sensorConfigSettings[i].settings[j].thresholdLow);
            Serial.print(" ");
            Serial.println(this->sensorConfigSettings[i].settings[j].thresholdHigh);
        }
    }
}