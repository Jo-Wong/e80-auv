
/*
 * File:   VelocityController.h
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 * 
 * TODO:
 *  - tune the proportional control gains (k_r, k_a, k_b)
 */

#ifndef __VELOCITY_CONTROLLER_H__
#define __VELOCITY_CONTROLLER_H__

#include <Arduino.h>

#include "StateEstimator.h"
#include "PathController.h"

typedef struct VelocitySetpoint {
  VelocitySetpoint(void)
    : v(0), w(0)
  {}
  
  VelocitySetpoint(float v_val, float w_val)
    : v(v_val), w(w_val)
  {}

  float v;
  float w;
} velocity_setpoint_t;

/* 
 * VelocityController determines a linear and rotational velocity setpoint to 
 * minimize the error between the robot's current position and the desired
 * waypoint.
 */
class VelocityController
{
public:
  void control(StateEstimator * stateEstimator_p, waypoint_t * desiredPosition_p, velocity_setpoint_t * desiredVelocity_p);

private:
  const double k_r = 1;
  const double k_a = 2.5;
  const double k_b = -1.5;
};

#endif