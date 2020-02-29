
# Captain 64 
A low-cost Commodore 64's compatible motherboard emulated in a microcontroller.

## Background
A few years ago, when I started diving deep into digital electronics, I really wanted to create my own 8-bit computer, so I started dreaming about what it would be like. 

I decided to make my dream computer compatible with the Commodore 64, because of some reasons: 1) I already had some experience with its processors machine code, and with the IEEE-488 (the serial port to connect floppy drives, printers and other accessories); 2) It has been listed in the Guinness World Records as the highest-selling single computer model of all time, that makes possible that today there are hundreds of tools that makes my job easier; 3) There is an active community where I can ask for help; 4) last but not least, it was my first computer so I carry it in my heart. 

[Building my dream computer](https://youtu.be/ayh0qebfD2g) by The 8 bit guy  and [Building an 8-bit breadboard computer!](https://youtu.be/LnzuMJLZRdU) by Ben Eater inspired me to start this project. By watching their videos I realised that some people might find entertaining to build their own 8-bit computers, but probably they  don't have the time, the knowledge or the money (or any combination of that three). 

Finally, after after analyzing some DIY Commodore 64 projects I found this one challenging enough to prevent me from rusting.  So I got down to work and defined the:

## Fundamental requirements:

As any of my projects:
 - All software must to be Free Software, under [GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.html) for all the other components. So that I get help for bug fixing and new features.
 - All hardware (schematics, documentation, etc) under Creative Commons [Attribution 3.0 Spain  CC-BY-3.0](https://creativecommons.org/licenses/by/3.0/). So that the project is announced.

For this specific one:
 - It must not use any old components that cannot be purchased anymore, missing pieces is a nightmare for any hobby. 
 - It must be modular, so that it can be acquired by parts, having incremental functionality from the beginning.
 - Keep it cheap must be a goal, so that makes it possible to build it by anyone who is really interested.
 - It must be build in [breadboards](https://en.wikipedia.org/wiki/Breadboard) (soldering iron and tin are not required), so that makes it easier to be build.
 
## Implementation:

Captain 64 uses a STM32 [Nucleo-144](https://github.com/ddonofrio/captain64/blob/master/Datasheets/nucleo-l496zg-p.pdf) board for emulating the [PLA](https://www.c64-wiki.com/wiki/PLA_(C64_chip)), the [CIA](https://www.c64-wiki.com/wiki/CIA), the [VIC II](https://www.c64-wiki.com/wiki/VIC#VIC-II). Also for translating the address for the [Kernal](https://www.c64-wiki.com/wiki/Kernal), [Charset](https://www.c64-wiki.com/wiki/Character_set) and [Basic](https://www.c64-wiki.com/wiki/BASIC_ROM) ROMs.

The three internal ROMs from the Commodore 64 are stored in a single chip: [AT28C256 ](https://www.mouser.es/datasheet/2/268/doc0006-1108095.pdf) Atmel EEPROM

The SRAM ([AS6C1008](https://www.mouser.es/datasheet/2/12/AS6C1008feb2007-1511508.pdf)) 128KBytes

