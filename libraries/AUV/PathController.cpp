
/*
 * File:   PathController.cpp
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 */

#include "PathController.h"

PathController::PathController(SdFile & file_)
  : file(file_)
{
}

void PathController::init(char* trajFileName, StateEstimator* stateEstimator_p, waypoint_t * desiredPosition_p)
{
  current_waypoint_idx = 0;
  if (file.open(trajFileName)) {
    // read lat lon points from file
    String numstr = "";
    int i = 0;
    char c;
    byte itemnum = 0;

    while (file.available() && i < 2*MAX_NUM_WAYPOINTS) {
      c = file.read(); 
      if (c == ',' || c == '\n') {
        // we have reached the end of a number, so
        // determine if this was an x or y coordinate
        itemnum = i % 3;
        if (itemnum == 0) {
          // x
          trajectory[i/3].x = numstr.toFloat();
        } else if (itemnum == 1){
          // y
          trajectory[i/3].y = numstr.toFloat();
        } else {
          // heading
          trajectory[i/3].heading = numstr.toFloat();
        }
        i++; // increment the number counter
        numstr = ""; // reset the numstr
      } else {
        numstr += c;
      }
    }

    file.close();

    if (i % 3 != 0) {
      Serial.println("Invalid trajectory file!");
    }

    num_waypoints = i/3;

    // Convert to XY and print the trajectory read over serial
    Serial.println("Trajectory: ");
    for (i = 0; i < num_waypoints; ++i) {
      float x; float y;
      stateEstimator_p->latlonToXY(trajectory[i].x, trajectory[i].y, &x, &y);
      trajectory[i].x = x;
      trajectory[i].y = y;

      Serial.print(i); Serial.print(": ");
      Serial.print(trajectory[i].x); Serial.print(", ");
      Serial.print(trajectory[i].y); Serial.print(",");
      Serial.print(trajectory[i].heading); Serial.print("\n");
    }

    *desiredPosition_p = trajectory[0];

  } else {
    Serial.print(trajFileName);
    Serial.println(": file not found");
  }
}

/* 
 * assigns desiredPosition to be the current waypoint on the trajectory.
 * updates the current waypoint if the robot is within a success radius of the 
 * current waypoint.
 */
void PathController::control(StateEstimator * stateEstimator_p, waypoint_t * desiredPosition_p)
{
  float dx = stateEstimator_p->state.x - desiredPosition_p->x;
  float dy = stateEstimator_p->state.y - desiredPosition_p->y;
  float distsquared = pow(dx,2) + pow(dy,2);
  if (distsquared < SUCCESS_RADIUS_SQUARED) {
    Serial.print("Reached waypoint "); Serial.println(current_waypoint_idx);
    if (current_waypoint_idx < num_waypoints - 1) {
      current_waypoint_idx++;
    } else {
      current_waypoint_idx = 0;
    }
  }

  *desiredPosition_p = trajectory[current_waypoint_idx];
}



