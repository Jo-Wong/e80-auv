
/*
 * File:   SensorPin.h
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 5 July 2016
 * 
 * TODO: test this class
 */

#ifndef __SENSOR_PIN_H__
#define __SENSOR_PIN_H__

#include <Arduino.h>
#include "DataSource.h"

class SensorPin : public DataSource {
public:
  SensorPin(int pin_, char * name_);

  // Starts the connection to the sensor
  void init(void);

  // Reads data from the sensor, returns whether new data was received
  bool read(void);

  // Latest reported data is stored here
  uint16_t value;
  void printState(void);

  // from DataSource
  void getCSVString(String * csvStr_p);
  size_t writeDataBytes(unsigned char * buffer, size_t idx);

private:
  int pin;
  char * name;
};

#endif