
/*
 * File:   StateEstimator.h
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 *
 * TODO: 
 *  - fix incorporateGPS function
 *  - restructure incorporation functions to be better suited
 *    for Kalman Filter implementation (have an predict function 
 *    and correction function)
 */

#ifndef __STATE_ESTIMATOR_H__
#define __STATE_ESTIMATOR_H__

#include <Arduino.h>

#include <SensorGPS.h>
#include <SensorIMU.h>
#include "MotorDriver.h"
#include "DataSource.h"


typedef struct {
  float x; // x position in global frame
  float y; // y position in global frame
  float heading; // heading in global frame
  float v; // linear velocity in robot frame
  float w; // rotational velocity in robot frame
} state_t;

/* 
 * StateEstimator class keeps track of the robot's state, incorporating 
 * measurements of the system outputs from the various sensors like IMU or 
 * GPS as well as the control inputs to the system.
 */
class StateEstimator : public DataSource
{
public:
  StateEstimator(void);

  // init
  void init(double loop_period, double orig_lat, double orig_lon);

  // Data incorporation (Kalman filter stuff would go here)
  void incorporateIMU(imu_state_t * imu_state_p);
  void incorporateGPS(gps_state_t * gps_state_p);
  void incorporateControl(MotorDriver * motorDriver_p);

  // State Access
  state_t state;
  void printState(void);

  void latlonToXY(double lat, double lon, float* x_p, float* y_p);

  // from DataSource
  void getCSVString(String * csvStr_p);
  size_t writeDataBytes(unsigned char * buffer, size_t idx);

private:
  double loop_period;
  double orig_lat;
  double orig_lon;
  float cosOrigLat;
};

#endif