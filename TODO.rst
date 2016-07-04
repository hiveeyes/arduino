.. include:: resources.rst

####
TODO
####

.. contents::
   :local:
   :depth: 1

----

****
2016
****

Bugs
====
- [o] Fix git submodule dependency when cloning freshly,
  still does not track feature branches.
- [o] Try to use BERadioShadowEncoder again
- [o] message.set_mtu_size(96); (!!!)

Features
========
- [o] Add sketch from Clemens
- [o] Add HX711 calibration sketch

BERadio
-------
- [x] Pluck into RadioHead, finally (char* to uint8_t* geschwummse)
- [x] Send data through RadioHead's RH_TCP driver
- [x] Debugging Infrastructure (dprint, memfree)
- [o] Integrate BERadio with Terkin transmitter infrastructure
- [o] Make :ref:`Kotori` receive data from RH_TCP driver
- [o] Make std::string to uint8_t conversion available at BERadioMessage::send

Documentation
=============
- [o] Credits and more upstream references
- [o] Display license in documentation

Packaging
=========
- [o] Project packaging (tarball, zip?) and publishing
- [o] Add ``git push github master`` to ``make release``. Don't miss the tags!
- [o] Also publish to aPlatformIO_

Platforms
=========
- [o] Get into aESP8266_


****
2015
****

Main
====
- [x] Add Arduino gateway code, e.g. ``gateway-rfm69-beradio``
- [x] Add ``node-wifi-mqtt`` from Clemens
- [x] Add :ref:`libberadio` spike and associated documentation from :ref:`beradio`

BERadio
-------
- [x] Send ``embencode.patch`` upstream to enable OO-based composition
- [x] Integrate ``StandardCplusplus`` via git submodule
- [x] Try to use it from regular sketches. Watch the firmware sizes!


Documentation
=============
- [x] Add Sphinx for documentation
- [x] Purge docs/build from git repository
- [x] Interlink Sphinx (conf.py) with hiveeyes, beradio kotori docs
- [x] Add "Howto use inotool" to README
- [x] Check out different Sphinx theme
- [x] Publish at https://hiveeyes.org/docs/arduino/


Infrastructure
==============
For more tooling like distribution package building, see :ref:`beradio` and :ref:`kotori` repositories

- [x] Add Makefile for convenience
- [x] Add bumpversion for release cutting
- [x] Find other modern command line driven Arduino IDE replacement, maybe just use ArduinoMk


********
Appendix
********

.. _aESP8266:

ESP8266
=======
- https://github.com/plerup/makeEspArduino
- https://github.com/esp8266/Arduino
- https://github.com/esp8266/Arduino/blob/master/boards.txt
- https://github.com/esp8266/Arduino/blob/master/platform.txt

.. _aPlatformIO:

PlatformIO
==========
- http://platformio.org/
- http://platformio.org/lib/show/92/RFM69
- http://platformio.org/lib/show/276/AESLib
- http://docs.platformio.org/en/latest/librarymanager/config.html#dependencies
- http://docs.platformio.org/en/latest/librarymanager/config.html#library-json
- http://docs.platformio.org/en/latest/librarymanager/creating.html#library-creating-examples
- http://docs.platformio.org/en/latest/userguide/lib/cmd_install.html#cmd-lib-install
- http://docs.platformio.org/en/latest/platforms/atmelavr.html#packages

