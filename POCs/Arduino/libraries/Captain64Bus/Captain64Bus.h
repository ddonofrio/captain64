#pragma once

#include <CMOSsRAM.h>
#include <Arduino.h>

class Captain64Bus {
	CMOSsRAM* ram;
	
public:
	
	// Constructor
	Captain64Bus(CMOSsRAM* ram);
	byte readData(void);
	unsigned int readAddress(void);
	void writeData(byte data);
	void writeAddress(unsigned int address);
	byte readData(unsigned int address);
	void writeData(unsigned int address, byte);


};