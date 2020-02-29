#pragma once

#include "Arduino.h"

class CMOSsRAM {
	byte oePin;
	byte wePin;
	byte cePin;
	byte ce2Pin;
public:
	// Constructor
	CMOSsRAM(byte *adressBus, byte *dataBus, byte oePin, byte wePin, byte cePin, byte ce2Pin);

	byte* adressBus;
	byte* dataBus;
	void disable(void);
	void readData(void);
	void writeData(void);
};