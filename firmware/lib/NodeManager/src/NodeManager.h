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

#ifndef __NODE_MANAGER_H__
#define __NODE_MANAGER_H__

#include <Arduino.h>
#include "NonVolatileConfig.h"
#include "Sensor.h"

#define CONFIG_DURATION_MS  5000

#define AT_COMMAND_MAX_SIZE 32
#define PAYLOAD_BUFFER_SIZE 64
#define STATUS_MESSAGE_SIZE 16

#define SerialAT SerialUSB

typedef enum errorCodes{
    AT_UNDEFINED_ERROR = 0,
    AT_INVALID_ID = 1,
    AT_INVALID_POL = 2,
    AT_WRONG_VALUE = 3,
    AT_WRONG_COMMAND = 4
} AT_ErrorCodes_t;

class NodeManager{

public:
    // constructor - id is unique identifier for the motherboard
    NodeManager(uint16_t id);

    // setup operation
    //  - scans connected sensors
    //  - configures lorawan modem
    //  - start timers
    //  - etc.
    void begin(void);

    void runConfigMode(bool forever=false);

    // main operation
    void loop(void);
    void sleep(void);
    uint8_t payloadAvailable(void); // return payload size
    uint8_t getLoraPayload(uint8_t * sendBuffer, uint8_t bufferSize);
    bool dataAccumulationEnabled(void);

private:
    void processAtCommands(void);
    void configureSensors(void);
    bool dataAvailable(void);
    void getSensorData(void);

    void initStatusMessage(void);
    void updateStatusMessage(uint16_t ctr, bool rescan=false);
    bool copyToPayloadBuffer(uint8_t * buffer, uint8_t bufferSize);

    NonVolatileConfig * nvConfig;

    // motherboard id
    uint16_t id;    // as 16-bit unsigned integer
    char idStr[5];  // as string (4-digit hex)

    // connected sensors
    uint8_t nrSensors;
    Sensor * sensorList;

    // AT command processing
    char atCommand[AT_COMMAND_MAX_SIZE];     // a String to hold incoming data
    uint8_t atFill;
    bool commandReceived;   // whether the string is complete

    unsigned long atStartTime;
    bool conFigMode;
    bool atTimerEnabled;

    uint8_t payloadBuffer[PAYLOAD_BUFFER_SIZE];
    uint8_t payloadBufferFill;
    
    bool doDataAccumulation;

    uint8_t statusMessage[STATUS_MESSAGE_SIZE];
    uint8_t statusMessageLength;

    int sleepRemaining = 0;
    int lastRtcWakeup;
    int statusCounter;
    int statusTimer;
};

#endif /*__NODE_MANAGER_H__*/
