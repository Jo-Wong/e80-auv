
/*
 * File:   SensorIMU.h
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 10 June 2016
 */

#ifndef __SENSOR_IMU_H__
#define __SENSOR_IMU_H__

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>
#include <Adafruit_9DOF.h>
#include "DataSource.h"

typedef struct {
  sensors_vec_t orientation;
  sensors_vec_t acceleration;
  sensors_vec_t omega;
} imu_state_t;

class SensorIMU : public DataSource {
public:
  SensorIMU(Adafruit_9DOF * dof_p_, Adafruit_LSM303_Accel_Unified * accel_p_, 
    Adafruit_LSM303_Mag_Unified * mag_p_, Adafruit_L3GD20_Unified * gyro_p_);

  // Starts the connection to the sensor
  void init(void);

  // Reads data from the sensor, returns whether new data was received
  bool read(void);

  // Latest reported data is stored here
  imu_state_t state;

  // prints state to serial
  void printState(void);

  // from DataSource
  void getCSVString(String * csvStr_p);
  size_t writeDataBytes(unsigned char * buffer, size_t idx);

private:
  Adafruit_9DOF * dof_p;
  Adafruit_LSM303_Accel_Unified * accel_p;
  Adafruit_LSM303_Mag_Unified * mag_p;
  Adafruit_L3GD20_Unified * gyro_p;
};

#endif