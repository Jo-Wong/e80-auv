/*
 * File:  Logger.cpp
 * Author: Apoorva Sharma (asharma@hmc.edu)
 * Edited by: Josephine Wong (jowong@hmc.edu)
 *
 * Created on 10 June 2016
 *
 */
 
#include "Params.h"
#include "MyLogger.h"
#include <stdio.h>

inline uint8_t queueNext(uint8_t ht) {return (ht+1) & (QUEUE_DIM -1);}

MyLogger::MyLogger(SdFat & sd_, SdFile & file_)
	: num_datasources(0), sd(sd_), file(file_)
{
}

void MyLogger::include(DataSource * source_p) {
	sources[num_datasources] = source_p;
	++num_datasources;
}

void MyLogger::padding(int number, byte width, String & str) {
	int currentMax = 10;
	for (byte i = 1; i < width; i++) {
		if (number < currentMax) {
			str.concat("0");
		}
		currentMax *= 10;
	}
	str.concat(number);
}

void MyLogger::init(void) {
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

	finalname = HEADINGS_FILENAME_BASE + numstr + ".txt";
	finalname.toCharArray(headingfilename, LOG_FILENAME_BUFFERLEN);
	
	Serial.print("Logger: Using log file name ");
	Serial.println(logfilename);
	
	String headingStr = "letter";
	String dataTypeStr = "char";
	for(size_t i = 0; i < num_datasources; ++i) {
		headingStr += ",";
		headingStr += sources[i]->csvVarNames;
		dataTypeStr += ",";
		dataTypeStr += sources[i]->csvDataTypes;
	}
	headingStr += "\n";
	headingStr += dataTypeStr;
	
	if(file.open(headingfilename, O_WRITE | O_CREAT | O_APPEND)) {
		file.println(headingStr);
		file.close();
	} else {
		Serial.print("Logger: error opening");
		Serial.println(headingfilename);
		while(1) {};
	}
	
	Serial.println("Creating log file");
	if(!file.createContiguous(sd.vwd(), logfilename, 512 * FILE_BLOCK_COUNT)) {
		Serial.print("Logger: error creating");
		Serial.println(logfilename);
		while(1) {};
	}
	
	if(!file.contiguousRange(&bgnBlock, &endBlock)) {
		Serial.println("Logger: error getting range");
		while(1) {};
	}
	
	file.close();
	
	uint32_t ERASE_SIZE = 263144L;
	Serial.println("Erasing all data");
	uint32_t bgnErase = bgnBlock;
	uint32_t endErase;
	while(bgnErase < endBlock) {
		endErase = bgnErase + ERASE_SIZE;
		if(endErase > endBlock) endErase = endBlock;
		if(!sd.card()->erase(bgnErase, endErase)) {
			Serial.println("Logger: error with erase");
			while(1) {};
		}
		bgnErase += ERASE_SIZE;
	}
	
	emptyHead = emptyTail = 0;
	fullHead = fullTail = 0;
	
	currentBuffer = 0;
	
	uint8_t* cache = (uint8_t*) sd.vol()->cacheClear();	
	if(cache == 0) {
		Serial.println("Logger: error in cacheClear");
		while(1) {};
	};

	emptyQueue[emptyHead] = cache;
	emptyHead = queueNext(emptyHead);
	
	for(uint8_t i = 0; i < BUFFER_BLOCK_COUNT; i++) {
		emptyQueue[emptyHead] = block + 512 * i;
		emptyHead = queueNext(emptyHead);
	}
	
	if(!sd.card()->writeStart(bgnBlock, FILE_BLOCK_COUNT)) {
		Serial.println("Logger: error writeBegin");
		while(1) {};
	}
}	

void MyLogger::log(void){
	size_t idx = 0;
	byte * int_slot = (byte *) rowbuffer;
	*int_slot = 65;
	idx += sizeof(byte);
	
	for(size_t i = 0; i < num_datasources; ++i) {
		idx = sources[i]->writeDataBytes(rowbuffer, idx);
	}
	
	if(currentBuffer == 0) {
		if(emptyHead != emptyTail) {
			currentBuffer = emptyQueue[emptyTail];
			emptyTail = queueNext(emptyTail);
			currentIdx = 0;
		} else {
			Serial.println("Logger: ran out of empty buffers. Skipping entry");
			return;
		}
	}
		
	for(size_t i = 0; i < idx; i++) {
		*(currentBuffer + currentIdx) = *(rowbuffer + i);
		currentIdx++;
	}
	
	if(currentIdx >= 512) {
		fullQueue[fullHead] = currentBuffer;
		fullHead = queueNext(fullHead);
		Serial.println("Logger: filled one buffer");
		
		if(emptyHead != emptyTail) {
			currentBuffer = emptyQueue[emptyTail];
			emptyTail = queueNext(emptyTail);	
			currentIdx = 0;
		} else {
			Serial.println("Logger: ran out of empty buffers. Skipping entry");
			return;
		}
	}
}

void MyLogger::write(void) {
	
	while(bn < FILE_BLOCK_COUNT) {
		if(Serial.available()) break;
		else {
			noInterrupts();
			if(fullHead != fullTail) {
				uint8_t* input = fullQueue[fullTail];
				interrupts();
				if(!sd.card()->writeData(input)) {
					Serial.println("Logger: error writing block");
					while(1) {};
				}
				noInterrupts();
				emptyQueue[emptyHead] = input;
				emptyHead = queueNext(emptyHead);
				fullTail = queueNext(fullTail);
				bn++;
			}
			interrupts();
		}
		
		delay(100);
	}
	
	if(!sd.card()->writeStop()) {
		Serial.println("Logger: error in writeStop");
		while(1) {};
	}
	
	Serial.print("filled file or hit stop");
	while(1) {};
}