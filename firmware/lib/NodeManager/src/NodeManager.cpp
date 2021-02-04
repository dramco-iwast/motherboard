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
 *      Version: 1.1
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
#include <Adafruit_SleepyDog.h>
#include "NodeManager.h"
#include "DebugSerial.h"

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
    
    this->sleepRemaining = 0;
    this->lastRtcWakeup = 0;
    this->statusTimer = STATUS_MESSAGE_INTERVAL-1;
    this->statusCounter = 0;

    this->nvConfig = new NonVolatileConfig();
    
    LowPower.setRtcTime(0);
}

void NodeManager::begin(void){
    DEBUG.println(F("Initializing NodeManager..."));

    //DUMP_EEPROM(32);

    // 1. Initialize interrupts pins
    for(uint8_t i=0; i<MAX_NR_OF_SENSORS; i++){
        pinMode(intPins[i], INPUT_PULLUP);
    }

    // 2. Initialize I2C bus
    delay(500);
    Wire.begin();
    Wire.setClock(100000);

    // 3. Build a sensor list (i.e. list of Sensor objects for interfacing with the sensor boards)
    uint8_t detectedSensors[MAX_NR_OF_SENSORS];
    uint8_t detectedCount = 0;
    Sensor * t =  new Sensor(); // temporary sensor object for interfacing with the bus

    // Test all addresses for a response
    for(uint8_t address=SENSOR_SCAN_START_ADDRESS; address<SENSOR_SCAN_END_ADDRESS+1; address++){
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
 
                    // attach pin interrupt callback
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

    this->atStartTime = millis();

    DEBUG.println("Done\n");
}

/* Run configuration interface over USB-serial
 * Setting forever to 'true' will keep it in config mode forever. This is only for testing purposes.
 * The default behaviour is to exit config mode when there is no serial communication within 
 * CONFIG_DURATION_MS. If there is, however, config mode can be exited by sending the "AT+CLS" command.
 */
void NodeManager::runConfigMode(bool skip){
    bool firstCommandProcessed = false;
    this->configUpdated = false;
    if(!skip){
        while(this->conFigMode){
            // disable at commands after 10000
            if(this->atTimerEnabled){
                if(millis() - this->atStartTime > CONFIG_DURATION_MS){
                    this->conFigMode = false;
                }
            }

            // Run the configuration interface
            if(this->processAtCommands()){
                // on the first command that is received (normally this is AT+PNG?) populate the config
                if(!firstCommandProcessed){
                    firstCommandProcessed = true;
                    // 1. Read a previous configuration from EEPROM
                    DEBUG.println(F("Reading previous config from EEPROM ..."));
                    this->nvConfig->readSensorConfig();
                    this->nvConfig->printSensorConfig();

                    // 2. Test if connected sensors (sensorList) match the previous configuration (nvConfig)
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

                    // 3. If list of sensors from the previous configuration differs from the currently installed sensors
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
                }
            }
        }
    }

    // We have exited configuration mode
    // Next step is to store config settings, clean up and start normal operation.
    DEBUG.println(F("Disconnecting USB ..."));
    SerialAT.end();
    delay(1000);
	USBDevice.detach();
    USBDevice.end();

    if(this->configUpdated){
        DEBUG.println(F("Using configuration:"));
        this->nvConfig->storeSensorConfig();
    }
    else{
        DEBUG.println(F("Reading previous config from EEPROM ..."));
        this->nvConfig->readSensorConfig();
        this->nvConfig->printSensorConfig();
    }

    DEBUG.println(F("Configuring connected sensors ..."));
    this->configureSensors();
    this->doDataAccumulation = (this->nvConfig->getDataAccumulation() != 0);
    delete this->nvConfig;

    DEBUG.println(F("Attach RTC callback."));
    LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, rtcCallback, CHANGE);

    DEBUG.println(F("Updating status message."));
    this->initStatusMessage();

    DEBUG.println(F("Exit config mode."));
    delay(10);
}

void NodeManager::loop(void){
    // we are awake
    Watchdog.reset();
    Watchdog.enable(10000);

    // stuff to do on rtc wake-up
    if(rtcWakeUp){
        this->lastRtcWakeup = LowPower.getRtcTime();
        DEBUG.print(F("epoch: "));
        DEBUG.println(this->lastRtcWakeup);
        DEBUG.println("periodic wake-up");
        // update poll timers for all sensors
        for(uint8_t i=0; i<this->nrSensors; i++){
            DEBUG.print("Sensor ");
            DEBUG.println(i);
            this->sensorList[i].updateTime();
        }
        this->statusTimer++;
        if(this->statusTimer == STATUS_MESSAGE_INTERVAL){
            this->statusTimer = 0;
            // build status message
#ifndef NO_STATUS_MESSAGES            
            DEBUG.println("Status update needed.");
            this->updateStatusMessage(this->statusCounter);
            this->statusCounter++;
#endif
        }

        rtcWakeUp = false;
    }
    
    // if we get here, either sensor pin interrupt or rtc interrupt has taken place
    if(this->dataAvailable()){
        // get data and add to payload
        this->getSensorData();
    }

    DEBUG.println(F("End of NodeManager loop() -> feeding the watchdog"));
    Watchdog.reset();
}

bool NodeManager::watchdogReset(void){
    if(Watchdog.resetCause() & PM_RCAUSE_WDT){
        DEBUG.println(F("Watchdog reset happend."));
        Wire.end(); // needed? and should it be here?
        return true;
    }
    return false;
}

void NodeManager::sleep(void){
    bool eventHandled = false;
    // Handle pending interrupts first
    if(this->dataAvailable()){
        // get data and add to payload
        this->getSensorData();
        eventHandled = true;
    }

    // 
    if(this->statusTimer == 0){
        eventHandled = true;
    }

    // update sleep-time remaining
    // if we got woken up by an sensor interrupt before the sleep time expires we need to keep sleeping
    int timeSinceLastRtcWakeup = (int)(LowPower.getRtcTime() - this->lastRtcWakeup);
     if(this->lastRtcWakeup == 0){
        DEBUG.println(F("First time sleep."));
        timeSinceLastRtcWakeup = 0;
        this->lastRtcWakeup = LowPower.getRtcTime();
    }
    DEBUG.print(F("Time elapsed since last wakeup: "));
    DEBUG.print(timeSinceLastRtcWakeup);
    DEBUG.println(F(" s."));

    DEBUG.println(F("Watchdog will fire in 16 s."));
    Watchdog.reset();
    Watchdog.enable(16000);

    int sleepFor = POLL_WAKEUP_INTERVAL;
    if(eventHandled){
        sleepFor -= timeSinceLastRtcWakeup;

        // Set new RTC alarm and go to sleep
        DEBUG.print(F("Sleep for "));
        DEBUG.print(sleepFor);
        DEBUG.println(F(" s."));
        DEBUG.flush();

        // Enter standby mode
        LowPower.sleep((sleepFor-1)*1000);
    }
    else{
        if((timeSinceLastRtcWakeup > 0) && (timeSinceLastRtcWakeup < POLL_WAKEUP_INTERVAL)){
            // Sleep until previously set alarm
            DEBUG.println(F("Back to sleep"));
            DEBUG.flush();
            LowPower.sleep();
        }
        else{
            if(timeSinceLastRtcWakeup > POLL_WAKEUP_INTERVAL){
                DEBUG.print(F("Something went wrong, best not to nap for to long."));
                sleepFor = 1;
            }
            // Set new RTC alarm and go to sleep
            DEBUG.print(F("Sleep for "));
            DEBUG.print(sleepFor);
            DEBUG.println(F(" s."));
            DEBUG.flush();

            // Enter standby mode
            LowPower.sleep((sleepFor-1)*1000);
        }
    }

}

uint8_t NodeManager::getLoraPayload(uint8_t * sendBuffer, uint8_t bufferSize){
    if(!sendBuffer){
        DEBUG.println(F("Buffer = NULL"));
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
    DEBUG.println(" bytes.");
    payloadBufferFill = 0;

    this->accMsgCounter = 0;

    return bytesToCopy;
}

uint8_t NodeManager::payloadAvailable(bool * isStatus){
    // if the payload buffer contains a status message -> send regardless of data accumulation
    if(this->sendStatusMessage){
        this->sendStatusMessage = false;
        *isStatus = true;
        return this->payloadBufferFill;
    }

    *isStatus = false;

    // only data (and data accumulation is enabled)
    if(this->doDataAccumulation){
        // only send if enough data in the buffer
        if(this->payloadBufferFill > LORA_ACCUMULATE_THRESHOLD){
            return this->payloadBufferFill;
        }
        else{
            return 0;
        }
    }

    return this->payloadBufferFill;
}

bool NodeManager::dataAccumulationEnabled(void){
    return this->doDataAccumulation;
}

// Read from each sensor that has data available and copy data to payload buffer
void NodeManager::getSensorData(void){
    // compose immediate data message
    // loop through sensors in the list
    for(uint8_t i=0; i<this->nrSensors; i++){
        // find callback assigned to sensor
        uint8_t cbIndex = this->sensorList[i].getCbNr();
        // see which sensor has data available
        if(sensorHasData[cbIndex]){
            sensorHasData[cbIndex] = false;

            uint8_t len = 0;

            if(this->doDataAccumulation){
                // format accumulated data message
                this->aDataMessage[MSG_TYPE_BYTE_IND] = ACCUM_DATA_MSG_TYPE_BYTE;
                this->aDataMessage[ACCUM_DATA_MSG_ADDRESS_IND] = this->sensorList[i].getIicAddress();
                uint32_t curEpoch = LowPower.getRtcTime();
                uint16_t elapsed = 0;

                if(this->accMsgCounter != 0){
                    elapsed = (uint16_t)(curEpoch - this->accMsgCounter);
                }
                this->accMsgCounter = curEpoch;

                this->aDataMessage[ACCUM_DATA_MSG_SECS_ELAPSED_IND] = (uint8_t)(elapsed >> 8);
                this->aDataMessage[ACCUM_DATA_MSG_SECS_ELAPSED_IND+1] = (uint8_t)(elapsed & 0x00FF);

                this->sensorList[i].readMeasurementData(this->aDataMessage+ACCUM_DATA_MSG_DATA_IND, &len);

                // copy to buffer
                if(this->copyToPayloadBuffer(this->aDataMessage, len+ACCUM_DATA_MSG_DATA_IND)){
                    DEBUG.print(F("Accumulated data message: "));
                    DEBUG.printHexBuf(this->aDataMessage, len+ACCUM_DATA_MSG_DATA_IND);
                }

            }
            else{
                // format immediate data message
                this->iDataMessage[MSG_TYPE_BYTE_IND] = IMMEDIATE_DATA_MSG_TYPE_BYTE;
                this->iDataMessage[IMMEDIATE_DATA_MSG_ADDRESS_IND] = this->sensorList[i].getIicAddress();
                this->sensorList[i].readMeasurementData(this->iDataMessage+IMMEDIATE_DATA_MSG_DATA_IND, &len);

                // copy to buffer
                if(this->copyToPayloadBuffer(this->iDataMessage, len+IMMEDIATE_DATA_MSG_DATA_IND)){
                    DEBUG.print(F("Immediate data message: "));
                    DEBUG.printHexBuf(this->iDataMessage, len+IMMEDIATE_DATA_MSG_DATA_IND);
                }
            }
        }
    }
}

bool NodeManager::copyToPayloadBuffer(uint8_t * buffer, uint8_t bufferSize){
    // DEBUG output
    DEBUG.print(F("Current payload size: is "));
    DEBUG.println(this->payloadBufferFill);
    DEBUG.print(F("Adding another "));
    DEBUG.println(bufferSize);
    DEBUG.print(F("Copying buffer: "));
    DEBUG.printHexBuf(buffer, bufferSize);


    if((this->payloadBufferFill+bufferSize)>PAYLOAD_BUFFER_SIZE){
        DEBUG.println(F("Payload buffer is full!"));
        return false;
    }
 
    memcpy((void*)(payloadBuffer+this->payloadBufferFill), (void *)buffer, bufferSize);
    this->payloadBufferFill += bufferSize;

    return true;
}

bool NodeManager::processAtCommands(void){
    bool commandProcessed = false;
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
        AT_ErrorCodes_t errorCode = AT_UNDEFINED_ERROR;
        // check for at command
        if(strstr(this->atCommand, "AT+")){
            char * specific = this->atCommand+3;

            // Ping motherboard
            if(strstr(specific, "CLS")){
                SerialAT.println("OK");
                commandProcessed = true;
                this->conFigMode = false;
                this->configUpdated = true;
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
                    SerialAT.println(pollInterval*POLL_WAKEUP_INTERVAL);
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

                if(!(pol % POLL_WAKEUP_INTERVAL)){
                    uint16_t polMinutes = (uint16_t)(pol / POLL_WAKEUP_INTERVAL);
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
    return commandProcessed;
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
            if(this->nvConfig->getSensorThresholdSettings(i, j, &enabled, &tll, &tlh)){
                this->sensorList[i].setThresholds(j, enabled, tll, tlh);
            }
            // set poll interval
            uint16_t poll;
            if(this->nvConfig->getSensorPollInterval(i, j, &poll)){
                this->sensorList[i].setPollInterval(j, poll);
            }
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


void NodeManager::initStatusMessage(void){
    this->statusMessage[MSG_TYPE_BYTE_IND] = STATUS_MSG_TYPE_BYTE;
    
    this->statusMessage[STATUS_MSG_MOTHERBOARD_ID_IND] = (uint8_t)(this->id >> 8);
    this->statusMessage[STATUS_MSG_MOTHERBOARD_ID_IND+1] = (uint8_t)(this->id & 0x00FF);
    
    if(this->doDataAccumulation){
        this->statusMessage[STATUS_MSG_DATA_ACCUM_IND] = 0x01;
    }
    else{
        this->statusMessage[STATUS_MSG_DATA_ACCUM_IND] = 0x00;
    }

    this->statusMessage[STATUS_MSG_NR_SENSORS_IND] = this->nrSensors;
    for(uint8_t i=0; i<this->nrSensors; i++){
        this->statusMessage[STATUS_MSG_ADDRESS_LIST_IND+i] = this->sensorList[i].getIicAddress();
    }

    this->statusMessageSize = STATUS_MSG_ADDRESS_LIST_IND + this->nrSensors;
}


void NodeManager::updateStatusMessage(uint16_t ctr, bool rescan){
    this->statusMessage[STATUS_MSG_COUNTER_IND] = (uint8_t)(ctr >> 8);
    this->statusMessage[STATUS_MSG_COUNTER_IND+1] = (uint8_t)(ctr & 0x00FF);

    if(rescan){
        // TODO: poll sensors again
    }

    // copy to buffer
    if(this->copyToPayloadBuffer(this->statusMessage, this->statusMessageSize)){
        DEBUG.print(F("Status message: "));
        DEBUG.printHexBuf(this->statusMessage, this->statusMessageSize);
    }

    this->sendStatusMessage = true;
}
