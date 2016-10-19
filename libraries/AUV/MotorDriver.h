
/*
 * File:   MotorDriver.h
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 *
 * TODO:
 *  - include the third motor
 */

#ifndef __MOTOR_DRIVER_H__
#define __MOTOR_DRIVER_H__

#include <Arduino.h>
#include "DataSource.h"

/* 
 * MotorDriver handles the raw signals that are needed to drive the robot's
 * motors.
 */
class MotorDriver : public DataSource
{
public:
  MotorDriver(int left1, int left2, int right1, int right2);

  // applies the stored values to the pins
  void apply(void);

  // prints current output to Serial port
  void printState(void);

  // motor values (range from -127 to +127) for full reverse or full forward
  int right;
  int left;

  // from DataSource
  void getCSVString(String * csvStr_p);
  size_t writeDataBytes(unsigned char * buffer, size_t idx);

private:
  // pins for the motors
  int left1;
  int left2;
  int right1;
  int right2;

  // absolute value of pwm outputs
  unsigned int l_abs;
  unsigned int r_abs;

  // direction of pwm outputs
  bool l_dir;
  bool r_dir;
};

#endif