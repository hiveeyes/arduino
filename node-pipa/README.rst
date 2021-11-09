########################
Hiveeyes Pipa Datalogger
########################


*****
About
*****

A study to bring some paradigms of the *Terkin Datalogger for MicroPython* to C++.

- https://github.com/hiveeyes/terkin-datalogger
- https://community.hiveeyes.org/t/nexterkin-terkin-c-portierung/3577


*********
Platforms
*********

The program currently compiles for

- AVR ATmega328
- Espressif ESP8266
- Espressif ESP32
- Espressif ESP32S2
- ARM SAMD Cortex-M0


*****
Usage
*****

Build all examples for all available environments::

    git clone --recursive https://github.com/hiveeyes/arduino
    cd arduino/node-pipa
    make

Build specific examples for all available environments::

    source .venv/bin/activate
    cd examples/pipa-envirodiy
    pio run

Build specific examples for specific environment::

    # Build for AVR ATmega328
    pio run --environment=avr328 --verbose

    # List available environments
    pio run --list-targets


*******
Details
*******

- The configuration domain has been sketched out by @marcelgasser.

  - https://github.com/marcelgasser/nexTerkin/tree/spike_configmgr

- The telemetry domain has been sketched out by @amotl.

  - https://github.com/hiveeyes/arduino/tree/master/node-pipa/examples/legacy-gprsbee
  - https://hiveeyes.org/docs/arduino/TerkinData/README.html

- The sensor domain might be implemented on top of the EnviroDIY
  ModularSensors library. It is a library to give environmental
  sensors a common interface of functions for use with Arduino-
  compatible dataloggers.

  - https://community.hiveeyes.org/t/envirodiy-mayfly-data-logger/3035
  - https://github.com/EnviroDIY/ModularSensors
  - https://github.com/hiveeyes/arduino/tree/master/node-pipa/examples/pipa-envirodiy


*********
Etymology
*********

https://en.wikipedia.org/wiki/Pipa
