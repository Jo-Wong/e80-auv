
/*
 * File:   SensorIMU.cpp
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 10 June 2016
 */

#include "SensorIMU.h"
#include "Params.h"

SensorIMU::SensorIMU(Adafruit_9DOF * dof_p_, Adafruit_LSM303_Accel_Unified * accel_p_, 
    Adafruit_LSM303_Mag_Unified * mag_p_, Adafruit_L3GD20_Unified * gyro_p_)
  : DataSource("axIMU,ayIMU,azIMU,wxIMU,wyIMU,wzIMU,rollIMU,pitchIMU,headingIMU","float,float,float,float,float,float,float,float,float"), 
    dof_p(dof_p_), accel_p(accel_p_), mag_p(mag_p_), gyro_p(gyro_p_)
{  
}

void SensorIMU::init(void)
{
  bool a = true;
  a = a && accel_p->begin();
  a = a && mag_p->begin();
  a = a && gyro_p->begin();
  if (!a)
    Serial.println("Could not connect to IMU");
}

bool SensorIMU::read(void)
{
  sensors_event_t accel_event;
  sensors_event_t mag_event;
  sensors_event_t gyro_event;
  
  /* Read the accelerometer and magnetometer */
  accel_p->getEvent(&accel_event);
  mag_p->getEvent(&mag_event);
  gyro_p->getEvent(&gyro_event);
  
  state.acceleration = accel_event.acceleration; // TODO remove gravity from this acceleration
  state.omega = gyro_event.gyro;

  dof_p->accelGetOrientation(&accel_event, &state.orientation);
  return dof_p->magGetOrientation(SENSOR_AXIS_Z, &mag_event, &state.orientation);

  // return dof_p->fusionGetOrientation(&accel_event, &mag_event, &state.orientation);
}

void SensorIMU::printState(void)
{
  Serial.print("IMU: ax:"); Serial.print(state.acceleration.x);
  Serial.print(" ay:"); Serial.print(state.acceleration.y);
  Serial.print(" az:"); Serial.print(state.acceleration.z);
  Serial.print(" wx:"); Serial.print(state.omega.x);
  Serial.print(" wy:"); Serial.print(state.omega.y);
  Serial.print(" wz:"); Serial.print(state.omega.z);
  Serial.print(" roll:"); Serial.print(state.orientation.roll);
  Serial.print(" pitch:"); Serial.print(state.orientation.pitch);
  Serial.print(" heading:"); Serial.print(state.orientation.heading);
  Serial.println("");
}

void SensorIMU::getCSVString(String * csvStr_p)
{
  *csvStr_p += String(state.acceleration.x); *csvStr_p += ",";
  *csvStr_p += String(state.acceleration.y); *csvStr_p += ",";
  *csvStr_p += String(state.acceleration.z); *csvStr_p += ",";
  *csvStr_p += String(state.omega.x); *csvStr_p += ",";
  *csvStr_p += String(state.omega.y); *csvStr_p += ",";
  *csvStr_p += String(state.omega.z); *csvStr_p += ",";
  *csvStr_p += String(state.orientation.roll); *csvStr_p += ",";
  *csvStr_p += String(state.orientation.pitch); *csvStr_p += ",";
  *csvStr_p += String(state.orientation.heading);
}

size_t SensorIMU::writeDataBytes(unsigned char * buffer, size_t idx)
{
  float * data_slot = (float *) (buffer + idx);
  data_slot[0] = state.acceleration.x;
  data_slot[1] = state.acceleration.y;
  data_slot[2] = state.acceleration.z;
  data_slot[3] = state.omega.x;
  data_slot[4] = state.omega.y;
  data_slot[5] = state.omega.z;
  data_slot[6] = state.orientation.roll;
  data_slot[7] = state.orientation.pitch;
  data_slot[8] = state.orientation.heading;
  return idx + 9*sizeof(float);
}
