#include <Captain64Bus.h>
#include "CMOSsRAM.h"
#include "Arduino.h"

Captain64Bus::Captain64Bus(CMOSsRAM* ram) {
	this->ram = ram;
}

byte Captain64Bus::readData(void) {
	byte data = 0;
	for (int b = 0; b < 8; b++) {
		pinMode(ram->dataBus[b], INPUT);
		unsigned char value = digitalRead(ram->dataBus[b]) ? 1 : 0;
		data = (data << 1) + value;
	}
	return data;
}

unsigned int Captain64Bus::readAddress(void) {
	unsigned int address = 0;
	for (int b = 0; b < 16; b++) {
		pinMode(ram->adressBus[b], INPUT);
		unsigned char value = digitalRead(ram->adressBus[b]) ? 1 : 0;
		address = (address << 1) + value;
	}
	return address;
}

void Captain64Bus::writeData(byte data) {
	int pos = 1;
	for (int b = 0; b < 8; b++) {
		pinMode(ram->dataBus[7 - b], OUTPUT);
		digitalWrite(ram->dataBus[7 - b], (data & pos) ? 1 : 0);
		pos <<= 1;
	}
}

void Captain64Bus::writeAddress(unsigned int address) {
	unsigned int pos = 1;
	for (int b = 0; b < 16; b++) {
		pinMode(ram->adressBus[15 - b], OUTPUT);
		digitalWrite(ram->adressBus[15 - b], (address & pos) ? 1 : 0);
		pos <<= 1;
	}
}
