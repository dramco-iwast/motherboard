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

#include <Sensor.h>

#define POLL_RETRIES  2
#define POLL_ACK      0xAA

// Commands to interface with the sensors
#define CMD_POLL                    0x10 // see if a sensor responds
#define CMD_DEVICE_TYPE             0x20 // request the sensor device type
#define CMD_INT_TOGGLE              0x30 // request toggle (on the sensor of interrupt line)
#define CMD_GET_M_DATA_LEN          0x40 // check how many bytes/measurement we get from this sensor
#define CMD_GET_M_DATA              0x41 // get measurement
#define CMD_GET_C_DATA_LEN          0x50 // check how many bytes of control data we can send
#define CMD_SET_C_DATA              0x51 // send control bytes (e.g. treshold levels)
#define CMD_START_MEASUREMENT       0x60 // start a measurement
#define CMD_GET_READY               0x61 // check if sensor measurement data is ready

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
  delay(5); // give the device some time to return the data

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
  uint8_t stat = Wire.endTransmission();
  if(stat){
    return false;
  }

  if(len != 0){
    Wire.beginTransmission(address);
    Wire.write(data, len);
    stat = Wire.endTransmission();
  }

  if(stat){
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
  this->cLen = 0;
  this->mData = NULL;

  this->_numErrors = 0;
}

/* Poll the I2C bus to see if there's a sensor with an address "iicAddress".
 */
bool Sensor::pollAddress(uint8_t iicAddress){
  int retries = 0;
  uint8_t pollResponse = 0x00;
  do{ // POLL sensor
    if(readSensorData(iicAddress, CMD_POLL, &pollResponse, 1)){
      if(pollResponse==POLL_ACK){
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
  if(!this->requestRxLen()){
    // something went wrong
    return false;
  }

  // request the tx len (will update this->_cLen )
  if(!this->requestTxLen()){
    // something went wrong
    return false;
  }

  // allocate memory to store rx data
  this->mData = (uint8_t *) malloc(this->mLen * sizeof(uint8_t));

  return true;
}


/* Toggle the sensor's interrupt line
 * This necessary to detect which sensor is connected to which interrupt line.
 */
bool Sensor::toggleInterrupt(void){
  uint8_t ack = 0x00;
  if(!writeSensorData(this->iicAddress, CMD_INT_TOGGLE, &ack, 1)){
    return false;
  }
  return (bool)(ack == POLL_ACK);
}

bool Sensor::startMeasurement(void){
  uint8_t ack = 0x00;
  if(!writeSensorData(this->iicAddress, CMD_START_MEASUREMENT, &ack, 1)){
    return false;
  }
  return (bool)(ack == POLL_ACK);
}

bool Sensor::requestMeasurementData(void){
  // Wait until ready mechanism not implemented

  uint8_t tempData[this->mLen];

  bool rv = readSensorData(this->iicAddress, CMD_GET_M_DATA, tempData, this->mLen);
  Serial.print("sensor->_mData:");
  for(uint8_t i=0; i<this->mLen; i++){
    if(this->mData[i] > 0x0F){
      Serial.print(" ");
    }
    else{
      Serial.print(" 0");
    }
    Serial.print(tempData[i], HEX);
  }
  Serial.println();

  return rv;
}

void Sensor::copyMeasurementData(uint8_t * buf, uint8_t len){
  uint8_t mLen = len;
  if(mLen > this->mLen){
    mLen = this->mLen;
  }
  for(uint8_t i=0; i<mLen; i++){
    *(buf+i) = *(this->mData+i);
  }
}

bool Sensor::writeControlData(uint8_t * buf, uint8_t len){
  if(len<this->cLen){
    return writeSensorData(this->iicAddress, CMD_SET_C_DATA, buf, len);
  }
 else{
    return writeSensorData(this->iicAddress, CMD_SET_C_DATA, buf, this->cLen);
  }
}


/* Access to sensor attributes ************************************************/

uint8_t Sensor::getIicAddress(void){
  return this->iicAddress;
}

uint8_t Sensor::getSensorType(void){
  return this->sensorType;
}

uint8_t Sensor::getRxLen(void){
  return this->mLen;
}

uint8_t Sensor::getTxLen(void){
  return this->cLen;
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

bool Sensor::requestRxLen(void){
  uint8_t rxLen;
  if(!readSensorData(this->iicAddress, CMD_GET_M_DATA_LEN, &rxLen, 1)){
    return false;
  }

  this->mLen = rxLen;

  return true;
}
  
bool Sensor::requestTxLen(void){
  uint8_t txLen;
  if(!readSensorData(this->iicAddress, CMD_GET_C_DATA_LEN, &txLen, 1)){
    return false;
  }

  this->cLen = txLen;

  return true;
}
  
bool Sensor::checkReady(void){
  uint8_t ready = 0x00;
  if(!readSensorData(this->iicAddress, CMD_GET_READY, &ready, 1)){
    Serial.println(ready, HEX);
    return false;
  }

  if(ready == 0x01){
    Serial.println(ready, HEX);
    return true;
  }

  return false;
}
