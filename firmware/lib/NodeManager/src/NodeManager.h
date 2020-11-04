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
#include "device_settings.h"

// CONFIG_DURATION_MS defines how long the motherboard will listen for
// incoming AT commands before exiting configuration mode.
// Note: once anything is received over the serial configuration interface,
// the motherboard will remain in configuration mode until the AT+CLS command
// is received.
#define CONFIG_DURATION_MS              15000

// Maximum number of sensors that is being supported
#define MAX_NR_OF_SENSORS               6
// Safety check
#if MAX_NR_OF_SENSORS > 6
#error "The hardware does not support more than 6 sensors."
#endif

// Address range that is used to scan for sensors 
#define SENSOR_SCAN_START_ADDRESS       0x40
#define SENSOR_SCAN_END_ADDRESS         0x7F

// Timing settings
#define POLL_WAKEUP_INTERVAL            60 // every 60 seconds (1 minute)
//#define POLL_WAKEUP_INTERVAL            10 //
#define STATUS_MESSAGE_INTERVAL         21600 / POLL_WAKEUP_INTERVAL // every 21600 seconds (6 hours)
//#define STATUS_MESSAGE_INTERVAL         30 / POLL_WAKEUP_INTERVAL

// Message formatting (general)
#define MSG_TYPE_BYTE_IND               0
#ifndef LORA_MAX_PAYLOAD_SIZE
#error "LORA_MAX_PAYLOAD_SIZE is not defined."
#else
#define PAYLOAD_BUFFER_SIZE             LORA_MAX_PAYLOAD_SIZE
#endif

// Message formatting (status message)
#define STATUS_MSG_TYPE_BYTE            'S'
#define STATUS_MSG_MOTHERBOARD_ID_IND   1
#define STATUS_MSG_COUNTER_IND          3
#define STATUS_MSG_DATA_ACCUM_IND       5
#define STATUS_MSG_NR_SENSORS_IND       6
#define STATUS_MSG_ADDRESS_LIST_IND     7
#define STATUS_MSG_SIZE                 ( STATUS_MSG_ADDRESS_LIST_IND + MAX_NR_OF_SENSORS )

// Message formatting (immediate data)
#define IMMEDIATE_DATA_MSG_TYPE_BYTE    'I'
#define IMMEDIATE_DATA_MSG_ADDRESS_IND  1
#define IMMEDIATE_DATA_MSG_DATA_IND     2
#define IMMEDIATE_DATA_MSG_MAX_SIZE     ( IMMEDIATE_DATA_MSG_DATA_IND + ( 2 * MAX_NR_METRICS) )

// Message formatting (accumulated data)
#define ACCUM_DATA_MSG_TYPE_BYTE        'A'
#define ACCUM_DATA_MSG_ADDRESS_IND      1
#define ACCUM_DATA_MSG_SECS_ELAPSED_IND 2
#define ACCUM_DATA_MSG_DATA_IND         4
#define ACCUM_DATA_MSG_MAX_SIZE         ( ACCUM_DATA_MSG_DATA_IND + ( 2 * MAX_NR_METRICS) )

#ifndef LORA_ACCUMULATE_THRESHOLD
#define LORA_ACCUMULATE_THRESHOLD       PAYLOAD_BUFFER_SIZE / 2
#warning "LORA_ACCUMULATE_TRESHOLD was not defined. Using default setting."
#endif

#define AT_COMMAND_MAX_SIZE             32

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
    uint8_t payloadBufferFill = 0;
    
    bool doDataAccumulation;
    uint32_t accMsgCounter = 0;

    uint8_t statusMessage[STATUS_MSG_SIZE];
    uint8_t statusMessageSize;
    uint8_t iDataMessage[IMMEDIATE_DATA_MSG_MAX_SIZE];
    uint8_t aDataMessage[ACCUM_DATA_MSG_MAX_SIZE];

    int sleepRemaining = 0;
    int lastRtcWakeup;
    int statusCounter;
    int statusTimer;
    bool sendStatusMessage = false;
};

#endif /*__NODE_MANAGER_H__*/
