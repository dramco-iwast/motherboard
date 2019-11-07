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
#include <Wire.h>
#include "NodeManager.h"

// Maximum number of sensors that is being supported
#define MAX_NR_OF_SENSORS               6

// Address range that is used to scan for sensors 
#define SENSOR_IIC_BASE_ADDRESS         0x60
#define SENSOR_IIC_END_ADDRESS          0x6F

// List of interrupt pins
uint8_t intPins[MAX_NR_OF_SENSORS] = {A3, A4, 8, 9, 38, 3};

volatile bool sensorHasData[MAX_NR_OF_SENSORS] = {false, false, false, false, false, false};

void sensor0Callback(void){
    sensorHasData[0] = true;
    SerialAT.println("cb0");
}
void sensor1Callback(void){
    sensorHasData[1] = true;
    SerialAT.println("cb1");
}
void sensor2Callback(void){
    sensorHasData[2] = true;
    SerialAT.println("cb2");
}
void sensor3Callback(void){
    sensorHasData[3] = true;
    SerialAT.println("cb3");
}
void sensor4Callback(void){
    sensorHasData[4] = true;
    SerialAT.println("cb4");
}
void sensor5Callback(void){
    sensorHasData[5] = true;
    SerialAT.println("cb5");
}

typedef void (*SensorCb)(void);
SensorCb sensorCallbacks[MAX_NR_OF_SENSORS] = {
    sensor0Callback,
    sensor1Callback,
    sensor2Callback,
    sensor3Callback,
    sensor4Callback,
    sensor5Callback
};

void DUMP_EEPROM(int size){
    SerialAT.println("EEPROM DUMP");
    for(int i=0; i<size; i++){
        SerialAT.print("A: ");
        SerialAT.print(i, HEX);
        SerialAT.print(" - ");
        SerialAT.println(EEPROM.read(i), HEX);
    }
}

NodeManager::NodeManager(uint16_t id){

//#warning "NodeManager class uses SerialAT."

    this->id = id;
    sprintf(this->idStr, "%04X", id);

    memset(this->atCommand, '\0', AT_COMMAND_MAX_SIZE);
    this->atFill = 0;
    this->commandReceived = false;

    this->atTimerEnabled = true;
    this->conFigMode = true;

    this->nvConfig = new NonVolatileConfig();
}

void NodeManager::begin(void){
    SerialAT.println("Initializing NodeManager...");

    DUMP_EEPROM(32);

    // 1. Initialize interrupts pins
    for(uint8_t i=0; i<MAX_NR_OF_SENSORS; i++){
        pinMode(intPins[i], INPUT_PULLUP);
    }

    // 2. Initialize I2C bus
    delay(500);
    Wire.begin();
    Wire.setClock(100000);
    Wire.setTimeout(500);

    // 3. Build a sensor list (i.e. list of Sensor objects for interfacing with the sensor boards)
    uint8_t detectedSensors[MAX_NR_OF_SENSORS];
    uint8_t detectedCount = 0;
    Sensor * t =  new Sensor(); // temporary sensor object for interfacing with the bus

    // Test all addresses for a response
    for(uint8_t address=SENSOR_IIC_BASE_ADDRESS; address<SENSOR_IIC_END_ADDRESS+1; address++){
        if(t->pollAddress(address)){
            SerialAT.print("Found sensor at address: 0x");
            SerialAT.println(address, HEX);
            if(detectedCount<MAX_NR_OF_SENSORS){
                detectedSensors[detectedCount++] = address;
            }
            else{
                SerialAT.println("Maximum number of sensors exceeded");
            }
        }
    }
    // Delete temporary object
    delete t;

    // Set number of sensors connected to the motherboard
    this->nrSensors = detectedCount;

    // Now create the actual list of sensors ...
    this->sensorList = new Sensor[this->nrSensors];
    // ... and initialize this list
    for(uint8_t i=0; i<this->nrSensors; i++){
        if(!this->sensorList[i].init(detectedSensors[i])){
            SerialAT.print("Failed to initialize sensor at address: 0x");
            SerialAT.println(detectedSensors[i], HEX);
        }
        else{
            // detect which interrupt pin the i-th sensor is connected to
            this->sensorList[i].toggleInterrupt();
            delay(1); // give sensor time to toggle pin
            for(uint8_t pin=0; pin<MAX_NR_OF_SENSORS; pin++){
                if(digitalRead(intPins[pin]) == LOW){
                    this->sensorList[i].setIntPin(intPins[pin]);
                    attachInterrupt(this->sensorList[i].getIntPin(), sensorCallbacks[i], FALLING);
                }
            }
            this->sensorList[i].toggleInterrupt();

            SerialAT.print("Sensor: ");
            SerialAT.println(i+1);
            SerialAT.print(" - type: 0x");
            SerialAT.println(this->sensorList[i].getSensorType(), HEX);
            SerialAT.print(" - metrics: ");
            SerialAT.println(this->sensorList[i].getNrMetrics());
            SerialAT.print(" - int pin: ");
            SerialAT.println(this->sensorList[i].getIntPin());
        }
        delay(20);
    }

    // 4. Read a previous configuration from EEPROM
    SerialAT.println("Reading previous config from EEPROM ...");
    this->nvConfig->readSensorConfig();
    this->nvConfig->printSensorConfig();

    // 5. Test if connected sensors (sensorList) match the previous configuration (nvConfig)
    bool diffDetected = false;
    for(uint8_t i=0; i<this->nrSensors; i++){
        if(!this->nvConfig->sensorInConfig(i, this->sensorList[i].getSensorType())){
            diffDetected = true;
            break;
        }
    }

    // 6. If list of sensors from the previous configuration differs from the currently installed sensors
    // we completely discard the previous configuration and build a new one based on the sensorList
    if(diffDetected){
        SerialAT.println("Connected sensors don't match with previous config.");
        SerialAT.println("Creating new config ...");
        // discard old config
        this->nvConfig->discardConfig();
        // build new config (in RAM)
        this->nvConfig->createNewConfig(this->nrSensors, this->sensorList);
        SerialAT.println("Updating non volatile config:");
        this->nvConfig->storeSensorConfig();
    }
    else{
        SerialAT.println("Connected sensors match with previous config.");
    }
    this->nvConfig->printSensorConfig();

    this->atStartTime = millis();

    SerialAT.println("Done\n");
}

void NodeManager::runConfigMode(bool forever){
    while(this->conFigMode || forever){
        // disable at commands after 10000
        if(this->atTimerEnabled){
            if(millis() - this->atStartTime > 10000){
                this->conFigMode = false;
            }
        }

        this->processAtCommands();
    }

    SerialAT.println("Configuring connected sensors ...");
    this->configureSensors();

    SerialAT.println("Using configuration:");
    this->nvConfig->storeSensorConfig();

    SerialAT.println("Exit config mode.");

    delete this->nvConfig;
}

void NodeManager::loop(void){
    // low-power loop (but not right now)
    delay(1000);
    for(uint8_t i=0; i<this->nrSensors; i++){
        this->sensorList[i].loop();
    }
    SerialAT.println("sleep");
}

bool NodeManager::dataAvailable(void){
    return (sensorHasData[0] | sensorHasData[1] | sensorHasData[2] | sensorHasData[3] | sensorHasData[4] | sensorHasData[5]);
}

void NodeManager::getSensorData(uint8_t * data, uint8_t * len){
    for(uint8_t i=0; i<this->nrSensors; i++){
        if(sensorHasData[i]){
            sensorHasData[i] = false;
            uint8_t tempData[20];
            uint8_t len = 0;
            this->sensorList[i].readMeasurementData(tempData, &len); // if  this is called sensor wakes up again?
            SerialAT.print("Sensor ");
            SerialAT.print(i);
            SerialAT.print(" data: ");
            for(uint8_t j=0; j<len; j++){
                if(tempData[j]<16){
                    SerialAT.print("0");
                }
                SerialAT.print(tempData[j], HEX);
            }
            SerialAT.println();
        }
    }
}

void NodeManager::processAtCommands(void){
    // receive characters
    while(SerialAT.available()) {
        // get the new byte:
        char atChar = (char)SerialAT.read();
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
                SerialAT.println("OK");
                commandProcessed = true;
                this->conFigMode = false;
            }

            // Ping motherboard
            if(strstr(specific, "PNG?")){
                SerialAT.print("+PNG: ");
                SerialAT.println(this->idStr);
                commandProcessed = true;
            }

            // List sensors 
            if(strstr(specific, "LS?")){
                SerialAT.print("+LS: ");
                char atOut[5];
                for(uint8_t i=0; i<this->nrSensors; i++){
                    if(i>0){
                        SerialAT.print(" ");
                    }
                    uint8_t type = 0;
                    this->nvConfig->getSensorType(i, &type);
                    sprintf(atOut, "%02X%02X", (uint8_t)(i+1), type);
                    SerialAT.print(atOut);
                }
                SerialAT.println();
                commandProcessed = true;
            }

            // Get sensor poll interval 
            if(strstr(specific, "POL?")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                uint16_t pollInterval = 0;
                // get poll interval for (id, metric)
                if(this->nvConfig->getSensorPollInterval(id, metric, &pollInterval)){ // false if either metric or id are invalid/incorrect
                    SerialAT.print("+POL: ");
                    SerialAT.println(pollInterval);
                    commandProcessed = true;
                }
            }

            // Set sensor poll interval 
            if(strstr(specific, "POL=")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                char * nextNr = strchr(argStr+3, ' ');
                uint16_t pol = strtol(nextNr, NULL, 10);

                if(this->nvConfig->storeSensorConfigField(id, metric, sensorConfPollInterval, pol)){
                    SerialAT.println("OK");
                    commandProcessed = true;
                }
            }

            // Get sensor threshold settings
            if(strstr(specific, "TH?")){
                char * argStr = specific + 3;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                uint8_t enabled = 0;
                uint16_t tLevLow = 0;
                uint16_t tLevHigh = 0;
                // get threshold settings for (id, metric)
                if(this->nvConfig->getSensorThresholdSettings(id, metric, &enabled, &tLevLow, &tLevHigh)){
                    SerialAT.print("+TH: ");
                    SerialAT.print(enabled);
                    SerialAT.print(" ");
                    SerialAT.print(tLevLow);
                    SerialAT.print(" ");
                    SerialAT.println(tLevHigh);
                    commandProcessed = true;
                }
            }

            // Set sensor threshold enabled 
            if(strstr(specific, "TE=")){
                char * argStr = specific + 3;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                char * nextNr = strchr(argStr+3, ' ');
                uint8_t enabled = strtol(nextNr, NULL, 10);

                if((enabled == 0) || (enabled == 1)){
                    if(this->nvConfig->storeSensorConfigField(id, metric, sensorConfThresholdEnabled, enabled)){
                        SerialAT.println("OK");
                        commandProcessed = true;
                    }
                }
            }

            // Set sensor low threshold level 
            // Set sensor threshold enabled 
            if(strstr(specific, "TLL=")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                char * nextNr = strchr(argStr+3, ' ');
                uint16_t tll = strtol(nextNr, NULL, 10);

                if(this->nvConfig->storeSensorConfigField(id, metric, sensorConfThresholdLow, tll)){
                    SerialAT.println("OK");
                    commandProcessed = true;
                }
            }

            // Set sensor low threshold level 
            if(strstr(specific, "TLH=")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // same principle here

                char * nextNr = strchr(argStr+3, ' ');
                uint16_t tlh = strtol(nextNr, NULL, 10);

                if(this->nvConfig->storeSensorConfigField(id, metric, sensorConfThresholdHigh, tlh)){
                    SerialAT.println("OK");
                    commandProcessed = true;
                }
            }

            // Set sensor low threshold level 
            if(strstr(specific, "TEST")){
                SerialAT.println("OK");
                this->configureSensors();
                commandProcessed = true;
            }
        }

        if(!commandProcessed){
            SerialAT.println("ERROR");
        }

        this->atTimerEnabled = false;

        // clear the buffer
        memset(this->atCommand, '\0', AT_COMMAND_MAX_SIZE);
        this->atFill = 0;
        this->commandReceived = false;
    }
}

void NodeManager::configureSensors(void){
    for(uint8_t i=0; i<this->nrSensors; i++){
        SerialAT.print("Sensor ");
        SerialAT.println(i);
        // for each sensor metric
        for(uint8_t j=0; j<this->sensorList[i].getNrMetrics(); j++){
            // set thresholds
            uint8_t enabled;
            uint16_t tll;
            uint16_t tlh;
            this->nvConfig->getSensorThresholdSettings(i, j, &enabled, &tll, &tlh);
            this->sensorList[i].setTresholds(j, enabled, tll, tlh);
            // set poll interval
            uint16_t poll;
            this->nvConfig->getSensorPollInterval(i, j, &poll);
            this->sensorList[i].setPollInterval(j, poll);
        }
    }
}