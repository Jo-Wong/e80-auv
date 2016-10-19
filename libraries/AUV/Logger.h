
/*
 * File:   Logger.h
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 9 June 2016
 * 
 * TODO: 
 *  - perhaps restructure to remove the need for interrupts
 *  - change write() function to not have a blocking loop
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#define MAX_NUM_DATASOURCES 20
#define LOG_FILENAME_BASE "log"
#define HEADINGS_FILENAME_BASE "inf"
#define LOG_FILENAME_BUFFERLEN 20

#define MAX_BYTES_PER_ROW 500

// buffered logging
// number of 512B blocks in the log file
#define FILE_BLOCK_COUNT 1000
// number of 512B blocks in the buffer
#define BUFFER_BLOCK_COUNT 12
// size of queue, must be a power of two
#define QUEUE_DIM 16


#include <Arduino.h>
#include <SdFat.h>
#include "DataSource.h"

class Logger {
public:
  Logger(SdFat & sd_, SdFile & file_);

  // include all dataSources before running init
  void include(DataSource * source_p);

  // run after all dataSources have been registered
  void init(void);

  // records a row of data with time value as given
  void log(unsigned long time_val);

  // writes buffered data to file
  void write(void);

private:
  DataSource* sources[MAX_NUM_DATASOURCES];
  unsigned int num_datasources;
  char logfilename[LOG_FILENAME_BUFFERLEN];
  char headingfilename[LOG_FILENAME_BUFFERLEN];

  SdFat & sd;
  SdFile & file;

  unsigned char rowbuffer[MAX_BYTES_PER_ROW];

  void padding(int number, byte width, String & str);


  // buffered logging
  uint8_t* emptyQueue[QUEUE_DIM];
  uint8_t emptyHead;
  uint8_t emptyTail;

  uint8_t* fullQueue[QUEUE_DIM];
  uint8_t fullHead;
  uint8_t fullTail;

  uint8_t* currentBuffer;
  uint16_t currentIdx;

  uint32_t bgnBlock, endBlock; // start and end of contiguous data block
  uint8_t block[512 * BUFFER_BLOCK_COUNT];
};

#endif
