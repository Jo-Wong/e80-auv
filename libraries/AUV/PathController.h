
/*
 * File:   PathController.h
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 */

#ifndef __PATH_CONTROLLER_H__
#define __PATH_CONTROLLER_H__

#include <Arduino.h>

#include "StateEstimator.h"
#include "Params.h"
#include <SdFat.h>

typedef struct Waypoint {
  Waypoint(void)
    : x(0), y(0), heading(0)
  {}

  Waypoint(float x_val, float y_val, float heading_val)
    : x(x_val), y(y_val), heading(heading_val)
  {}

  float x;
  float y;
  float heading;
} waypoint_t;

/* 
 * PathController reads in a trajectory and then updates the next point 
 */
class PathController
{
public:
  PathController(SdFile & file_);

  void init(char* trajFileName, StateEstimator * stateEstimator_p, waypoint_t * desiredPosition_p);

  void control(StateEstimator * stateEstimator_p, waypoint_t * desiredPosition_p);

private:
  byte current_waypoint_idx;
  byte num_waypoints;
  waypoint_t trajectory[MAX_NUM_WAYPOINTS];

  SdFile & file;
};

#endif