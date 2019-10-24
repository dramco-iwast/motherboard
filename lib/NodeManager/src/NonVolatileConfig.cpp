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
#include "NonVolatileConfig.h"

// Maximum number of sensors that is being supported
#define MAX_NR_OF_SENSORS               8

// List of interrupt pins
// TODO

// Address range that is used to scan for sensors 
#define SENSOR_IIC_BASE_ADDRESS         0x60
#define SENSOR_IIC_END_ADDRESS          0x6F

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

}

/***/
// store complete sensor configuration / will overwrite existing configs
bool NonVolatileConfig::storeSensorConfig(uint8_t nrSensors, SensorConfig_t * config){
    if(nrSensors > MAX_NR_OF_SENSORS){
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
bool NonVolatileConfig::storeSensorConfig(int offset, SensorConfig_t config, int * bytesStored){
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

bool NonVolatileConfig::storeSensorConfigField(uint8_t sensorId, uint8_t value, SensorConfigField_t field, uint8_t metric){
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

bool NonVolatileConfig::storeSensorConfigField(uint8_t sensorId, uint16_t value, SensorConfigField_t field, uint8_t metric){
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

int NonVolatileConfig::getConfigFieldOffset(uint8_t sensorId, SensorConfigField_t field, uint8_t metric){
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
bool NonVolatileConfig::readSensorConfig(void){
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

void NonVolatileConfig::printSensorConfig(void){
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