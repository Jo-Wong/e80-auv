
/*
 * File:   Params.h
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 * 
 * TODO: 
 *  - determine a good way of estimating control model parameters
 */

#ifndef __PARAMS_H__
#define __PARAMS_H__

/* GPS */
#define GPS_UNITS_PER_DEG 1000000.0

/* CONTROL MODEL */
// [a b; c d]*[rpwm; lpwm] = [v; w]
#define FWD_MODEL_A 0.0068
#define FWD_MODEL_B 0.0063
#define FWD_MODEL_C 0.0019
#define FWD_MODEL_D -0.0042
// [a b; c d]*[v; w] = [rpwm; lpwm]
// this matrix should be inv(FWD_MODEL matrix)
#define INV_MODEL_A 103.5546
#define INV_MODEL_B 156.1537
#define INV_MODEL_C 46.8461
#define INV_MODEL_D -167.4543

// maximum linear velocity in m/s
#define MAX_LIN_VEL 1.0

// maximum rotational velocity in rad/s
#define MAX_ROT_VEL 0.2


/* PATH CONTROLLER */
#define MAX_NUM_WAYPOINTS 20
#define SUCCESS_RADIUS_SQUARED 0.0625 // success radius in meters, squared


/* TIMING */
// how long the GPS data transfer function should take, in milliseconds
#define GPS_READ_INTERVAL 3

/* PINS */
#define SD_CHIP_SELECT 10

#define MOTOR_L_FORWARD 3
#define MOTOR_L_REVERSE 4
#define MOTOR_R_FORWARD 22
#define MOTOR_R_REVERSE 21

/* MOTOR DYNAMICS */
// the minimum PWM amount that causes the motors to actually spin
#define MOTOR_L_DEADZONE 34
#define MOTOR_R_DEADZONE 34 

#endif