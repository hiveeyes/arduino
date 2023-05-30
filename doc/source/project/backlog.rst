.. include:: ../resources.rst

.. _todo:

#######
Backlog
#######

.. contents::
   :local:
   :depth: 1
   :class: this-will-duplicate-information-and-it-is-still-useful-here

----


****
2023
****

Iteration 1
===========
- [x] FW INDEX :hidden:
- [x] Refine new firmware overview page
- [x] Get rid of tools subdirectory
- [x] Hibernate Firmware Builder
- [x] Run linkchecker
- [x] Use brand color of previous theme
- [x] Naming things: ``trudering`` => ``stulabs`` or ``ringlabs``
- [x] Docs: Add Ringlabs Bienenwaage
- [x] Fix links within ``coverage.md``
  https://hiveeyes.org/docs/arduino/#coverage
- [x] Add Mermaid

Iteration 2
===========
- [x] Dissolve ``StandardCplusplus`` completely
- [o] Release aerowind-ads1231 0.1.0
- [o] Broadcast PlatformIO configurations to @wtf's and @mois' firmwares
- [o] multihop: Remove notion of ``HE_CONTAINERS``.
      After switching to ArduinoSTL, container data structures can be taken for granted.
- [o] Curate dependencies once more
- [o] Release

Layout
------
- [o] @mois-update

  - New group ``Moislabs``
  - https://github.com/bee-mois/beescale
  - Update README
  - first: https://www.euse.de/wp/blog/2011/11/bienen-ueberwachen/ (8. November 2011)
  - recent: https://www.euse.de/wp/blog/2023/05/wettermessung-direkt-am-stand/ (22. Mai 2023)

- [o] Rename ``Homie`` group to ``Nettetal`` or ``Nettlabs``

Documentation
-------------
- [o] Polish individual pages
- [o] At least, on all firmwares, you will need to configure the ``HX711_OFFSET`` and ``HX711_KNOWN_WEIGHT`` values ...
- [o] Rename ``./openhive/rfm69-node/synopsis.dot``
- [o] Add a flowchart to each program? Use Mermaid instead of Graphviz?
- [o] Improve leftnav, with sections
- [o] Improve section about Kotori
- [o] Add more footer links like Discourse
- [o] Bienenwaage 6.0?
- [o] Update ``project/licenses`` page to encourage users to use open data licenses as well
- [o] Update changelog
- [o] Nettetal: Links veraltet.

  - https://www.imker-nettetal.de/esp8266-beescale-erste-eindruecke/
  - https://www.nikolaus-lueneburg.de/2016/10/wemos-d1-mini-battery-shield/
  - https://community.hiveeyes.org/t/inbetriebnahme-von-node-wifi-mqtt-homie-mit-hiveeyes-anbindung/185
- [o] Link to "Bauanleitungen"




Iteration 3
===========
- [o] Migrate POSIX examples to PlatformIO
- [o] Add firmwares from Beelogger, BEEP, EasyHive, Autonome Zelle, Dachboden Lab, @hsors, Terkin, BOB
- [o] Firmware builder NG::

    pip install pio-launch
    pio-launch https://github.com/hiveeyes/arduino/tree/main/ringlabs/bienenwaage-5.0

- [o] Add firmwares by @wtf, @tonke, and @hsors.

  - https://git.cicer.de/autonome-zelle/autonome-zelle-sfwv
  - https://git.cicer.de/autonome-zelle/az3-cubecell-boden
  - https://github.com/hsors/connected-bee-hive
  - https://github.com/hiveeyes/terkin-datalogger
  - https://github.com/marcelgasser/nexTerkin
  - https://git.cicer.de/tonke/esphome-nau7802
  - https://git.cicer.de/tonke/preferencesscale
  - https://git.cicer.de/tonke/sx126x-arduino-lorawan
  - https://git.cicer.de/tonke/sim7000g
  - https://git.cicer.de/tonke/esphome
  - https://git.cicer.de/tonke/fipy
  - https://git.cicer.de/tonke/wipy
  - https://git.cicer.de/tonke/amqp-to-mqtt

- Look at muon, and more. https://github.com/hiveeyes/beradio/issues
- Generalize HTTP access

  - https://registry.platformio.org/libraries/csharpermantle2/http-parser
  - https://registry.platformio.org/libraries/csharpermantle/http-parser-arduino
  - https://registry.platformio.org/libraries/exosite/libexosite%20http-data-api%20sync
  - https://registry.platformio.org/libraries/azure/AzureIoTProtocol_HTTP
  - https://registry.platformio.org/libraries/khoih-prog/AsyncHTTPRequest_RP2040W
  - https://github.com/boblemaire/asyncHTTPrequest
  - https://registry.platformio.org/libraries/petruspierre/EasyHTTP
  - https://registry.platformio.org/libraries/khoih-prog/AsyncHTTPSRequest_Generic

- More libs

  - https://registry.platformio.org/libraries/ostaquet/SIM800L%20HTTP%20connector
  - https://registry.platformio.org/libraries/heltecautomation/ESP32_LoRaWAN
  - https://registry.platformio.org/libraries/ostaquet/SIM800L%20HTTP%20connector

Include reStructuredText - links do not work::

    ```{eval-rst}
    .. include:: ../resources.rst
    ```

Firmware-Builder - Helper.mk::

    firmware-info:

        @# For architecture "AVR"
        @echo "TARGET_HEX: $(TARGET_HEX)"
        @echo "TARGET_ELF: $(TARGET_ELF)"
        @echo "TARGET_BOARD_TAG: $(BOARD_TAG)"
        @echo "TARGET_BOARD_SUB: $(BOARD_SUB)"

        @# For architecture "ESP"
        @echo "TARGET_BIN: $(MAIN_EXE)"
        @echo "TARGET_CHIP: $(CHIP)"

    all-plus-firmware-info: all firmware-info




****
2019
****


2019-01-09
==========
- Look at https://github.com/SergiusTheBest/plog,
  a portable, simple and extensible C++ logging library



****
2017
****

Misc
====


2017-04-20
----------
- Make more #define's @ node-gprs-http-firmware-builder::

    const long loadCellZeroOffset = 38623;
    //  const long loadCellKgDivider  = 22053;
    // 1/2 value for single side measurement, so that 1 kg is displayed as 2 kg
    const long loadCellKgDivider  = 11026;

- With the "Modern, flexible firmware: WiFi, MQTT, JSON" firmware, decode and interpolate the
  topology information from the channel address into appropriate firmware variables:
  https://community.hiveeyes.org/t/firmware-builder-beta/265

- check out workbench@elbanco:/opt/esp-open-sdk after resizing /opt

  - https://github.com/pfalcon/esp-open-sdk
  - http://domoticx.com/sdk-esp8266-xtensa-architecture-toolchain/


2017-04-09
----------
- [o] Make SerialDebugger optionally use SERIAL_PORT_HARDWARE


2017-04-06
----------
- Further improvements to :ref:`node-wifi-mqtt`

  - [o] Publish discrete values to the MQTT bus, use MQTT_DISCRETE maybe.
  - [o] Add debugging using http://playground.arduino.cc/Code/SerialDebugger
  - [x] Comment: "Connect the D0 pin with RST pin to enable wakeup of the MCU." re. deep sleep mode
      - On "Adafruit Feather HUZZAH", wire RST with GPIO16,
        see also https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/pinouts
  - [x] @gtuveri: it makes no sense to add #if SENSOR* stuff if SENSOR_DUMMY is true?
  - [o] @gtuveri: why to proceed within execution if no WiFi connection has been established?
  - [x] @Thias: ``#define DEEPSLEEP_TIME MEASUREMENT_INTERVAL * 1000UL`` since measurement interval is in ms already
  - [x] Improve inline comments again
  - [o] Switch from Adafruit_MQTT to https://github.com/tuanpmt/esp_mqtt or https://github.com/marvinroger/async-mqtt-client?
  - [x] Add sensor ADS1231


2017-04-05
----------
- Polish https://github.com/hiveeyes/aerowind-ads1231
- Add http://www.nikolaus-lueneburg.de/2016/10/wemos-d1-mini-battery-shield/
- Announce at http://forum.sodaq.com/t/hive-scale-code/275 when this works on ESP8266 also


2017-04-03
----------
- https://github.com/PaulStoffregen/TimeAlarms
- https://github.com/pycom/pycom-micropython/blob/master/docs/lopy/quickref.rst
- https://github.com/pycom/pycom-micropython/blob/master/docs/library/network.LoRa.rst


2017-03-31
----------
- Use LWT (Last Will and Testament) MQTT feature
  http://www.hivemq.com/blog/mqtt-essentials-part-9-last-will-and-testament
- Update TerkinData with appropriate JSON serialization::

    int json_length = root.measureLength();
    char payload[json_length+1];
    root.printTo(payload, sizeof(payload));

  See also: https://github.com/bblanchon/ArduinoJson/issues/75

- Let all devices send a "system boot" annotation


2017-01-11
----------
- Document ADS1231 patch for ESP8266, write to forum
- Augment calibration sketches to send calibration values to backend?
- Use PlatformIO
- Rename repository from "arduino" to "firmware"
- node-wifi-mqtt: Submit data as JSON container
- # If you like this project, please add a star!
- Test framework based on gtest: https://github.com/google/googletest

2017-01-10
----------
- Should use recent ESP8266 Arduino Core, see also https://github.com/esp8266/Arduino/compare/2.3.0...master
- Properly document correct libraries for individual sketches (A. Nickel)
- https://github.com/mike-matera/ArduinoSTL/pull/4

2017-01-09
----------
- https://github.com/bogde/HX711/pull/59
- Alternative libraries for ADS1231:

  - https://github.com/rfjakob/barwin-arduino
  - | https://github.com/jensfranke/ads-1231-particle-library
    | see also: https://community.particle.io/t/library-for-ads-1231-bee-hive-weigh-scale/21719

- http://forum.sodaq.com/c/beekeeping
- Rename node-gprs-http to node-gsm-wifi-http
- node-openhive: Make HTTP interface compatible with Hiveeyes backend
- node-openhive: Make feature flags compatible with firmware builder


****
2016
****

Misc
====

2016-12-15
----------
RaspIO setup

- Hardware jumper to decouple SPI from RFM69

- Compile using::

    cat Makefile-Linux.mk

    # RasPIO Hiveeyes, 2016-12-15
    BOARD_TAG         = gert328
    MCU = atmega328p
    F_CPU = 12000000L
    ISP_PROG = gpio
    FORCE_MONITOR_PORT = true

- Upload using::

    sudo avrdude -P gpio -c gpio -p atmega328p -v -U flash:w:../../bin/src/gert328/src.hex

- Enable UART on /dev/ttyS0::

    root@raspberrypi:~# cat /boot/config.txt | grep uart
    enable_uart=1

- Use the **correct** serial device::

    screen /dev/ttyS0 115200

.. seealso:: http://raspberrypi.stackexchange.com/questions/47671/why-my-program-wont-communicate-through-ttyama0-on-raspbian-jessie

- Stop picocom::

    CTRL + A + X


2016-12-02
----------
- [o] Investigate alternative HX711 library

  - http://www.arduinolibraries.info/libraries/queuetue-digital-balance-library
  - http://www.arduinolibraries.info/libraries/queuetue-hx711-library
  - https://github.com/queuetue/Q2-Balance-Arduino-Library
  - https://github.com/queuetue/Q2-HX711-Arduino-Library

- [o] Migrate from StandardCplusplus to ArduinoSTL?

  - https://github.com/mike-matera/ArduinoSTL
  - http://platformio.org/lib/show/750/ArduinoSTL
  - See also https://github.com/maniacbug/StandardCplusplus/pull/20


2016-11-01
----------
- [o] Provide complete Zip Snapshot of Repository contents


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
- https://platformio.org/
- https://registry.platformio.org/libraries/lowpowerlab/RFM69
- https://registry.platformio.org/libraries/davylandman/AESLib
- https://docs.platformio.org/en/latest/librarymanager/dependencies.html#dependency-specifications
- https://docs.platformio.org/en/latest/manifests/library-json/index.html#library-json
- https://docs.platformio.org/en/latest/librarymanager/creating.html#examples
- https://docs.platformio.org/en/latest/core/userguide/lib/cmd_install.html#pio-lib-install
- https://docs.platformio.org/en/latest/platforms/atmelavr.html#packages
