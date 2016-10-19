#ifndef __LOGGER_H__
#define __LOGGER_H__
#include <stdio.H>

#define QUEUE_DIM 16
#define LOG_FILENAME_BASE "log"
#define LOG_FILENAME_BUFFERLEN 20
#define HEADINGS_FILENAME_BASE "inf"
#define FILE_BLOCK_COUNT 1000
#define MAX_BYTES_PER_ROW 500
#define BUFFER_BLOCK_COUNT 12
#define MAX_NUM_DATASOURCES 20

#include <Arduino.h>
#include <SdFat.h>
#include "DataSource.h"

class MyLogger {
public:
	MyLogger(SdFat & sd_, SdFile & file_);
	void include(DataSource * source_p);
	void init(void);
	void log(void);
	void write(void);

private:
	DataSource* sources[MAX_NUM_DATASOURCES];
	unsigned int num_datasources;
	char logfilename[LOG_FILENAME_BUFFERLEN];
	char headingfilename[LOG_FILENAME_BUFFERLEN];
	
	SdFat & sd;
	SdFile & file;
	
	uint32_t bgnBlock, endBlock;

	uint8_t* emptyQueue[QUEUE_DIM];
	uint8_t emptyHead, emptyTail;
	uint8_t* fullQueue[QUEUE_DIM];
	uint8_t fullHead, fullTail;
	uint8_t block[512 * BUFFER_BLOCK_COUNT];

	uint8_t* currentBuffer;
	uint16_t currentIdx;
	unsigned char rowbuffer[MAX_BYTES_PER_ROW];
	uint32_t bn = 0;

	void padding(int number, byte width, String & str);
};
#endif