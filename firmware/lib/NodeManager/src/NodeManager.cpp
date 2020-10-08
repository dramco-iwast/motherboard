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
#include <ArduinoLowPower.h>
#include "NodeManager.h"
#include "DebugSerial.h"

// Maximum number of sensors that is being supported
#define MAX_NR_OF_SENSORS               6

// Address range that is used to scan for sensors 
#define SENSOR_IIC_BASE_ADDRESS         0x40
#define SENSOR_IIC_END_ADDRESS          0x7F


// List of interrupt pins
uint8_t intPins[MAX_NR_OF_SENSORS] = {A3, A4, 8, 9, 38, 3};

volatile bool sensorHasData[MAX_NR_OF_SENSORS] = {false, false, false, false, false, false};
volatile bool rtcWakeUp = false;

// rtc interrupt callback
void rtcCallback(void){
    rtcWakeUp = true;
}

// pin interrupt callbacks
void intPin0Callback(void){
    // ignore 'high' or 'rising edge' interrupt
    if(digitalRead(intPins[0])) return;

    // 'low' interrupt -> wait until sensors sets pin 'high' again
    while(!digitalRead(intPins[0]));
    sensorHasData[0] = true;
    DEBUG.println("cb0");
}

void intPin1Callback(void){
    // ignore 'high' or 'rising edge' interrupt
    if(digitalRead(intPins[1])) return;

    // 'low' interrupt -> wait until sensors sets pin 'high' again
    while(!digitalRead(intPins[1]));
    sensorHasData[1] = true;
    DEBUG.println("cb1");
}

void intPin2Callback(void){
    // ignore 'high' or 'rising edge' interrupt
    if(digitalRead(intPins[2])) return;

    // 'low' interrupt -> wait until sensors sets pin 'high' again
    while(!digitalRead(intPins[2]));
    sensorHasData[2] = true;
    DEBUG.println("cb2");
}

void intPin3Callback(void){
    // ignore 'high' or 'rising edge' interrupt
    if(digitalRead(intPins[3])) return;

    // 'low' interrupt -> wait until sensors sets pin 'high' again
    while(!digitalRead(intPins[3]));
    sensorHasData[3] = true;
    DEBUG.println("cb3");
}

void intPin4Callback(void){
    // ignore 'high' or 'rising edge' interrupt
    if(digitalRead(intPins[4])) return;

    // 'low' interrupt -> wait until sensors sets pin 'high' again
    while(!digitalRead(intPins[4]));
    sensorHasData[4] = true;
    DEBUG.println("cb4");
}

void intPin5Callback(void){
    // ignore 'high' or 'rising edge' interrupt
    if(digitalRead(intPins[5])) return;

    // 'low' interrupt -> wait until sensors sets pin 'high' again
    while(!digitalRead(intPins[5]));
    sensorHasData[5] = true;
    DEBUG.println("cb5");
}

// dummy callback for pin interrupt
void nullCb(void){
    return;
}

typedef void (*SensorCb)(void);
SensorCb sensorCallbacks[MAX_NR_OF_SENSORS] = {
    intPin0Callback,
    intPin1Callback,
    intPin2Callback,
    intPin3Callback,
    intPin4Callback,
    intPin5Callback
};

// for debugging purposes
void DUMP_EEPROM(int size){
    DEBUG.println("EEPROM DUMP");
    for(int i=0; i<size; i++){
        DEBUG.print("A: ");
        DEBUG.print(i, HEX);
        DEBUG.print(" - ");
        DEBUG.println(EEPROM.read(i), HEX);
    }
}

// constructor
NodeManager::NodeManager(uint16_t id){
    this->id = id;
    sprintf(this->idStr, "%04X", id);

    memset(this->atCommand, '\0', AT_COMMAND_MAX_SIZE);
    this->atFill = 0;
    this->commandReceived = false;

    this->atTimerEnabled = true;
    this->conFigMode = true;

    this->payloadBufferFill = 0;
    
    this->sleepRemaining = 60000;
    this->lastRtcWakeup = 0;

    this->nvConfig = new NonVolatileConfig();
    
    LowPower.setRtcTime(0);
}

void NodeManager::begin(void){
    DEBUG.println(F("Initializing NodeManager..."));

    DUMP_EEPROM(32);

    // 1. Initialize interrupts pins
    for(uint8_t i=0; i<MAX_NR_OF_SENSORS; i++){
        pinMode(intPins[i], INPUT_PULLUP);
        // assign dummy interrupt on rising edge
        // this is necessary because otherwise, using the "Low-Power"-lib seems to result
        // unhandled interrupts
        //attachInterrupt(intPins[i], nullCb, RISING);
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
            DEBUG.print(F("Found sensor at address: 0x"));
            DEBUG.println(address, HEX);
            if(detectedCount<MAX_NR_OF_SENSORS){
                detectedSensors[detectedCount++] = address;
            }
            else{
                DEBUG.println(F("Maximum number of sensors exceeded"));
            }
        }
    }
    // Delete temporary object
    delete t;

    // Set number of sensors connected to the motherboard
    this->nrSensors = detectedCount;
    DEBUG.print(F("Scan has found "));
    DEBUG.print(this->nrSensors);
    DEBUG.println(F(" sensors."));

    // Now create the actual list of sensors ...
    this->sensorList = new Sensor[this->nrSensors];
    // ... and initialize this list
    for(uint8_t i=0; i<this->nrSensors; i++){
        if(!this->sensorList[i].init(detectedSensors[i])){
            DEBUG.print(F("Failed to initialize sensor at address: 0x"));
            DEBUG.println(detectedSensors[i], HEX);
        }
        else{
            // detect which interrupt pin the i-th sensor is connected to
            this->sensorList[i].toggleInterrupt();
            delay(1);
            for(uint8_t s=0; s<MAX_NR_OF_SENSORS; s++){
                DEBUG.print("testing pin ");
                DEBUG.print(intPins[s]);
                if(digitalRead(intPins[s]) == LOW){
                    DEBUG.println(" - response");
                    // set high again before we attach interrupt, or it will block forever
                    this->sensorList[i].toggleInterrupt();

                    // update sensor information
                    this->sensorList[i].setIntPin(intPins[s]);
                    this->sensorList[i].setCbNr(s);
 
                    // detach previously attached 'dummy' callback
                    //detachInterrupt(this->sensorList[i].getIntPin());

                    // attach pin interrupt callback
                    //attachInterrupt(this->sensorList[i].getIntPin(), sensorCallbacks[s], LOW);
                    LowPower.attachInterruptWakeup(this->sensorList[i].getIntPin(), sensorCallbacks[s], FALLING);
                }
                else{
                    DEBUG.println("- no response");
                }
            }
            
            // print some debug information
            DEBUG.print("Sensor: ");
            DEBUG.println(i+1);
            DEBUG.print(" - type: 0x");
            DEBUG.println(this->sensorList[i].getSensorType(), HEX);
            DEBUG.print(" - metrics: ");
            DEBUG.println(this->sensorList[i].getNrMetrics());
            DEBUG.print(" - int pin: ");
            DEBUG.println(this->sensorList[i].getIntPin());
            DEBUG.print(" - callback: cb");
            DEBUG.println(this->sensorList[i].getCbNr());
        }
        delay(20);
    }

    // 4. Read a previous configuration from EEPROM
    DEBUG.println(F("Reading previous config from EEPROM ..."));
    this->nvConfig->readSensorConfig();
    this->nvConfig->printSensorConfig();

    // 5. Test if connected sensors (sensorList) match the previous configuration (nvConfig)
    bool diffDetected = false;
    if(this->nvConfig->getNrSensors() != this->nrSensors){
        diffDetected = true;
    }
    else{
        for(uint8_t i=0; i<this->nrSensors; i++){
            if(!this->nvConfig->sensorInConfig(i, this->sensorList[i].getIicAddress(), this->sensorList[i].getSensorType())){
                diffDetected = true;
                break;
            }
        }
    }
    
    // 6. If list of sensors from the previous configuration differs from the currently installed sensors
    // we completely discard the previous configuration and build a new one based on the sensorList
    if(diffDetected){
        DEBUG.println(F("Connected sensors don't match with previous config."));
        DEBUG.println(F("Creating new config ..."));
        // discard old config
        this->nvConfig->discardConfig();
        // build new config (in RAM)
        this->nvConfig->createNewConfig(this->nrSensors, this->sensorList);
        DEBUG.println(F("Updating non volatile config:"));
        this->nvConfig->storeSensorConfig();
    }
    else{
        DEBUG.println(F("Connected sensors match with previous config."));
    }
    this->nvConfig->printSensorConfig();

    this->atStartTime = millis();

    DEBUG.println("Done\n");
}

/* Run configuration interface over USB-serial
 * Setting forever to 'true' will keep it in config mode forever. This is only for testing purposes.
 * The default behaviour is to exit config mode when there is no serial communication within 
 * CONFIG_DURATION_MS. If there is, however, config mode can be exited by sending the "AT+CLS" command.
 */
void NodeManager::runConfigMode(bool forever){
    while(this->conFigMode || forever){
        // disable at commands after 10000
        if(this->atTimerEnabled){
            if(millis() - this->atStartTime > CONFIG_DURATION_MS){
                this->conFigMode = false;
            }
        }

        // Run the configuration interface
        this->processAtCommands();
    }

    // We have exited configuration mode
    // Next step is to store config settings, clean up and start normal operation.
    // Detach USB interface
    SerialAT.end();
    delay(1000);
	USBDevice.detach();

    DEBUG.println("Configuring connected sensors ...");
    this->configureSensors();

    DEBUG.println("Using configuration:");
    this->nvConfig->storeSensorConfig();
    delete this->nvConfig;

    DEBUG.println("Attach RTC callback.");
    LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, rtcCallback, CHANGE);
    this->lastRtcWakeup = LowPower.getRtcTime();

    DEBUG.println("Exit config mode.");
    delay(10);
}

void NodeManager::loop(void){
    // stuff to do on rtc wake-up
    if(rtcWakeUp){
        this->lastRtcWakeup = LowPower.getRtcTime();
        DEBUG.print(F("epoch: "));
        DEBUG.println(this->lastRtcWakeup);
        DEBUG.println("periodic wake-up");
        // update poll timers for all sensors
        for(uint8_t i=0; i<this->nrSensors; i++){
            /*DEBUG.print("sensor ");
            DEBUG.println(i);*/
            this->sensorList[i].updateTime();
        }

        rtcWakeUp = false;
    }
    
    // if we get here, either sensor pin interrupt or rtc interrupt has taken place
    if(this->dataAvailable()){
        // get data and add to payload
        this->getSensorData();
    }

    // update sleep-time remaining
    // if we got woken up by an sensor interrupt before the sleep time expires we need to keep sleeping
    int timeSinceLastRtcWakeup = (int)(LowPower.getRtcTime() - this->lastRtcWakeup);
    if(timeSinceLastRtcWakeup > 60){
        timeSinceLastRtcWakeup = 0;
    }
    DEBUG.print(F("Tdiff: "));
    DEBUG.println(timeSinceLastRtcWakeup);
    this->sleepRemaining = 60 - timeSinceLastRtcWakeup;

    if((this->sleepRemaining > 0) ){
        // sleep until wake-up event occurs
        DEBUG.print(F("going to sleep for: "));
        DEBUG.print(this->sleepRemaining);
        DEBUG.println(F(" s."));
        DEBUG.flush();

        // Enter standby mode
        LowPower.sleep(this->sleepRemaining*1000);
    }
}

uint8_t NodeManager::getLoraPayload(uint8_t * sendBuffer, uint8_t bufferSize){
    if(!sendBuffer){
        DEBUG.println("Buffer = NULL");
        return 0;
    }
    uint8_t bytesToCopy = 0;

    if(this->payloadBufferFill>bufferSize){
        bytesToCopy = bufferSize;
    }
    else{
        bytesToCopy = this->payloadBufferFill;
    }

    memcpy(sendBuffer, this->payloadBuffer, bytesToCopy);

    DEBUG.print("Copied ");
    DEBUG.print(bytesToCopy);
    DEBUG.println("bytes.");
    payloadBufferFill = 0;

    return bytesToCopy;
}

uint8_t NodeManager::payloadAvailable(void){
    return payloadBufferFill;
}

/*
bool NodeManager::dataAvailable(void){
    return (sensorHasData[0] | sensorHasData[1] | sensorHasData[2] | sensorHasData[3] | sensorHasData[4] | sensorHasData[5]);
}*/

// TODO: make private -> create packet
void NodeManager::getSensorData(void){
    for(uint8_t i=0; i<this->nrSensors; i++){
        uint8_t cbIndex = this->sensorList[i].getCbNr();
        if(sensorHasData[cbIndex]){
            sensorHasData[cbIndex] = false;
            uint8_t tempData[20];
            uint8_t len = 0;

            this->sensorList[i].readMeasurementData(tempData, &len);

            if((payloadBufferFill+len)<PAYLOAD_BUFFER_SIZE){
                // copy data to payload buffer
                payloadBuffer[payloadBufferFill++] = this->sensorList[i].getIicAddress();

                DEBUG.print("Sensor ");
                DEBUG.print(i);
                DEBUG.print(" len:" );
                DEBUG.print(len);
                DEBUG.print(" data: ");
                for(uint8_t j=0; j<len; j++){
                    if(tempData[j]<16){
                        DEBUG.print("0");
                    }
                    DEBUG.print(tempData[j], HEX);

                    payloadBuffer[payloadBufferFill++] = tempData[j];
                }
                DEBUG.println();
                DEBUG.print("timestamp: ");
                DEBUG.println(LowPower.getRtcTime());
            }
            else{
                DEBUG.println(F("Payload buffer is full!"));
            }
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
        AT_ErrorCodes_t errorCode = AT_UNDEFINED_ERROR;
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
                    uint8_t id = 0;
                    this->nvConfig->getSensorType(i, &type);
                    this->nvConfig->getSensorI2CAddress(i, &id);
                    sprintf(atOut, "%02X%02X", id, type);
                    SerialAT.print(atOut);
                }
                SerialAT.println();
                commandProcessed = true;
            }

            // Get sensor poll interval 
            if(strstr(specific, "POL?")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16); // sensor are identified by their i2c address
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)

                // look-up index of sensor with id in the nvConfig list
                // if the address (id) is not found, lookupIdIndex will return 0 (which results in ind = 255 > NR_SENSORS, this is ok)
                uint8_t ind = this->nvConfig->lookupIdIndex(id) - 1;
                DEBUG.print("index: ");
                DEBUG.println(ind);
                
                uint16_t pollInterval = 0;
                // get poll interval for (id, metric)
                if(this->nvConfig->getSensorPollInterval(ind, metric, &pollInterval)){ // false if either metric or id are invalid/incorrect
                    SerialAT.print("+POL: ");
                    SerialAT.println(pollInterval*60);
                    commandProcessed = true;
                }
                else{
                    errorCode = AT_INVALID_ID;
                }
            }

            // Set sensor poll interval 
            if(strstr(specific, "POL=")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16); // sensor are identified by their i2c address
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)

                // look-up index of sensor with id in the nvConfig list
                // if the address (id) is not found, lookupIdIndex will return 0 (which results in ind = 255 > NR_SENSORS, this is ok)
                uint8_t ind = this->nvConfig->lookupIdIndex(id) - 1;
                DEBUG.print("index: ");
                DEBUG.println(ind);

                char * nextNr = strchr(argStr+3, ' ');
                uint16_t pol = strtol(nextNr, NULL, 10);

                if(!(pol % 60)){
                    uint16_t polMinutes = (uint16_t)(pol / 60);
                    if(this->nvConfig->storeSensorConfigField(ind, metric, sensorConfPollInterval, polMinutes)){
                        SerialAT.println("OK");
                        commandProcessed = true;
                    }
                    else{
                        errorCode = AT_INVALID_ID;
                    }
                }
                else{
                    errorCode = AT_INVALID_POL;
                }
            
            }

            // Get sensor threshold settings
            if(strstr(specific, "TH?")){
                char * argStr = specific + 3;
                uint8_t id = strtol(argStr, NULL, 16); // sensor are identified by their i2c address
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)

                // look-up index of sensor with id in the nvConfig list
                // if the address (id) is not found, lookupIdIndex will return 0 (which results in ind = 255 > NR_SENSORS, this is ok)
                uint8_t ind = this->nvConfig->lookupIdIndex(id) - 1;
                DEBUG.print("index: ");
                DEBUG.println(ind);

                uint8_t enabled = 0;
                uint16_t tLevLow = 0;
                uint16_t tLevHigh = 0;
                // get threshold settings for (id, metric)
                if(this->nvConfig->getSensorThresholdSettings(ind, metric, &enabled, &tLevLow, &tLevHigh)){
                    SerialAT.print("+TH: ");
                    SerialAT.print(enabled);
                    SerialAT.print(" ");
                    SerialAT.print(tLevLow);
                    SerialAT.print(" ");
                    SerialAT.println(tLevHigh);
                    commandProcessed = true;
                }
                else{
                    errorCode = AT_INVALID_ID;
                }
            }

            // Set sensor threshold enabled 
            if(strstr(specific, "TE=")){
                char * argStr = specific + 3;
                uint8_t id = strtol(argStr, NULL, 16); // sensor are identified by their i2c address
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)

                char * nextNr = strchr(argStr+3, ' ');
                uint8_t enabled = strtol(nextNr, NULL, 10);

                // look-up index of sensor with id in the nvConfig list
                // if the address (id) is not found, lookupIdIndex will return 0 (which results in ind = 255 > NR_SENSORS, this is ok)
                uint8_t ind = this->nvConfig->lookupIdIndex(id) - 1;
                DEBUG.print("index: ");
                DEBUG.println(ind);

                if((enabled == 0) || (enabled == 1)){
                    if(this->nvConfig->storeSensorConfigField(ind, metric, sensorConfThresholdEnabled, enabled)){
                        SerialAT.println("OK");
                        commandProcessed = true;
                    }
                    else{
                        errorCode = AT_INVALID_ID;
                    }
                }
                else{
                    errorCode = AT_WRONG_VALUE;
                }
            }

            // Set sensor low threshold level 
            // Set sensor threshold enabled 
            if(strstr(specific, "TLL=")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16); // sensor are identified by their i2c address
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)

                char * nextNr = strchr(argStr+3, ' ');
                uint16_t tll = strtol(nextNr, NULL, 10);

                // look-up index of sensor with id in the nvConfig list
                // if the address (id) is not found, lookupIdIndex will return 0 (which results in ind = 255 > NR_SENSORS, this is ok)
                uint8_t ind = this->nvConfig->lookupIdIndex(id) - 1;
                DEBUG.print("index: ");
                DEBUG.println(ind);

                if(this->nvConfig->storeSensorConfigField(ind, metric, sensorConfThresholdLow, tll)){
                    SerialAT.println("OK");
                    commandProcessed = true;
                }
                else{
                    errorCode = AT_INVALID_ID;
                }
            }

            // Set sensor low threshold level 
            if(strstr(specific, "TLH=")){
                char * argStr = specific + 4;
                uint8_t id = strtol(argStr, NULL, 16); // sensor are identified by their i2c address
                uint8_t metric = strtol(argStr+3, NULL, 10) - 1; // argStr 00 will translate to 256 which is > MAX_NR_SENSORS (but no problem)

                char * nextNr = strchr(argStr+3, ' ');
                uint16_t tlh = strtol(nextNr, NULL, 10);

                // look-up index of sensor with id in the nvConfig list
                // if the address (id) is not found, lookupIdIndex will return 0 (which results in ind = 255 > NR_SENSORS, this is ok)
                uint8_t ind = this->nvConfig->lookupIdIndex(id) - 1;
                DEBUG.print("index: ");
                DEBUG.println(ind);

                if(this->nvConfig->storeSensorConfigField(ind, metric, sensorConfThresholdHigh, tlh)){
                    SerialAT.println("OK");
                    commandProcessed = true;
                }
                else{
                    errorCode = AT_INVALID_ID;
                }
            }

            // Set sensor data accumulation 
            if(strstr(specific, "ACC=")){
                char * argStr = specific + 4;
                uint8_t yesNo = strtol(argStr, NULL, 16); // sensor are identified by their i2c address

                this->nvConfig->setDataAccumulation(yesNo);
                SerialAT.println("OK");
                commandProcessed = true;
            }

            // Get sensor data accumulation 
            if(strstr(specific, "ACC?")){
                SerialAT.print("+ACC: ");
                SerialAT.println(this->nvConfig->getDataAccumulation());

                SerialAT.println("OK");
                commandProcessed = true;
            }

            /*// Set sensor low threshold level 
            if(strstr(specific, "TIME=")){ // AT+TIME=<epoch>
                char * argStr = specific + 5;
                uint32_t epoch = strtol(argStr, NULL, 10);

                SerialAT.print("epoch:" );
                SerialAT.println(epoch);

                if(epoch != 0){
                    LowPower.setRtcTime(epoch);
                    SerialAT.println("OK");
                    commandProcessed = true;
                }
                else{
                    errorCode = AT_WRONG_VALUE;
                }
            }

            // Set sensor low threshold level 
            if(strstr(specific, "TIME?")){ // AT+TIME?
                SerialAT.print("epoch:" );
                SerialAT.println(LowPower.getRtcTime());
                SerialAT.println("OK");

                commandProcessed = true;
            }*/

        }
        else{
            errorCode = AT_WRONG_COMMAND;
        }

        if(!commandProcessed){
            SerialAT.print("ERROR ");
            SerialAT.println(errorCode);
        }
        
        // show command received
        DEBUG.println(this->atCommand);

        this->atTimerEnabled = false;

        // clear the buffer
        memset(this->atCommand, '\0', AT_COMMAND_MAX_SIZE);
        this->atFill = 0;
        this->commandReceived = false;
    }
}

void NodeManager::configureSensors(void){
    for(uint8_t i=0; i<this->nrSensors; i++){
        DEBUG.print("Sensor ");
        DEBUG.println(i);
        // for each sensor metric
        for(uint8_t j=0; j<this->sensorList[i].getNrMetrics(); j++){
            // set thresholds
            uint8_t enabled;
            uint16_t tll;
            uint16_t tlh;
            this->nvConfig->getSensorThresholdSettings(i, j, &enabled, &tll, &tlh);
            this->sensorList[i].setThresholds(j, enabled, tll, tlh);
            // set poll interval
            uint16_t poll;
            this->nvConfig->getSensorPollInterval(i, j, &poll);
            this->sensorList[i].setPollInterval(j, poll);
            DEBUG.print("metric ");
            DEBUG.print(j);
            DEBUG.print(" - ");
            DEBUG.print(poll);
            DEBUG.print(" - ");
            DEBUG.print(enabled);
            DEBUG.print(" - ");
            DEBUG.print(tll);
            DEBUG.print(" - ");
            DEBUG.println(tlh);
            delay(10);
        }
    }
}

bool NodeManager::dataAvailable(void){
    return (sensorHasData[0] | sensorHasData[1] | sensorHasData[2] | sensorHasData[3] | sensorHasData[4] | sensorHasData[5]);
}