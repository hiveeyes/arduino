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

Misc
====

2016-11-07
----------
- Provide snapshot archive of whole repository


2016-11-01
----------
- [o] Add http://platformio.org/
- [o] Add https://nodemcu-build.com/
- [o] Add https://www.heise.de/developer/artikel/Auf-Kommando-3361570.html


Bugs
====
- [x] Fix git submodule dependency when cloning freshly,
  still does not track feature branches.
- [o] BERadio: Try to use BERadioShadowEncoder again
- [o] BERadio: message.set_mtu_size(96); (!!!)
- [o] generic.ino-gateway: put delay, to allow serial interrupt
- [o] BERadio: Add warning message if BERadioMessage overflows
  buffer of EmBencode (display only once!)

Features
========
- [x] Add main sketch from Clemens
- [o] Automatic firmware building for node-gprs-http
- [o] Add scale adjustment sketches for ADS1231 and HX711

BERadio
-------
- [x] Pluck into RadioHead, finally (char* to uint8_t* geschwummse)
- [x] Send data through RadioHead's RH_TCP driver
- [x] Debugging Infrastructure (dprint, memfree)
- [o] Integrate BERadio with Terkin transmitter infrastructure
- [o] Make :ref:`Kotori` receive data from RH_TCP driver
- [o] Make std::string to uint8_t conversion available at BERadioMessage::send

Libraries
---------
- [o] Move to https://github.com/rocketscream/Low-Power


Documentation
=============
- [o] Credits and more upstream references
- [o] CONTRIBUTORS.rst
- [o| node-gprs-http/README.rst
- [o] Display licenses in documentation
- [o] Properly document BOARD_TAG vs. BOARD_SUB for Arduino 1.0.x vs. 1.6.x

Packaging
=========
- [o] Project packaging (tarball, zip?) and publishing
- [o] Add ``git push github master`` to ``make release``. Don't miss the tags!
- [o] Also publish to aPlatformIO_
- [o] Detect when compling against Arduino 1.0.x vs. 1.6.x and adjust
  BOARD_TAG vs. BOARD_SUB appropriately.

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

