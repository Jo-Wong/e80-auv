
/*
 * File:   SensorGPS.cpp
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 10 June 2016
 */

#include "SensorGPS.h"
#include "Params.h"

SensorGPS::SensorGPS(HardwareSerial * Uart_p_)
  : DataSource("lat,lon,hdop,nsats","int32,int32,uint16,uint8"), Uart_p(Uart_p_)
{  
}

void SensorGPS::init(void)
{
  Uart_p->begin(9600);
}

bool SensorGPS::read(void)
{
  unsigned long start_time = millis();
 
  while (millis() - start_time < GPS_READ_INTERVAL) {
    if (Uart_p->available()) {
      char c = Uart_p->read();
      if (gps.encode(c)) {
        updateState();
        return 1;
      }
    }
  }
  return 0;
}

void SensorGPS::updateState(void)
{
  gps.get_position(&state.lat, &state.lon, &state.age);
  state.hdop = gps.hdop();
  state.num_sat = gps.satellites();
}

void SensorGPS::printState(void)
{
  Serial.print("GPS: Lat:"); Serial.print(state.lat);
  Serial.print(" Lon:"); Serial.print(state.lon);
  Serial.print(" HDOP:"); Serial.print(state.hdop);
  Serial.print(" N sats:"); Serial.print(state.num_sat);
  Serial.println("");
}

void SensorGPS::getCSVString(String * csvStr_p)
{
  *csvStr_p += String(state.lat); *csvStr_p += ",";
  *csvStr_p += String(state.lon); *csvStr_p += ",";
  *csvStr_p += String(state.hdop); *csvStr_p += ",";
  *csvStr_p += String(state.num_sat);
}

size_t SensorGPS::writeDataBytes(unsigned char * buffer, size_t idx)
{
  int32_t * int32_slot = (int32_t *) (buffer + idx);
  int32_slot[0] = state.lat;
  int32_slot[1] = state.lon;
  idx += 2*sizeof(int32_t);

  uint16_t * uint16_slot = (uint16_t *) (buffer + idx);
  uint16_slot[0] = state.hdop;
  idx += 1*sizeof(uint16_t);

  uint8_t * uint8_slot = (uint8_t *) (buffer + idx);
  uint8_slot[0] = state.num_sat;
  idx += sizeof(uint8_t);
  return idx;
}