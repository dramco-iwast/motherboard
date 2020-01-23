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
 *         File: NonVolatileConfig.cpp
 *      Created: 2019-10-25
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
#include "NonVolatileConfig.h"
#include "DebugSerial.h"

// Non-Volatile storage offsets
#define NR_SENSORS_OFFSET               0
#define SENSOR_CONFIG_I2C_OFFSET        2
#define SENSOR_CONFIG_TYPE_OFFSET       3
#define SENSOR_CONFIG_NRSET_OFFSET      4
#define SENSOR_CONFIG_HEADER_SIZE       5
#define SENSOR_CONFIG_POL_OFFSET        (SENSOR_CONFIG_HEADER_SIZE)
#define SENSOR_CONFIG_TE_OFFSET         (SENSOR_CONFIG_HEADER_SIZE+2)
#define SENSOR_CONFIG_TL_OFFSET         (SENSOR_CONFIG_HEADER_SIZE+3)
#define SENSOR_CONFIG_TH_OFFSET         (SENSOR_CONFIG_HEADER_SIZE+5)
#define SENSOR_CONFIG_SETTTINGS_SIZE    7


NonVolatileConfig::NonVolatileConfig(void){
    this->sensorConfigSettings = NULL;
    this->nrSensors = 0;
}

NonVolatileConfig::~NonVolatileConfig(){
    this->discardConfig();
}

uint8_t NonVolatileConfig::getNrSensors(void){
    return this->nrSensors;
}

bool NonVolatileConfig::sensorInConfig(uint8_t ind, uint8_t i2c, uint8_t type){
    DEBUG.print("Looking up: ");
    DEBUG.print(ind);
    DEBUG.print(" - 0x");
    DEBUG.println(type, HEX);

    if(ind<this->nrSensors){
        DEBUG.print("ind in config, type = 0x");
        DEBUG.println(this->sensorConfigSettings[ind].type, HEX);

        if(this->sensorConfigSettings[ind].type == type && this->sensorConfigSettings[ind].i2cAddress == i2c){
            DEBUG.println("MATCH");
            return true;
        }
    }

    DEBUG.println("NO MATCH");

    return false;
}

void NonVolatileConfig::discardConfig(void){
    if(this->nrSensors > 0){
        // loop through config and free memory for settings
        for(uint8_t i=0; i<this->nrSensors; i++){
            free(this->sensorConfigSettings[i].settings);
        }
        // now free the rest
        free(this->sensorConfigSettings);
    }

    this->nrSensors = 0;

    DEBUG.println("RAM freed");

}

bool NonVolatileConfig::createNewConfig(uint8_t nrSensors, Sensor * list){
    this->nrSensors = nrSensors;

    this->sensorConfigSettings = (SensorConfig_t *) malloc(sizeof(SensorConfig_t) * this->nrSensors);
    if(this->sensorConfigSettings == NULL){
        DEBUG.println("Malloc failed.");
        return false;
    }

    for(int i=0; i<this->nrSensors; i++){
        // copy i-th sensor i2c address
        this->sensorConfigSettings[i].i2cAddress = list[i].getIicAddress();
        
        // copy i-th sensor type address
        this->sensorConfigSettings[i].type = list[i].getSensorType();

        // copy i-th sensor nr metric settings
        this->sensorConfigSettings[i].nrMetrics = list[i].getNrMetrics();

        // allocate space to store the settings
        this->sensorConfigSettings[i].settings = (MetricSettings_t *) malloc(sizeof(MetricSettings_t) * this->sensorConfigSettings[i].nrMetrics);
        if(this->sensorConfigSettings[i].settings == NULL){
            DEBUG.println("Malloc settings failed.");
            return false;
        }

        // init i-th sensor metric settings
        for(int j=0; j<this->sensorConfigSettings[i].nrMetrics; j++){
            // poll interval
            this->sensorConfigSettings[i].settings[j].pollInterval = 0;
            
            // threshold enabled
            this->sensorConfigSettings[i].settings[j].thresholdEnabled = 0;

            // threshold level low
            this->sensorConfigSettings[i].settings[j].thresholdLow = 0;

            // threshold level high
            this->sensorConfigSettings[i].settings[j].thresholdHigh = 0xFFFF;
        }
    }

    return true;
}

// get type of sensor in config (return false if sensor ind is not valid)
bool NonVolatileConfig::getSensorType(uint8_t ind, uint8_t * type){
    if(ind < this->nrSensors){
        *type = this->sensorConfigSettings[ind].type;
        return true;
    }
    return false;
}

// get i2caddress of sensor in config (return false if sensor ind is not valid)
bool NonVolatileConfig::getSensorI2CAddress(uint8_t ind, uint8_t * i2cAddress){
    if(ind < this->nrSensors){
        *i2cAddress = this->sensorConfigSettings[ind].i2cAddress;
        return true;
    }
    return false;
}

// get poll interval for metric 'metric' of sensor with ind 'ind'
bool NonVolatileConfig::getSensorPollInterval(uint8_t ind, uint8_t metric, uint16_t * poll){
    if(ind < this->nrSensors){
        if(metric < this->sensorConfigSettings[ind].nrMetrics){
            *poll = this->sensorConfigSettings[ind].settings[metric].pollInterval;
            return true;
        }
    }
    return false;
}

// get poll interval for metric 'metric' of sensor with ind 'ind'
bool NonVolatileConfig::getSensorThresholdSettings(uint8_t ind, uint8_t metric, uint8_t *enabled, uint16_t * tll, uint16_t *tlh){
    if(ind < this->nrSensors){
        if(metric < this->sensorConfigSettings[ind].nrMetrics){
            *enabled = this->sensorConfigSettings[ind].settings[metric].thresholdEnabled;
            *tll = this->sensorConfigSettings[ind].settings[metric].thresholdLow;
            *tlh = this->sensorConfigSettings[ind].settings[metric].thresholdHigh;
            return true;
        }
    }
    return false;
}

bool NonVolatileConfig::storeSensorConfigField(uint8_t sensorId, uint8_t metric, SensorConfigField_t field, uint8_t value){
    int offset = this->getConfigFieldOffset(sensorId, field, metric);
    if(offset == -1){
        return false;
    }
    if(offset<EEPROM.length()){
        this->storeSensorConfigField(offset, value);
        // also update RAM
        if(field ==  sensorConfThresholdEnabled){
            this->sensorConfigSettings[sensorId].settings[metric].thresholdEnabled = value;
        }
        return true;
    }
    return false;
}

bool NonVolatileConfig::storeSensorConfigField(uint8_t sensorId, uint8_t metric, SensorConfigField_t field, uint16_t value){
    int offset = this->getConfigFieldOffset(sensorId, field, metric);
    if(offset == -1){
        return false;
    }
    if(offset<EEPROM.length() - 1){
        this->storeSensorConfigField(offset, value);
        // also update RAM
        switch(field){
            case sensorConfPollInterval: {
                this->sensorConfigSettings[sensorId].settings[metric].pollInterval = value;
            } break;
            case sensorConfThresholdLow: {
                this->sensorConfigSettings[sensorId].settings[metric].thresholdLow = value;
            } break;
            case sensorConfThresholdHigh: {
                this->sensorConfigSettings[sensorId].settings[metric].thresholdHigh = value;
            } break;
            default : break;
        }
        return true;
    }
    return false;
}

// store complete sensor configuration / will overwrite existing configs
void NonVolatileConfig::storeSensorConfig(void){
    EEPROM.write(NR_SENSORS_OFFSET, nrSensors);

    int offset = 1;
    int bytesStored = 0;
    for(uint8_t i=0; i<this->nrSensors; i++){
        storeSensorConfig(offset, this->sensorConfigSettings[i], &bytesStored);
        offset += bytesStored;
    }

    EEPROM.commit();
    this->printSensorConfig();
}

void NonVolatileConfig::printSensorConfig(void){
    DEBUG.print("Nr sensors: ");
    DEBUG.println(this->nrSensors);

    for(int i=0; i<nrSensors; i++){
        DEBUG.print(" * Sensor ");
        DEBUG.println(i);
        DEBUG.print("    - i2c: ");
        DEBUG.println(this->sensorConfigSettings[i].i2cAddress, HEX);
        DEBUG.print("    - type: ");
        DEBUG.println(this->sensorConfigSettings[i].type, HEX);
        DEBUG.print("    - metrics: ");
        DEBUG.println(this->sensorConfigSettings[i].nrMetrics);
        for(int j=0; j<this->sensorConfigSettings[i].nrMetrics; j++){
            DEBUG.print("    - m");
            DEBUG.print(j);
            DEBUG.print(" ");
            DEBUG.print(this->sensorConfigSettings[i].settings[j].pollInterval);
            DEBUG.print(" ");
            DEBUG.print(this->sensorConfigSettings[i].settings[j].thresholdEnabled);
            DEBUG.print(" ");
            DEBUG.print(this->sensorConfigSettings[i].settings[j].thresholdLow);
            DEBUG.print(" ");
            DEBUG.println(this->sensorConfigSettings[i].settings[j].thresholdHigh);
        }
    }
}


/*****************************************************************************
 * Private methods                                                           *
 *****************************************************************************/

// store configuration of a single sensor (does not commit to 'EEPROM')
bool NonVolatileConfig::storeSensorConfig(int offset, SensorConfig_t config, int * bytesStored){
    int space_needed = SENSOR_CONFIG_HEADER_SIZE + SENSOR_CONFIG_SETTTINGS_SIZE*config.nrMetrics;
    if(space_needed + offset > EEPROM.length()){
        return false;
    }

    int size = 2;
    int memPtr = offset;
    uint8_t msb, lsb;

    EEPROM.write(memPtr + size++, config.i2cAddress);
    EEPROM.write(memPtr + size++, config.type);
    EEPROM.write(memPtr + size++, config.nrMetrics);
    
    for(int i=0; i<config.nrMetrics; i++){
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

    return true;
}

// store a single configuration field (1 byte)
void NonVolatileConfig::storeSensorConfigField(int offset, uint8_t value){
    EEPROM.write(offset, value);
    EEPROM.commit();
}

// store a single configuration field (2 bytes)
void NonVolatileConfig::storeSensorConfigField(int offset, uint16_t value){
    uint8_t msb, lsb;
    msb = (uint8_t)(value >> 8);
    lsb = (uint8_t)(value & 0xFF);
    EEPROM.write(offset, msb);
    EEPROM.write(offset+1, lsb);
    EEPROM.commit();
}

int NonVolatileConfig::getConfigFieldOffset(uint8_t sensorId, SensorConfigField_t field){
    int baseOffset = 1;
    int fieldOffset;

    uint8_t msb, lsb;

    // check ind is in range
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

int NonVolatileConfig::getConfigFieldOffset(uint8_t sensorId, SensorConfigField_t field, uint8_t metric){
    int baseOffset = 1;
    int fieldOffset;

    uint8_t msb, lsb;

    // check ind is in range
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
bool NonVolatileConfig::readSensorConfig(void){
    if(!EEPROM.isValid()){
        return false;
    }

    this->nrSensors = EEPROM.read(NR_SENSORS_OFFSET);

    this->sensorConfigSettings = (SensorConfig_t *) malloc(sizeof(SensorConfig_t) * this->nrSensors);
    if(this->sensorConfigSettings == NULL){
        DEBUG.println("Malloc failed.");
        return false;
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
        this->sensorConfigSettings[i].nrMetrics = EEPROM.read(offset+SENSOR_CONFIG_NRSET_OFFSET);

        // allocate space to store the settings
        this->sensorConfigSettings[i].settings = (MetricSettings_t *) malloc(sizeof(MetricSettings_t) * this->sensorConfigSettings[i].nrMetrics);
        if(this->sensorConfigSettings[i].settings == NULL){
            DEBUG.println("Malloc settings failed.");
            return false;
        }

        // read i-th sensor metric settings
        for(int j=0; j<this->sensorConfigSettings[i].nrMetrics; j++){
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

