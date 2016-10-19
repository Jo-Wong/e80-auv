
/*
 * File:   MotorController.cpp
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 */

 #include "MotorController.h"
 #include "Params.h"

 void MotorController::control(StateEstimator * stateEstimator_p, velocity_setpoint_t * desiredVelocity_p, MotorDriver * driver_p)
 {
  float v = desiredVelocity_p->v;
  float w = desiredVelocity_p->w;

  int rval = INV_MODEL_A*v + INV_MODEL_B*w;
  int lval = INV_MODEL_C*v + INV_MODEL_D*w;

  // bounds checking
  driver_p->right = (rval > 127) ? 127 : ( (rval < -127) ? -127 : rval ); 
  driver_p->left = (lval > 127) ? 127 : ( (lval < -127) ? -127 : lval ); 
 }