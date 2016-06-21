========================
Firmware size comparison
========================

avr-gcc compilations » binary result sizes

avr-hello
---------
source::

    #include <stdio.h>

    int main() {
         sprintf("%s", "hello world");
         return 0;
    }

avr-gcc -o avr-hello.elf avr-hello.cpp::

    # regular
    elf: 2590, hex: 791

    # stripped
    elf:  624, hex: 529


avr-gcc -mmcu=atmega328p -DF_CPU=8000000L -DARDUINO=105 -ffunction-sections -fdata-sections -g -Os -w -fno-exceptions -o avr-hello.elf avr-hello.cpp::

    # regular
    elf: 11596, hex: 586

    # stripped
    elf:   568, hex: 586


avr-sim
-------
source::

    #include <sim.h>

    int main() {
         _d("hello world\n");
         return 0;
    }

binary sizes::

    # regular
    elf: 10406, hex: 4747

    # stripped
    elf:   568, hex: 586


arduino-vector
--------------

Based on avr-stl-1.1.1:

- http://andybrown.me.uk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/
- https://drive.google.com/uc?export=download&id=0B9Zobp2aWUKzb2xvZ0Y2VGd1RTQ

source::

    #include <Arduino.h>
    #include <pnew.cpp>
    #include <iterator>
    #include <vector>

    int main() {
        std::vector<double> values;
        values.push_back(42.42);
        return 0;
    }

binary sizes::

    # regular
    hello.elf: 51200, hello.hex: 2358

    # stripped
    hello.elf:  1272, hello.hex: 2358

