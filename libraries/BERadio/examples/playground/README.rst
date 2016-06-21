==============
AVR playground
==============

.. note::

    The Makefile includes machine-specific path definitions. Please adapt to your environment.


Hello world on simulavr
=======================

Build ``hello.cpp`` for AVR::

    make name=avr-hello

Build ``avr-sim.cpp`` and run in ``simulavr``::

    make sim name=avr-sim

It should say something like::

    ------------------------------------------
    Compiling
    ------------------------------------------
    /opt/local/bin/avr-gcc -mmcu=atmega328p -DF_CPU=8000000L -DARDUINO=105 -ffunction-sections -fdata-sections -g -Os -w -fno-exceptions 		-I/opt/local/avr/include -I/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/cores/arduino -I/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/variants/standard -I/Users/amo/dev/foss/open.nshare.de/beradio/src/cpp/avr-stl/include -I. -o avr-sim.elf avr-sim.cpp

    ------------------------------------------
    Simulating
    ------------------------------------------
    # http://www.nongnu.org/simulavr/intro.html
    /Users/amo/dev/foss/open.nshare.de/simulavr/src/simulavr --verbose --cpufrequency 8000000 --device atmega328 --terminate=exit --writetopipe=0x91,- --writetoabort=0x95 --writetoexit=0x96 --file avr-sim.elf
    MESSAGE File to load: avr-sim.elf
    MESSAGE Device name is atmega328, signature 0x1e9514.
    MESSAGE Add WriteToPipe-Register at 0x91 and write to file: -
    MESSAGE Add WriteToAbort-Register at 0x95
    MESSAGE Add WriteToExit-Register at 0x96
    MESSAGE Termination or Breakpoint Symbol: exit
    MESSAGE Running with CPU frequency: 8.000 MHz (8000000 Hz)
    hello world
    MESSAGE Simulation finished!
    SystemClock::Endless stopped
    number of cpu cycles simulated: 271



Vectors via avr-stl
===================
http://andybrown.me.uk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/
Download avr-stl version 1.1.1 from https://drive.google.com/uc?export=download&id=0B9Zobp2aWUKzb2xvZ0Y2VGd1RTQ
Unpack and properly adapt include path in Makefile.

Run demo::

    make sim name=arduino-vector

