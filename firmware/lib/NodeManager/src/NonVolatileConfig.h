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
 *         File: NonVolatileConfig.h
 *      Created: 2019-10-25
 *       Author: Geoffrey Ottoy
 *      Version: 1.2
 *
 *  Description: IWAST node manager
 *      needs more info
 *
 *
 *  License (T.B.D.)
 */

#ifndef __NON_VOLATILE_CONFIG_H__
#define __NON_VOLATILE_CONFIG_H__

#include <Arduino.h>
#include "Sensor.h"
#include "device_settings.h"

typedef struct nvLoraSettings{
    uint8_t dataAccumulation;
    uint8_t adrOn;
    char devEUI[EUI_LENGTH+1];
    char appKey[KEY_LENGTH+1];
} NvLoraSettings_t;

typedef struct metricSettings{
    uint16_t pollInterval;
    uint8_t thresholdEnabled;
    uint16_t thresholdLow;
    uint16_t thresholdHigh;
} MetricSettings_t;

typedef struct sensorConfig{
    uint8_t i2cAddress;
    uint8_t type;
    uint8_t nrMetrics;
    MetricSettings_t * settings;
} SensorConfig_t;

typedef enum configFields{
    sensorConfBase,
    sensorConfI2cAddress,
    sensorConfType,
    sensorConfPollInterval,
    sensorConfThresholdEnabled,
    sensorConfThresholdLow,
    sensorConfThresholdHigh
} SensorConfigField_t;

class NonVolatileConfig{

public:
    // constructor
    NonVolatileConfig(void);
    // desturctor
    ~NonVolatileConfig();

    // read complete configuration from non-volatile memory
    bool readSensorConfig(void);

    // set/get accumulation
    uint8_t getDataAccumulation(void);
    void setDataAccumulation(uint8_t yesNo);

    // get/set ADR
    uint8_t getADR(void);
    void setADR(uint8_t adrOn);

    // get/set Dev. EUI
    void getDevEUI(char * eui);
    void setDevEUI(char eui[EUI_LENGTH+1]);

    // get/set Application Key
    void getAppKey(char * key);
    void setAppKey(char key[KEY_LENGTH+1]);

    // get number of sensors in the configuration
    uint8_t getNrSensors(void);

    uint8_t lookupIdIndex(uint8_t id){
        uint8_t i = 0;
        while(i < this->nrSensors){
            if(this->sensorConfigSettings[i++].i2cAddress == id){
                return i;
            }
        }
        return 0;
    };

    // check if sensor with specified ind and type exists in the configuration
    bool sensorInConfig(uint8_t ind, uint8_t i2c, uint8_t type);
    
    // discard current configuration
    void discardConfig(void);
    // create a new configuration based on a list of sensors
    bool createNewConfig(uint8_t nrSensors, Sensor * list);

    // get type of sensor with ind 'ind'
    bool getSensorType(uint8_t ind, uint8_t * type);
    // get i2caddress of sensor in config (return false if sensor ind is not valid)
    bool getSensorI2CAddress(uint8_t ind, uint8_t * i2cAddress);
    // get poll interval for metric 'metric' of sensor with ind 'ind'
    bool getSensorPollInterval(uint8_t ind, uint8_t metric, uint16_t * poll);
    // get threshold settings for metric 'metric' of sensor with ind 'ind'
    bool getSensorThresholdSettings(uint8_t ind, uint8_t metric, uint8_t *enabled, uint16_t * tll, uint16_t *tlh);

    // store a single configuration field (1 byte)
    bool storeSensorConfigField(uint8_t sensorId, uint8_t metric, SensorConfigField_t field, uint8_t value);
    // store a single configuration field (2 bytes)
    bool storeSensorConfigField(uint8_t sensorId, uint8_t metric, SensorConfigField_t field, uint16_t value);

    // store complete sensor configuration / will overwrite existing configs
    void storeSensorConfig(void);

    // for debugging purposes
    void printSensorConfig(void);

private:
    // store lora settings
    bool storeLoraSettings(void);
    bool readLoraSettings(void);

    // store configuration of a single sensor
    bool storeSensorConfig(int offset, SensorConfig_t config, int * bytesStored);

    // store a single configuration field (1 byte)
    void storeSensorConfigField(int offset, uint8_t value);
    // store a single configuration field (2 bytes)
    void storeSensorConfigField(int offset, uint16_t value);

    int getConfigFieldOffset(uint8_t sensorId, SensorConfigField_t field, uint8_t metric);
    int getConfigFieldOffset(uint8_t sensorId, SensorConfigField_t field);
    
    uint8_t nrSensors;
    NvLoraSettings_t loraSettings;
    SensorConfig_t * sensorConfigSettings;
};



#endif /*__NON_VOLATILE_CONFIG_H__*/