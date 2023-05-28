*****************
PlatformIO how-to
*****************


Setup
=====
::

    pip install --upgrade platformio


Atmel AVR
=========
E.g. for Arduino Pro Mini::

    platformio init --board pro8MHzatmega328
    platformio run --environment pro8MHzatmega328


Atmel SAM
=========
E.g. for Arduino Due::

    platformio init --board due
    platformio run --environment due


Espressif 8266
==============
E.g. for Adafruit Huzzah::

    platformio init --board huzzah
    platformio run --environment huzzah

