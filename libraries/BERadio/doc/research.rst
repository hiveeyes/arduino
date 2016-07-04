############################
BERadio C++ library research
############################

**********
Foundation
**********

avr-stl
=======
    | The Standard Template Library (STL) for AVR with C++ streams
    | Version 1.1.1 (most recent as of 2015-11-01)
    | http://andybrown.me.uk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/
    | https://drive.google.com/uc?export=download&id=0B9Zobp2aWUKzb2xvZ0Y2VGd1RTQ


***************************************
Convenient C++ programming with Arduino
***************************************

About
=====
The ATmega328_ has 32 KB ISP flash memory and 2 KB SRAM.

.. _ATmega328: https://en.wikipedia.org/wiki/ATmega328


Tech
====

Main readings
-------------
- http://www.stroustrup.com/C++11FAQ.html#init-list
- http://www.stroustrup.com/C++11FAQ.html#uniform-init
- http://www.stroustrup.com/C++11FAQ.html#variadic-templates

Variable-Length Argument Lists
------------------------------
- http://c-faq.com/varargs/
- http://c-faq.com/~scs/cgi-bin/faqcat.cgi?sec=varargs
- http://en.cppreference.com/w/cpp/utility/variadic
- https://www.eskimo.com/~scs/cclass/int/sx11b.html
- https://stackoverflow.com/questions/1657883/variable-number-of-arguments-in-c
- https://stackoverflow.com/questions/16337459/undefined-number-of-arguments
- http://www.varesano.net/blog/fabio/functions-variable-lenght-arguments-arduino
- http://www.cprogramming.com/tutorial/c/lesson17.html
- https://en.wikipedia.org/wiki/Variadic_template
- http://linux.die.net/man/3/va_arg

initializer_list
................
- http://coliru.stacked-crooked.com/a/84c7739d7d7e03cc

Modern C++
..........
http://www.murrayc.com/permalink/2015/12/05/modern-c-variadic-template-parameters-and-tuples/


Standard Template Library
-------------------------
Our pick
........
- | The Standard Template Library (STL) for AVR with C++ streams
  | Version 1.1.1 (most recent as of 2015-11-01)
  | http://andybrown.me.uk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/
  | https://drive.google.com/uc?export=download&id=0B9Zobp2aWUKzb2xvZ0Y2VGd1RTQ

Other choices
.............
- http://hackaday.com/2012/10/22/giving-the-arduino-deques-vectors-and-streams-with-the-standard-template-library/
- https://github.com/maniacbug/StandardCplusplus
- https://andybrown.me.uk/2011/01/15/the-standard-template-library-stl-for-avr-with-c-streams/
- https://arduino.stackexchange.com/questions/9835/problem-with-declaring-2d-vector-in-arduino
- https://msharov.github.io/ustl/
- http://cxx.uclibc.org/
- | EASTL -- Electronic Arts Standard Template Library
  | http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2271.html
- | uSTL
  | https://msharov.github.io/ustl/


Using the "Vector" container object
-----------------------------------
- https://github.com/maniacbug/StandardCplusplus/blob/master/vector
- https://github.com/maniacbug/StandardCplusplus/blob/master/vector.cpp
- | Topic: container for Objects like c++ vector?
  | https://stackoverflow.com/questions/9986591/vectors-in-arduino
- http://forum.arduino.cc/index.php/topic,45626.0.html
- | LinkedList for Arduino
  | https://gist.github.com/obedrios/2957439


Memory profiling
----------------
- http://andybrown.me.uk/2011/01/01/debugging-avr-dynamic-memory-allocation/

Object initialization
---------------------
- http://en.cppreference.com/w/cpp/language/direct_initialization
- http://en.cppreference.com/w/cpp/language/value_initialization


Float to String
---------------
- http://playground.arduino.cc/Main/FloatToString
- http://forum.arduino.cc/index.php/topic,37391.0.html

Misc
----
- http://electrons.psychogenic.com/modules/arms/art/3/AVRGCCProgrammingGuide.php
- https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
- https://gcc.gnu.org/onlinedocs/gcc-4.6.2/gcc/AVR-Options.html
- https://www.tty1.net/blog/2008/avr-gcc-optimisations_en.html


Simulation
==========
- http://www.nongnu.org/simulavr/
- https://github.com/Traumflug/simulavr
- http://mirror.unicorncloud.org/savannah-nongnu/simulavr/manual-1.0.pdf
- http://download.savannah.gnu.org/releases/simulavr/


Operating Systems
=================
- AVR RTOS - Atomthreads: Open Source RTOS
    - http://atomthreads.com/?q=node/1
    - http://atomthreads.com/index.php?q=node/20
    - https://raw.githubusercontent.com/kelvinlawson/atomthreads/master/ports/avr/README

MQTT interfaces
===============
- https://github.com/adafruit/Adafruit_MQTT_Library/blob/master/Adafruit_MQTT.cpp
- https://github.com/adafruit/Adafruit_MQTT_Library/blob/master/Adafruit_MQTT_Client.cpp
- http://knolleary.net/arduino-client-for-mqtt/
- http://knolleary.net/2009/01/29/updated-client-for-mqtt-library/
- https://github.com/knolleary/pubsubclient/blob/master/src/PubSubClient.cpp
- https://github.com/andreareginato/pubsubclient
- https://github.com/andreareginato/pubsubclient/commit/cdec8bf33742883057120e6a27fc3e3591991e6e

JSON serializers
================
- https://github.com/bblanchon/ArduinoJson
