
/*
 * File:   SensorPin.cpp
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 10 June 2016
 */

#include "SensorPin.h"
#include "Params.h"

SensorPin::SensorPin(int pin_, char * name_)
  : DataSource(name,"uint16"), pin(pin_), name(name_)
{  
}

void SensorPin::init(void)
{
  pinMode(pin,INPUT);
}

bool SensorPin::read(void)
{
  value = analogRead(pin);
  return 1;
}

void SensorPin::printState(void)
{
  Serial.print(name); Serial.print(":"); Serial.println(value);
}

void SensorPin::getCSVString(String * csvStr_p)
{
  *csvStr_p += String(value);
}

size_t SensorPin::writeDataBytes(unsigned char * buffer, size_t idx)
{
  uint16_t * uint16_slot = (uint16_t *) (buffer + idx);
  uint16_slot[0] = value;
  idx += sizeof(uint16_t);
  return idx;
}