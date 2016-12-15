
/*
 * File:   Logger.cpp
 * Author: Apoorva Sharma (asharma@hmc.edu)
 *
 * Created on 10 June 2016
 */

#include "Params.h"
#include "Logger.h"

inline uint8_t queueNext(uint8_t ht) {return (ht + 1) & (QUEUE_DIM -1);} 

Logger::Logger(SdFat & sd_, SdFile & file_)
  : num_datasources(0), sd(sd_), file(file_)
{
}

void Logger::include(DataSource * source_p)
{
  sources[num_datasources] = source_p;
  ++num_datasources;
}

void Logger::padding(int number, byte width, String & str) {
  int currentMax = 10;
  for (byte i=1; i<width; i++){
   if (number < currentMax) {
     str.concat("0");
   }
   currentMax *= 10;
  }
  str.concat(number);
}

void Logger::init(void)
{
  // Determine logfilename by finding the first X such that basenameX is not
  // already on the SD card.
  unsigned int number = 0;
  String numstr = "";
  padding(number, 3, numstr);
  String finalname = LOG_FILENAME_BASE + numstr + ".bin";
  finalname.toCharArray(logfilename, LOG_FILENAME_BUFFERLEN);

  while(sd.exists(logfilename)) {
    number++;
    numstr = "";
    padding(number, 3, numstr);
    finalname = LOG_FILENAME_BASE + numstr + ".bin";
    finalname.toCharArray(logfilename, LOG_FILENAME_BUFFERLEN);
  }

  // determine corresponding headingfilename
  finalname = HEADINGS_FILENAME_BASE + numstr + ".txt";
  finalname.toCharArray(headingfilename, LOG_FILENAME_BUFFERLEN);
  
  Serial.print("Logger: Using log file name "); Serial.println(logfilename);

  // Write column headings and datatypes to file
  String headingStr = "time";
  String dataTypeStr = "ulong";
  for (size_t i = 0; i < num_datasources; ++i) {
    headingStr += ",";
    headingStr += sources[i]->csvVarNames;
    dataTypeStr += ",";
    dataTypeStr += sources[i]->csvDataTypes;
  }
  headingStr += "\n";
  headingStr += dataTypeStr;

  if (file.open(headingfilename, O_WRITE | O_CREAT | O_APPEND)) {
    file.println(headingStr);
    file.close();
  } else {
    Serial.print("Logger: Error opening "); Serial.println(headingfilename);
    while (1) {}; // stop the show
  }

  // Set up the binary log file
  Serial.println("Creating log file");
  if (!file.createContiguous(sd.vwd(), logfilename, 512 * FILE_BLOCK_COUNT)) {
    Serial.print("Logger: Error creating "); Serial.println(logfilename);
    while (1) {}; // stop the show
  }
  // get address of file on SD
  if (!file.contiguousRange(&bgnBlock, &endBlock)) {
    Serial.println("Logger: Error getting range.");
    while (1) {}; // stop the show
  }
  file.close();

  uint32_t ERASE_SIZE = 262144L;
  Serial.println("Erasing all data");
  // flash erase all data in file
  uint32_t bgnErase = bgnBlock;
  uint32_t endErase;
  while (bgnErase < endBlock) {
    endErase = bgnErase + ERASE_SIZE;
    if (endErase > endBlock) endErase = endBlock;
    if (!sd.card()->erase(bgnErase, endErase)) {
      Serial.println("Logger: Error with erase");
      while (1) {}; // stop the show
    }
    bgnErase += ERASE_SIZE;
  }

  // initialize queues
  emptyHead = emptyTail = 0;
  fullHead = fullTail = 0;

  currentBuffer = 0;

  // use SdFats internal buffer
  uint8_t* cache = (uint8_t*)sd.vol()->cacheClear();
  if (cache == 0){
    Serial.println("Logger: error in cacheClear");
    while (1) {}; // stop the show
  };
  emptyQueue[emptyHead] = cache;
  emptyHead = queueNext(emptyHead);
  
  // put rest of buffers in empty queue
  for ( uint8_t i = 0; i < BUFFER_BLOCK_COUNT; i++) {
    emptyQueue[emptyHead] = block + 512 * i;
    emptyHead = queueNext(emptyHead);
  }

  // start multiple block write
  if (!sd.card()->writeStart(bgnBlock, FILE_BLOCK_COUNT)) {
    Serial.println("Logger: Error writeBegin");
    while (1) {}; // stop the show
  }
}

void Logger::log(unsigned long time_val)
{
  unsigned long t1 = micros();

  size_t idx = 0;
  unsigned long * time_slot = (unsigned long *) rowbuffer;
  *time_slot = time_val;
  idx += sizeof(time_val);

  for (size_t i = 0; i < num_datasources; ++i) {
    idx = sources[i]->writeDataBytes(rowbuffer, idx);
  }

  // write data to rowbuffer

  // if we don't have a buffer
  if (currentBuffer == 0) {
    // grab an empty buffer if there is one
    if (emptyHead != emptyTail) {
      currentBuffer = emptyQueue[emptyTail];
      emptyTail = queueNext(emptyTail);

      currentIdx = 0; // reset the index
    } else {
      // no more empty buffers!! 
      Serial.println("Logger: ran out of empty buffer space! Skipping entry.");
      return;
    }
  }

  // copy rowbuffer to logbuffer
  for (size_t i = 0; i < idx; i++) {
    *(currentBuffer+currentIdx) = *(rowbuffer+i);
    currentIdx++;

    // check if we've filled the buffer
    if (currentIdx >= 512) {
      // place full buffer in the fullQueue
      fullQueue[fullHead] = currentBuffer;
      fullHead = queueNext(fullHead);
      Serial.println("Logger: filled one buffer");

      // grab an empty buffer if there is one
      if (emptyHead != emptyTail) {
        currentBuffer = emptyQueue[emptyTail];
        emptyTail = queueNext(emptyTail);

        currentIdx = 0; // reset the index
      } else {
        // no more empty buffers!! 
        Serial.println("Logger: ran out of empty buffer space! Skipping entry.");
        return;
      }
    }
  }

  t1 = micros() - t1;
  Serial.print("Logger.log function took (us): "); Serial.println(t1);
}

void Logger::write(void)
{
  uint32_t bn = 0;
  while (bn < FILE_BLOCK_COUNT) {
    // if the fullQueue isn't empty
    noInterrupts();
    if (fullHead != fullTail) {
      // write a block
      uint8_t* block = fullQueue[fullTail];
      interrupts();
      unsigned long t1 = micros();
      if (!sd.card()->writeData(block)) {
        Serial.println("Logger: error writing block to file.");
        while (1) {}; // stop the show
      }
      t1 = micros() - t1;
      noInterrupts();
      // remove it from the full queue, back to the empty one
      emptyQueue[emptyHead] = block;
      emptyHead = queueNext(emptyHead);
      fullTail = queueNext(fullTail);

      Serial.print("Logger: wrote block"); Serial.print(bn);
      Serial.print(" in (us): "); Serial.println(t1);
      Serial.print(" "); Serial.print(fullHead-fullTail); Serial.println("full blocks remaining");
      bn++;
    }
    interrupts();
  }

  if (!sd.card()->writeStop()) {
    Serial.println("Logger: error in writeStop");
    while(1) {}; // stop the show
  }
  Serial.println("Logger: Filled file!");
  while (1) {}; // stop the show
}