/*
||
|| @author Alexander Brevig
|| @version 1.3
||
*/

#include "SerialDebug.h"
#include "HardwareSerial.h"

SerialDebug::SerialDebug(){
  enabled=false;
  debugState=0;
  serialPort = 0;
}

SerialDebug::SerialDebug(unsigned int baudrate){
  begin(baudrate);
  debugState=0;
  serialPort = 0;
}

SerialDebug::SerialDebug(unsigned int baudrate, uint8_t type){
  begin(baudrate);
  debugState=type;
  serialPort = 0;
}

SerialDebug::SerialDebug(uint8_t uart, unsigned int baudrate, uint8_t type){
  begin(uart, baudrate);
  debugState=type;
}

void SerialDebug::begin(unsigned int baudrate){
  enabled=true;
  Serial.begin(baudrate);
  printer = &Serial;
}

void SerialDebug::begin(uint8_t uart, unsigned int baudrate){ 
  serialPort = uart;
  enabled=true;
  switch (uart){
	#if defined(__AVR_ATmega1280__)
	case 2: Serial1.begin(baudrate); printer = &Serial1; break;
	case 3: Serial2.begin(baudrate); printer = &Serial2; break;
	case 4: Serial3.begin(baudrate); printer = &Serial3; break;
	#endif
	default: Serial.begin(baudrate); break;
  }
}

void SerialDebug::enable(uint8_t type){ 
  debugState = debugState | type; 
}

void SerialDebug::disable(uint8_t type){
  debugState = debugState & ~type; 
}

void SerialDebug::toggle(uint8_t type){
  if((debugState & type)==type){
    disable(type);
  }else{
    enable(type);
  }
}

bool SerialDebug::debug(uint8_t type, char* source, char* message){
  
  if (enabled && ((debugState & type)==type)){
    println().print(millis()).print(" - ");
	if ((debugState & type)==ERROR){ print("ERROR:"); }
	else if ((debugState & type)==WARNING){ print("WARNING:"); }
	else if ((debugState & type)==NOTIFICATION){ print("NOTIFICATION:"); }
	else if ((debugState & type)==STATUSUPDATE){ print("STATUS:"); }
	print("\n\t\t").print(source).print(" | ").print(message).print(" ");
	return true;
  }
  return false;
}

SerialDebug SerialDebugger;

/*
|| Changelog:
||
|| 2009-04-15 - Alexander Brevig : No longer need for Arduino core modification
|| 2009-04-15 - Alexander Brevig : Fixed a bug in debug and write 
|| 2009-04-12 - Alexander Brevig : Implemented cascadable print functions
|| 2009-04-12 - Alexander Brevig : Implemented void append(char* message);
|| 2009-04-12 - Alexander Brevig : Initial Release
*/

