/*  ____  ____      _    __  __  ____ ___
 * |  _ \|  _ \    / \  |  \/  |/ ___/ _ \
 * | | | | |_) |  / _ \ | |\/| | |  | | | |
 * | |_| |  _ <  / ___ \| |  | | |__| |_| |
 * |____/|_| \_\/_/   \_\_|  |_|\____\___/
 *                           research group
 *                             dramco.be/
 *
 *   KU Leuven - Technology Campus Gent,
 *   Gebroeders De Smetstraat 1,
 *   B-9000 Gent, Belgium
 *
 *         File: Sensor.cpp
 *      Created: 2018-05-02
 *       Author: Geoffrey Ottoy
 *      Version: 4.1
 *
 *  Description: Implementation of the Sensor class for interfacing with
 *    individual sensors.
 *
 *  License (T.B.D.)
 */


#include <Wire.h>
#include "Sensor.h"
#include "DebugSerial.h"

#define POLL_RETRIES  2

#define DEFAULT_ACK                 0xAA

// i2c slave device commands
#define CMD_POLL                    0x10 // see if a sensor responds
#define CMD_DEVICE_TYPE             0x11 // request the sensor device type
#define CMD_INT_TOGGLE              0x15 // request toggle (on the sensor of interrupt line)
#define CMD_GET_M_NR                0x23 // get nr. of metrics
#define CMD_GET_M_DATA              0x14 // get measurement
#define CMD_START_MEASUREMENT       0x13 // get/start a measurement
#define CMD_SET_THRESHOLDS          0x24 // set thresholds

/******************************************************************************
 * Static functions (IIC communication)
 ******************************************************************************/

/* Internal function: reads data from a profile sensor using I2C.
 */
static bool readSensorData(uint8_t address, uint8_t command, uint8_t * data, uint8_t len){
  // Send command to specify the data that is being read
  Wire.beginTransmission(address);
  Wire.write(command);
  //delay(1); // give the device some time to return the data
  uint8_t stat = Wire.endTransmission();
  if(stat){
    /*Serial.print("addr: 0x");
    Serial.println(address, HEX);
    Serial.print("write (read) end transm.: ");
    Serial.println(stat);*/
    return false;
  }

  // Read the data returned by the device
  delay(10); // give the device some time to return the data

  Wire.requestFrom((uint8_t)address, (uint8_t)len);
  int i=len;

  while(Wire.available() && i--){
    *(data++) = Wire.read();
  }

  if(i){
    /*Serial.print("addr: 0x");
    Serial.println(address, HEX);
    Serial.print("read end. no more data: ");
    Serial.println(i);*/
    return false;
  }

  return true;
}

/* Internal function: writes data to a profile sensor using I2C.
 */
static bool writeSensorData(uint8_t address, uint8_t command, uint8_t * data, uint8_t len){
  Wire.beginTransmission(address);
  Wire.write(command);

  if(len != 0){
    Wire.write(data, len);
  }

  if(Wire.endTransmission()){
    return false;
  }

  return true;
}

/******************************************************************************
 * Methods for individual Sensor manipulation
 ******************************************************************************/

/* General functionality ******************************************************/

/* Object constructor (basic initialization).
 */
Sensor::Sensor(void){
  this->iicAddress = 0;
  this->sensorType = 0;

  this->mLen = 0;

  this->_numErrors = 0;
}

/* Poll the I2C bus to see if there's a sensor with an address "iicAddress".
 */
bool Sensor::pollAddress(uint8_t iicAddress){
  int retries = 0;
  uint8_t pollResponse = 0x00;
  do{ // POLL sensor
    if(readSensorData(iicAddress, CMD_POLL, &pollResponse, 1)){
      if(pollResponse==DEFAULT_ACK){
        /*Serial.print("Sensor at address: 0x");
        Serial.println(iicAddress, HEX);*/
        
        return true;
      }
    }
    retries++;
    delay(20);
  } while (retries < POLL_RETRIES);
  
  return false;
}

bool Sensor::init(uint8_t iicAddress){
  // initialize address;
  this->iicAddress = iicAddress;

  // request the sensor type (will update this->_sensorType )
  if(!this->requestSensorType()){
    // something went wrong
    return false;
  }

  // request the rx len (will update this->_mLen )
  if(!this->requestNrMetrics()){
    // something went wrong
    return false;
  }

  // set length of al measurements combined
  this->mLen = this->nrMetrics * 2;
  // allicate memory for timing the measurements
  this->pollInterval = (uint16_t *)malloc(sizeof(uint16_t)*this->nrMetrics);
  this->pollTimer = (uint16_t *)malloc(sizeof(uint16_t)*this->nrMetrics);

  return true;
}


/* Toggle the sensor's interrupt line
 * This necessary to detect which sensor is connected to which interrupt line.
 */
bool Sensor::toggleInterrupt(void){
  return writeSensorData(this->iicAddress, CMD_INT_TOGGLE, (uint8_t*)0, 0);
}

/* Start a new measurement on the sensor (polled operation)
 * TODO: specify metrics? or do metric filtering here?
 */
bool Sensor::startMeasurement(void){
  return writeSensorData(this->iicAddress, CMD_START_MEASUREMENT, (uint8_t*)0, 0);
}

/* Read all measurement data from the sensor
 * TODO: for all metrics?
 */
bool Sensor::readMeasurementData(uint8_t * buf, uint8_t * len){
  bool rv = readSensorData(this->iicAddress, CMD_GET_M_DATA, buf, this->mLen);
  * len = this->mLen;

  return rv;
}

/* Send threshold settings for 'metric' to the sensor
 */
bool Sensor::setThresholds(uint8_t metric, uint8_t enabled, uint16_t tLevelLow, uint16_t tLevelHigh){
  uint8_t data[6];
  data[0] = metric;
  data[1] = enabled;
  data[2] = (uint8_t)(tLevelLow >> 8);
  data[3] = (uint8_t)(tLevelLow & 0x00FF);
  data[4] = (uint8_t)(tLevelHigh >> 8);
  data[5] = (uint8_t)(tLevelHigh & 0x00FF);

  return writeSensorData(this->iicAddress, CMD_SET_THRESHOLDS, data, 6);
}

/* Send poll interval for 'metric'
 * For 'poll' = 0, polling is disabled (sensor will only supply data if thresholds are enabled)
 */
void Sensor::setPollInterval(uint8_t metric, uint16_t poll){
  this->pollInterval[metric] = poll;
  this->pollTimer[metric] = poll;
}

/* start a new measurement if poll timer has expired
 * TODO: metric filtering
 */
void Sensor::updateTime(){
  for(uint8_t i=0; i<this->nrMetrics; i++){
    DEBUG.print(" - metric ");
    DEBUG.print(i);
    DEBUG.print(": ");
    if(this->pollInterval[i] > 0){
      this->pollTimer[i]--;
      if(this->pollTimer[i] == 0){
        this->pollTimer[i] = this->pollInterval[i];
        this->startMeasurement(); // TODO: specify metric
        // start measurement results in pin interrupt
        DEBUG.print("measure ");
        DEBUG.print(this->mLen);
        DEBUG.println(" bytes");
      }
      else{
        DEBUG.println("timer updated");
      }
    }
    else{
      DEBUG.println("no polling");
    }
  }
  DEBUG.println();
}

/* Access to sensor attributes ************************************************/

uint8_t Sensor::getIicAddress(void){
  return this->iicAddress;
}

uint8_t Sensor::getSensorType(void){
  return this->sensorType;
}

uint8_t Sensor::getNrMetrics(void){
  return this->nrMetrics;
}

uint8_t Sensor::getIntPin(void){
  return this->intPin;
}

uint8_t Sensor::getCbNr(void){
  return this->cbNr;
}

void Sensor::setIntPin(uint8_t pinNr){
  this->intPin = pinNr;
}

void Sensor::setCbNr(uint8_t cb){
  this->cbNr = cb;
}

/* Private methods ************************************************************/

/* Request the sensor type and intialize the _sensorType attribute
 */
bool Sensor::requestSensorType(void){
  uint8_t devType = 0x00;
  int retries = 0;

  do{ // Read sensor type
    readSensorData(this->iicAddress, CMD_DEVICE_TYPE, &devType, 1);
    if(devType != 0x00){
      this->sensorType = devType;

      return true;
    }
    retries++;
    delay(5);
  } while (retries < POLL_RETRIES);

  return false;
}

bool Sensor::requestNrMetrics(void){
  uint8_t nm;
  if(!readSensorData(this->iicAddress, CMD_GET_M_NR, &nm, 1)){
    return false;
  }

  this->nrMetrics = nm;

  return true;
}
