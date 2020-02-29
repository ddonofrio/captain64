/*
  MCU/Board: ATmega2560/Arduino MEGA 2560 R3
  Ports:
    22-29 = PORTA
    37-30 = PORTC  (inverted order)
    48-42 = PORTL  (inverted order)
    54-61 = PORTF
    62-69 = PORTK

    General BUS
    11   0 - A16 Select High/Low 64K mem bank
    37   1 - A15 Address bus
    36   2 - A14 Address bus
    35   3 - A13 Address bus
    34   4 - A12 Address bus
    33   5 - A11 Address bus
    32   6 - A10 Address bus
    31   7 - A9  Address bus
    30   8 - A8  Address bus
    29   9 - A7  Address bus
    28  10 - A6  Address bus
    27  11 - A5  Address bus
    26  12 - A4  Address bus
    25  13 - A3  Address bus
    24  14 - A2  Address bus
    23  15 - A1  Address bus
    22  16 - A0  Address bus
    45  17 - D7  Data bus   
    44  18 - D6  Data bus   
    43  19 - D5  Data bus   
    42  20 - D4  Data bus   
    41  21 - D3  Data bus   
    40  22 - D2  Data bus   
    39  23 - D1  Data bus   
    38  24 - D0  Data bus   
    10  25 - R/W Processor operation (High=Read/Low=Write)
     9  26 - IRQ Processor IRQ input (High=Normal operation)
     8  27 - RDY Processor Ready input signal (High=Normal operation)
    13  28 - CLK Processor clock input signal
     7  29 - MMI Processor MMI input signal (High=Normal operation)
    nc  30 - OUT Processor clock out (not used)
    12  31 - RST Processor reset signal (pullup)
     5  32 - OE# Ram Read Enabled Signal
     6  33 - WE# Ram Write Enabled Signal
     4  34 - CE# Ram Chip Enabled
     3  35 - CE2 Ram Chip Enabled 2

    Ram logic table:
                    R W
     Output Disable H H High-Z
               Read L H DOUT
              Write X L DIN

*/
#include <CMOSsRAM.h>
#include <Captain64Bus.h>
#include <string.h>

#define setLow(pin) pinMode(pin, OUTPUT); digitalWrite(pin, LOW);
#define setHigh(pin) pinMode(pin, OUTPUT); digitalWrite(pin, HIGH);
#define setInput(pin) pinMode(pin, INPUT);
#define setInputPullUp(pin) pinMode(pin, INPUT_PULLUP);

byte adressBus[] = {37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22};
byte dataBus[] = {45, 44, 43, 42, 41, 40, 39, 38};

CMOSsRAM *ram;
Captain64Bus *mother;

#define MP_RW_PIN 10
#define CLOCK_PIN 13
#define MP_MPI_PIN 7
#define MP_RDY_PIN 8
#define MP_IRQ_PIN 9
#define RESET_PIN 12
#define MEM_OE_PIN 5
#define MEM_WE_PIN 6
#define MEM_CE_PIN 4
#define MEM_C2_PIN 3
#define MEM_H_PIN 11

bool loadProgram = false;
//bool loadProgram = true;

unsigned long currentTic = 0;

void clock_init() {
 setHigh(CLOCK_PIN); 
}

void setup() {
  setInput(MP_RW_PIN);
  clock_init();
  setLow(MP_MPI_PIN); // Disabling MP
  setLow(MP_RDY_PIN); // Disabling MP
  setHigh(MP_IRQ_PIN);
  setInput(RESET_PIN);
  setLow(MEM_H_PIN); // Disabling 2nd bank of ram
  setHigh(MEM_C2_PIN);
  setLow(MEM_CE_PIN);
  ram = new CMOSsRAM(adressBus, dataBus, MEM_OE_PIN, MEM_WE_PIN, MEM_CE_PIN, MEM_C2_PIN);
  mother = new Captain64Bus(ram);
  Serial.begin(115200);
  ram->disable();

  Serial.print("Loading program in RAM...");
  
  if (loadProgram) {
    for (long i=0;i<0xFFFF;i++) {
      mother->writeAddress(i);
      mother->writeData(0xEA); //NOP
      ram->writeData();
    }
  }  

 /*********************************************/

  
  mother->writeAddress(0xfffc);
  mother->writeData(0x00);
  ram->writeData();
  mother->writeAddress(0xfffd);
  mother->writeData(0x1E);
  ram->writeData();

  Serial.println("Done.");

  /*********************************************/

  Serial.println("Done");
  Serial.print("Reseting CPU...");
  setLow(RESET_PIN);
  delay(2500);
  setInput(RESET_PIN);
  Serial.println("Done");
  
}

void addressHandler(unsigned long address) {
  
  switch(address){
    case 0x2000:
      ram->disable();
      mother->writeData(0x4C);
      break;
    case 0x2001:
      ram->disable();
      mother->writeData(0x00);
      break;
    case 0x2002:
      ram->disable();
      mother->writeData(0x20);
      break;
    case 0x3000:
      currentTic=0;
      break;
  }
  
}


bool isResetPressed=false;

void reset(boolean isPressed) {
  if (isPressed) {
    detachInterrupt(digitalPinToInterrupt(RESET_PIN));
    setLow(RESET_PIN); // LOW = Reset pressed
    isResetPressed = true;
  } else {
    setInputPullUp(RESET_PIN);
    attachInterrupt(digitalPinToInterrupt(RESET_PIN), hwResetPressed, LOW);
    isResetPressed = false;
  }
}

void hwResetPressed() {
  Serial.print('[Reset] Hardware button reset pressed\n');
}


void printBits(long int n) {
  byte numBits = 16; 
  for (byte i = 0; i < numBits; i++) {
      // shift 1 and mask to identify each bit value
    char b = (n & (1 << (numBits - 1 - i))) > 0 ? '1' : '0';
    Serial.print(b);
    if (i < (numBits - 1) && ((numBits-i - 1) % 4 == 0 )) 
      Serial.print(':'); // print a separator at every 4 bits
  }
}

boolean clockIsEnabled=true;
String *command = new String("");

long clocklInterval = 1;

boolean isClockHigh = false;
boolean isCPUreading = true;
boolean prevCPUop = false;
unsigned long previousMicros = 0;
unsigned int currentAdress = 0;
unsigned int previousAdress = 0;
unsigned int currentCount = 0;

unsigned int maxTix = 0;
unsigned int warmUp = 500;

void loop() {  
  if (micros() - previousMicros >= clocklInterval) { // Clock tic
    if (currentTic > warmUp) {
      isCPUreading = digitalRead(MP_RW_PIN);
      currentAdress = mother->readAddress();
      if (previousAdress != currentAdress
        || prevCPUop != isCPUreading) {
        previousAdress = currentAdress;
        prevCPUop = isCPUreading;
  
        if (isCPUreading){
          ram->readData();
        } else {
          ram->writeData();
        }

        addressHandler(currentAdress);
        
        printBits(currentAdress);
        char debugLine[80];
        byte byteData=mother->readData();
        
        sprintf(debugLine, "\t%04x\t%c\t%02x\t%s\n", currentAdress, 
          isCPUreading?'R':'W', byteData,(byteData==0xEA)?"NOP":"???");
        Serial.print(debugLine);
      }
    } else {
      if (currentTic == warmUp) {
        reset(false);
      } else
        reset(true);
    }
    if (clockIsEnabled) {
      digitalWrite(CLOCK_PIN, isClockHigh?LOW:HIGH);
      isClockHigh=!isClockHigh;
    }
    previousMicros = micros();
  }
  
  if (maxTix && (currentTic > maxTix)) {
    Serial.println("[Program halted]");
    while (1) {
      
    }
  }

    
  currentTic++;
}
