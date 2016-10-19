
/*
 * File:   MotorDriver.cpp
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 8 June 2016
 */

#include "MotorDriver.h"
#include "Params.h"

MotorDriver::MotorDriver(int left1, int left2, int right1, int right2)
  : DataSource("left,right","int,int"), right(0), left(0), left1(left1), left2(left2), right1(right1), right2(right2)
{

}

void MotorDriver::apply(void)
{
  // determine direction and magnitude of spin required:
  l_abs = (left < 0) ? -2*left : 2*left;
  r_abs = (right < 0) ? -2*right : 2*right;

  // correct for deadzone if not zero
  if (l_abs)
    l_abs = l_abs - (MOTOR_L_DEADZONE*l_abs)/255 + MOTOR_L_DEADZONE;
  if (r_abs)
    r_abs = r_abs - (MOTOR_R_DEADZONE*r_abs)/255 + MOTOR_R_DEADZONE;

  l_dir = (left >= 0); // true if motor goes forward
  r_dir = (right >= 0);

  analogWrite(left1, l_dir*l_abs); // forward pin
  analogWrite(left2, (!l_dir)*l_abs); // backward pin
  analogWrite(right1, r_dir*r_abs); // forward pin
  analogWrite(right2, (!r_dir)*r_abs); // backward pin
}

void MotorDriver::printState(void) 
{
  Serial.print("L:"); Serial.print(left);
  Serial.print(" R:"); Serial.println(right);

  Serial.print("L: "); Serial.print(l_dir*l_abs); Serial.print(" "); Serial.println((!l_dir)*l_abs);
  Serial.print("R: "); Serial.print(r_dir*r_abs); Serial.print(" "); Serial.println((!r_dir)*r_abs);
}

void MotorDriver::getCSVString(String * csvStr_p)
{
  *csvStr_p += String(left); *csvStr_p += ",";
  *csvStr_p += String(right);
}

size_t MotorDriver::writeDataBytes(unsigned char * buffer, size_t idx)
{
  int * data_slot = (int *) (buffer + idx);
  data_slot[0] = left;
  data_slot[1] = right;
  return idx + 2*sizeof(int);
}