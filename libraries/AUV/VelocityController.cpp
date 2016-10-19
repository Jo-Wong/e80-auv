
/*
 * File:   VelocityController.cpp
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 */

#include "VelocityController.h"
#include <math.h>
#include <Params.h>

inline float modfloat(float x, float m) {
  float r = fmod(x,m);
  return (r<0) ? r + m : r;
}

void VelocityController::control(StateEstimator * stateEstimator_p, waypoint_t * desiredPosition_p, velocity_setpoint_t * desiredVelocity_p)
{
  // compute rho, alpha, beta:
  float dx = desiredPosition_p->x - stateEstimator_p->state.x;
  float dy = desiredPosition_p->y - stateEstimator_p->state.y;
  float rho = sqrt(pow(dx,2) + pow(dy,2));

  float alpha = -stateEstimator_p->state.heading + atan2(dy, dx);
  float beta = -stateEstimator_p->state.heading - alpha + desiredPosition_p->heading;
  alpha = modfloat(alpha + M_PI, 2*M_PI) - M_PI;
  beta = modfloat(beta + M_PI, 2*M_PI) - M_PI;

  // determine if we should use forward or backward control
  //if (alpha < M_PI_2 && alpha >= -M_PI_2) {
    // forward
    desiredVelocity_p->v = k_r*rho;
    desiredVelocity_p->w = k_a*alpha + k_b*beta;
  // } else {
  //   // backward
  //   alpha = -stateEstimator_p->state.heading + atan2(-dy, -dx);
  //   beta = -stateEstimator_p->state.heading - alpha + desiredPosition_p->heading;
  //   desiredVelocity_p->v = -k_r*rho;
  //   desiredVelocity_p->w = k_a*alpha + k_b*beta;
  // }

  // enforce max velocities
  if (desiredVelocity_p->v > MAX_LIN_VEL) {
    desiredVelocity_p->v = MAX_LIN_VEL;
  } else if (desiredVelocity_p->v < -MAX_LIN_VEL) {
    desiredVelocity_p->v = -MAX_LIN_VEL;
  }

  if (desiredVelocity_p->w > MAX_ROT_VEL) {
    desiredVelocity_p->w = MAX_ROT_VEL;
  } else if (desiredVelocity_p->w < -MAX_ROT_VEL) {
    desiredVelocity_p->w = -MAX_ROT_VEL;
  }
}