####
TODO
####


Code
====
- [x] Add Arduino gateway code, e.g. ``gateway-rfm69-beradio``
- [x] Add ``node-wifi-mqtt`` from Clemens
- [o] Add :ref:`libberadio` spike and associated documentation from :ref:`beradio`
- [o] Add HX711 calibration sketch

libberadio
----------
- [o] Send ``embencode.patch`` upstream
- [o] Integrate ``StandardCplusplus`` via git submodule
- [o] Try to use it from regular sketches. Watch the firmware sizes!


Documentation
=============
- [x] Add Sphinx for documentation
- [x] Purge docs/build from git repository
- [o] Publish at https://hiveeyes.org/docs/arduino/
- [o] Interlink Sphinx (conf.py) with swarm, beradio and maybe kotori docs
- [x] Add "Howto use inotool" to README
- [o] Check out different Sphinx theme


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

