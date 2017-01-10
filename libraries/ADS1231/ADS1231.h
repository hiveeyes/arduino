    #ifndef ADS1231_H
    #define ADS1231_H

	#include <inttypes.h>
    #include <Arduino.h> //It is very important to remember this! note that if you are using Arduino 1.0 IDE, change "WProgram.h" to "Arduino.h"

    #ifdef __SAM3X8E__
      typedef volatile RwReg PortReg;
      typedef uint32_t PortMask;
    #elif defined(ESP8266)
      typedef volatile uint32_t PortReg;
      typedef uint32_t PortMask;
    #else
      typedef volatile uint8_t PortReg;
      typedef uint8_t PortMask;
    #endif

	#define MAX_ADS1231   6

	#define INVALID_ADS1231         255     // flag indicating an invalid servo index

	typedef struct  {
	  uint8_t sclBit          ;             // a pin number from 0 to 63
	  uint8_t sclPort         ;             // a pin number from 0 to 63
	  uint8_t dataBit         ;             // a pin number from 0 to 63
	  uint8_t dataPort        ;             // a pin number from 0 to 63
	  uint8_t pwdnBit         ;             // a pin number from 0 to 63
	  uint8_t pwdnPort        ;             // a pin number from 0 to 63
	} ADS1231_t   ;


    class ADS1231 {
    public:
            ADS1231();
            ~ADS1231();
			void attach( uint8_t sclPin , uint8_t dataPin , uint8_t pwdnPin );
            void power( uint8_t power_mode );
            uint8_t  check();
            long readData();
            long calibrate();
			void sclPulse();
	private:
			uint8_t ADS1231Index;             // index into the channel data for this ADS1231
    };

    #endif
