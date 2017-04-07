/*
||
|| @author Alexander Brevig
|| @version 1.3
||
*/

#ifndef SERIALDEBUG_H
#define SERIALDEBUG_H

#include "Arduino.h"
#include "Print.h"
#include "PrintCascade.h"

#define  WARNING     	B00000001
#define  ERROR       	B00000010
#define  NOTIFICATION   B00000100
#define  STATUSUPDATE  	B00001000

class SerialDebug : public PrintCascade{
  public:
    SerialDebug();
    SerialDebug(unsigned int baudrate);
    SerialDebug(unsigned int baudrate,uint8_t state);
    SerialDebug(uint8_t uart,unsigned int baudrate,uint8_t state);
    void begin(unsigned int baudrate);
    void begin(uint8_t uart, unsigned int baudrate);
    void enable(uint8_t state);
    void disable(uint8_t state);
    void toggle(uint8_t state);
	bool debug(uint8_t type, char* source, char* message);
  private:
	uint8_t serialPort;
    bool enabled;
    uint8_t debugState;
};

extern SerialDebug SerialDebugger;

#endif

/*
|| Changelog:
||
|| 1.3 2009-04-15 - Alexander Brevig : No longer need for Arduino core modification
|| 1.2 2009-04-13 - Alexander Brevig : Made print calls cascadable
|| 1.1 2009-04-12 - Alexander Brevig : Added void append(char* message);
|| 1.0 2009-04-12 - Alexander Brevig : Initial Release
*/

