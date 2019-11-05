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
  void setIntPin(uint8_t pinNr);

  bool setTresholds(uint8_t metric, uint8_t enabled, uint16_t tLevelLow, uint16_t tLevelHigh);
  bool startMeasurement(void);
  void setPollInterval(uint8_t metric, uint16_t poll);
  void loop(void);

// start block not tested ===========================
  
  bool readMeasurementData(uint8_t * buf, uint8_t * len);

  //end block not tested ===========================

  // return sensor attributes
  uint8_t getIicAddress(void);
  uint8_t getSensorType(void);
  uint8_t getNrMetrics(void);
  uint8_t getIntPin(void);
  
private:
  bool requestSensorType(void);
  bool requestNrMetrics(void);

protected:
  uint8_t sensorType;
  uint8_t iicAddress;
  uint8_t intPin;
  
  uint8_t nrMetrics;  // nr of different metrics supported by the sensor, each metric measurement takes up 2 bytes
  uint8_t mLen;       // (max nr of measurement bytes=) nrMetrics * 2 

  uint16_t * pollInterval;
  uint16_t * pollTimer;

  // optional
  uint8_t _numErrors; // not used

};

#endif /* __SENSOR_H__ */
