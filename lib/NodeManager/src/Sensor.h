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
 *         File: Sensor.h
 *      Created: 2018-05-02
 *       Author: Geoffrey Ottoy
 *      Version: 4.1
 *
 *  Description: Header file for Sensor.cpp. Defines the Sensor class for
 *    interfacing with individual sensors.
 *
 *  License (T.B.D.)
 */

#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <Arduino.h>


/* The Sensor class specifies a common interface to all the sensors 
 * connected to the motherboad.
 */
class Sensor {
public:
  // Constructor
  Sensor(void);

  // General functionality
  bool pollAddress(uint8_t iicAddress);

  bool init(uint8_t iicAddress);
  bool toggleInterrupt(void);

  bool startMeasurement(void);
  bool requestMeasurementData(void);
  void copyMeasurementData(uint8_t * buf, uint8_t len);

  bool writeControlData(uint8_t * buf, uint8_t len);

  // return sensor attributes
  uint8_t getIicAddress(void);
  uint8_t getSensorType(void);
  uint8_t getRxLen(void);
  uint8_t getTxLen(void);
  
private:
  bool requestSensorType(void);
  bool requestRxLen(void);
  bool requestTxLen(void);
  bool checkReady(void);

protected:
  uint8_t sensorType;
  uint8_t iicAddress;
  
  uint8_t mLen;
  uint8_t cLen;
  uint8_t * mData;

  // optional
  uint8_t _numErrors;


};

#endif /* __SENSOR_H__ */
