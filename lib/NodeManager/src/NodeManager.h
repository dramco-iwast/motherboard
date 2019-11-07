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
 *      Version: 0.1
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
#include <RTCZero.h>
#include "NonVolatileConfig.h"
#include "Sensor.h"

#define AT_COMMAND_MAX_SIZE 32

#define SerialAT SerialUSB

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
    bool dataAvailable(void);
    void getSensorData(uint8_t * data, uint8_t * len);

private:
    void processAtCommands(void);
    void configureSensors(void);

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


};



#endif /*__NODE_MANAGER_H__*/