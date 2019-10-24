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
 *         File: NodeManager.h
 *      Created: 2019-10-21
 *       Author: Geoffrey Ottoy
 *      Version: 0.1
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

#define AT_COMMAND_MAX_SIZE 32

typedef struct metricSettings{
    uint16_t pollInterval;
    uint8_t thresholdEnabled;
    uint16_t thresholdLow;
    uint16_t thresholdHigh;
} MetricSettings_t;

typedef struct sensorConfig{
    uint8_t i2cAddress;
    uint8_t type;
    uint8_t nrSettings;
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

    // destructor needed? e.g. memory free when in "normal operation = sensors are configured"

    // needed?
    void begin(void);

    // read complete configuration from non-volatile memory
    bool readSensorConfig(void);

private:


    // store complete sensor configuration / will overwrite existing configs
    bool storeSensorConfig(uint8_t nrSensors, SensorConfig_t * config);
    // store configuration of a single sensor
    bool storeSensorConfig(int offset, SensorConfig_t config, int * bytesStored);

    // store a single configuration field (1 byte)
    bool storeSensorConfigField(uint8_t sensorId, uint8_t value, SensorConfigField_t field, uint8_t metric=0);
    void storeSensorConfigField(int offset, uint8_t value);
    // store a single configuration field (2 bytes)
    bool storeSensorConfigField(uint8_t sensorId, uint16_t value, SensorConfigField_t field, uint8_t metric=0);
    void storeSensorConfigField(int offset, uint16_t value);

    int getConfigFieldOffset(uint8_t sensorId, SensorConfigField_t field, uint8_t metric);
    int getConfigFieldOffset(uint8_t sensorId, SensorConfigField_t field);

    void printSensorConfig(void);

    uint16_t id;
    char idStr[5];

    uint8_t nrSensors;

    // for testing only? keep settings in RAM
    SensorConfig_t * sensorConfigSettings;
};



#endif /*__NON_VOLATILE_CONFIG_H__*/