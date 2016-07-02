.. include:: resources.rst

####
TODO
####


Main
====
- [x] Add Arduino gateway code, e.g. ``gateway-rfm69-beradio``
- [x] Add ``node-wifi-mqtt`` from Clemens
- [x] Add :ref:`libberadio` spike and associated documentation from :ref:`beradio`
- [o] Add HX711 calibration sketch
- [o] Add sketch from Clemens

BERadio
-------
- [x] Send ``embencode.patch`` upstream to enable OO-based composition
- [x] Integrate ``StandardCplusplus`` via git submodule
- [x] Try to use it from regular sketches. Watch the firmware sizes!
- [o] Integrate BERadio with Terkin transmitter infrastructure
- [o] Send data through RadioHead's RH_TCP driver to :ref:`Kotori`
- [o] Check char* to uint8_t conversion with RadioHead's ``manager69.sendtoWait(buf69, sizeof(buf69), RH69_TRANSCEIVER_ID);``


Documentation
=============
- [x] Add Sphinx for documentation
- [x] Purge docs/build from git repository
- [x] Interlink Sphinx (conf.py) with hiveeyes, beradio kotori docs
- [x] Add "Howto use inotool" to README
- [x] Check out different Sphinx theme
- [o] Publish at https://hiveeyes.org/docs/arduino/
- [o] Display license in documentation


Infrastructure
==============
For more tooling like distribution package building, see :ref:`beradio` and :ref:`kotori` repositories

- [x] Add Makefile for convenience
- [x] Add bumpversion for release cutting
- [o] Project packaging (tarball, zip?) and publishing
- [o] Add ``git push github master`` to ``make release``. Don't miss the tags!
- [o] Find other modern command line driven Arduino IDE replacement, maybe just use ArduinoMk
- [o] Use http://platformio.org/

    - http://platformio.org/lib/show/92/RFM69
    - http://platformio.org/lib/show/276/AESLib
    - http://docs.platformio.org/en/latest/librarymanager/config.html#dependencies
    - http://docs.platformio.org/en/latest/librarymanager/config.html#library-json
    - http://docs.platformio.org/en/latest/librarymanager/creating.html#library-creating-examples
    - http://docs.platformio.org/en/latest/userguide/lib/cmd_install.html#cmd-lib-install
    - http://docs.platformio.org/en/latest/platforms/atmelavr.html#packages


ESP8266
=======
- https://github.com/plerup/makeEspArduino
- https://github.com/esp8266/Arduino
- https://github.com/esp8266/Arduino/blob/master/boards.txt
- https://github.com/esp8266/Arduino/blob/master/platform.txt
-