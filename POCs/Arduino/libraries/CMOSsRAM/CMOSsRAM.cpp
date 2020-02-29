#include "CMOSsRAM.h"
#include "Arduino.h"

CMOSsRAM::CMOSsRAM(byte* adressBus, byte* dataBus, byte oePin, byte wePin, byte cePin, byte ce2Pin) {
	this->adressBus = adressBus;
	this->dataBus = dataBus;
	this->oePin = oePin;
	this->wePin = wePin;
	this->cePin = cePin;
	this->ce2Pin = ce2Pin;
	for (int b = 0; b < 16; b++) {
		pinMode(this->adressBus[b], INPUT);
		if (b < 8) { // hack for faster init of both busses at a time.
			pinMode(this->dataBus[b], INPUT);
		}
	}
	pinMode(this->oePin, OUTPUT);
	pinMode(this->wePin, OUTPUT);
	this->disable();
}

void CMOSsRAM::disable(void) {
    /*
	                R W
     Output Disable H H High-Z 
               Read L H DOUT
              Write X L DIN
    */
	digitalWrite(this->oePin, HIGH);
	digitalWrite(this->wePin, HIGH);
}

void CMOSsRAM::readData(void) {
	/* Read L H DOUT */
	digitalWrite(this->cePin, LOW);
	digitalWrite(this->ce2Pin, HIGH);
	digitalWrite(this->wePin, HIGH);
	digitalWrite(this->oePin, LOW);
	__asm__("nop\n\t");
	__asm__("nop\n\t");
	digitalWrite(this->cePin, HIGH);
	digitalWrite(this->ce2Pin, LOW);
	digitalWrite(this->oePin, HIGH);
}

void CMOSsRAM::writeData(void) {
	/* Write X L DIN */
	digitalWrite(this->cePin, LOW);
	digitalWrite(this->ce2Pin, HIGH);
	digitalWrite(this->wePin, LOW);
	digitalWrite(this->oePin, LOW);
	__asm__("nop\n\t");
	__asm__("nop\n\t");
	digitalWrite(this->cePin, HIGH);
	digitalWrite(this->ce2Pin, LOW);
	digitalWrite(this->wePin, HIGH);
}

