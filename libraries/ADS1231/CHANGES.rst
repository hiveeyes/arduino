============================
Fix interrupt toggling error
============================
Error::

    ../libraries/ADS1231/ADS1231.cpp:30:27: error: 'SREG' was not declared in this scope
             uint8_t oldSREG = SREG;

Solution::

    // block interrupts
    -uint8_t oldSREG = SREG;
    -cli();
    +noInterrupts();

    // reenable interrupts
    -SREG = oldSREG;
    +interrupts();

See also: https://github.com/esp8266/Arduino/issues/2218


=========================
Fix type conversion error
=========================
Error::

    ../libraries/ADS1231/ADS1231.cpp:33:13: error: cannot convert 'volatile uint32_t* {aka volatile unsigned int*}' to 'volatile uint8_t* {aka volatile unsigned char*}' in assignment
             out = portOutputRegister(ADS1231s[this->ADS1231Index].sclPort);

Solution
========

ADS1231.h
---------
::

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


ADS1231.cpp
-----------
::

    -volatile uint8_t *out;
    +volatile PortReg *out;

See also:

- https://github.com/esp8266/Arduino/issues/1110
- https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library/pull/38



================
Fix linker error
================
Problem::

    Creating core archive
    Linking /tmp/mkESP/node-gprs-http_generic/node-gprs-http.bin
      Versions: 0.10.0-16-g62487f1-dirty, Unknown
    /Users/amo/dev/hiveeyes/sdk/esp8266-arduino-core-2.3.0/Arduino-2.3.0/tools/xtensa-lx106-elf/bin/../lib/gcc/xtensa-lx106-elf/4.8.2/../../../../xtensa-lx106-elf/bin/ld: /tmp/mkESP/node-gprs-http_generic/node-gprs-http.elf section `.text' will not fit in region `iram1_0_seg'
    collect2: error: ld returned 1 exit status
    ma

Solution: Use most recent ESP8266 Arduino Core from Git::

    git clone https://github.com/esp8266/Arduino

::

    Linking /tmp/mkESP/node-gprs-http_generic/node-gprs-http.bin
      Versions: 0.10.0-16-g62487f1-dirty, 2.3.0-88-g0291a6e

