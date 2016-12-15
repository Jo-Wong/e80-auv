
/*
 * File:   MotorController.h
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 *
 * TODO:
 *  - add a depth setpoint as an argument to control, and use it to set the z-motor's thrust value
 *    in the MotorDriver.
 */

#ifndef __MOTOR_CONTROLLER_H__
#define __MOTOR_CONTROLLER_H__

#include <Arduino.h>

#include "StateEstimator.h"
#include "VelocityController.h"
#include "MotorDriver.h"

/* 
 * MotorController computes the motor speeds necessary to achieve the velocity
 * setpoint provided.
 */
class MotorController
{
public:
  void control(StateEstimator * stateEstimator_p, velocity_setpoint_t * desiredVelocity_p, MotorDriver * driver_p);

private:
};

#endif