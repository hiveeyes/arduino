.. include:: resources.rst

.. _changes:

#######
CHANGES
#######


in-progress
===========
- :ref:`scale-adjust-firmware`: Make `scale-adjust-hx711.ino`_ compile against Espressif/ESP8266
- :ref:`generic-firmware`: Reduce loop interval when being in bootstrap phase
- :ref:`node-wifi-mqtt`: Fix Makefile for ESP8266 reported not to work on Ubuntu 16.10. Thanks, Matthias!
- :ref:`node-gsm-http`: Fix Makefile for ESP8266. Thanks, Matthias!
- :ref:`node-yun-http`: Add firmware for Mois Box. Thanks, Markus!


2017-03-31 0.14.0
=================
- Add slot, build infrastructure and dependency libraries for :ref:`mois-node`
- Update GPRSbee library to 1.9.2
- Update OneWire library to 2.3.3
- Update HX711 library incorporating our fix »Define “yield” as noop for older Arduino Core releases«
- Update Arduino-Makefile and makeEspArduino tools
- Improve firmwares :ref:`node-wifi-mqtt-homie` and :ref:`node-wifi-mqtt-homie-battery`
- :ref:`node-wifi-mqtt`:

    - Fix JSON serialization: Transmit sensor readings as float values and improve robustness. Thanks, Matthias and Giuseppe!
    - Improve configuration and documentation, last but not least regarding MQTT authentication.


2017-03-17 0.13.0
=================
- Add x-www-form-urlencoded serialization to :ref:`lib-terkin-data` library
- Fix JSON serialization: Don’t convert values to string
- Improve CSV serialization demo: Set float precision to three digits


2017-02-01 unreleased
=====================
- Add JSON serialization to :ref:`lib-terkin-data` library


2017-01-14 0.12.0
=================
- Add ArduinoSTL, a STL and iostream implementation for Arduino based on uClibc++
- Update firmware for :ref:`node-wifi-mqtt` (`node-wifi-mqtt.ino`_). Thanks Clemens and Karsten!

    - Make things actually work
    - Add dependency "Adafruit_MQTT_Library"
    - Add more sensors
    - Add Makefile and documentation

- Improve Homie Firmware for :ref:`node-wifi-mqtt-homie`

    - Add dependencies as git submodules
    - Add Makefile

- Add :ref:`lib-terkin-data` library: Flexible data collection for decoupling sensor reading and telemetry domains


2017-01-10 0.11.0
=================
- Add firmware for :ref:`node-wifi-mqtt-homie` (`node-wifi-mqtt-homie.ino`_).
  Contributed as `PR #1`_ by Alexander Wilms of `Imkerverein Nettetal e.V.`_. Thanks a bunch!
- Update firmware for :ref:`open-hive-firmware-gprs`: (`node-gprs-http.ino`_). Thanks, Clemens!

    - Add support for HX711
    - Add support for ESP8266

- Add "makeEspArduino", a makefile for ESP8266 Arduino projects by Peter Lerup
- Pull update to HX711 submodule
- Fix compilation errors for ADS1231 library on ESP8266
- Improve documentation

.. _PR #1: https://github.com/hiveeyes/arduino/pull/1


2016-11-01 0.10.0
=================
- Add sketch and update documentation about CSV payload transmission and forwarding to UART using a JeeLink device
  to complete the Open Hive RFM69 trio: :ref:`open-hive-firmware-rfm69-gateway-uart` (`gateway-rfm69-csv-uart.ino`_).
  Thanks Richard!
- Update sketch (beta release) and add documentation about :ref:`node-wifi-mqtt` (`node-wifi-mqtt.ino`_).
  Thanks Clemens and Karsten!


2016-09-17 0.9.0
================
- Add documentation about :ref:`scale-adjust-firmware`.
- Update firmware for :ref:`open-hive-firmware-gprs` "`node-gprs-http.ino`_": Modularize sensors and debugging. Thanks, Clemens!
- Add firmwares for Open Hive RFM69 tandem with build infrastructure and documentation

    - :ref:`open-hive-firmware-rfm69-node`: `node-rfm69-csv.ino`_
    - :ref:`open-hive-firmware-rfm69-gateway-sdcard`: `gateway-rfm69-csv-sdcard.ino`_

- Improve documentation


2016-07-08 0.8.1
================
- Add documentation about the new :ref:`generic-firmware`, "`generic.ino`_".
- Switch “node-gprs-http” sketch from Rob Tillaarts DHTlib to DHTstable, as suggested by Clemens. Thanks again!


2016-07-08 0.8.0
================
- Make building “generic” sketch for x86_64 a bit easier, just do ``cd generic; ./sim/build``.
- Add :ref:`open-hive-firmware-gprs` from Clemens Gruber as "`node-gprs-http.ino`_". Thanks, Clemens!
- Satisfy dependencies of “node-gprs-http” per git submodules (mostly), accompanied by Arduino Makefile yak shaving.
- Add :ref:`scale-adjust-firmware`. Thanks again, Clemens!
- Finish restructuring work on "generic" sketch. Proof-of-Concept "node69-transceiver69x95-gateway95" is working!


2016-07-04 0.7.1
================
- Improve documentation at :ref:`README`, :ref:`beradio-c++` and :ref:`firmware-builder`.


2016-07-04 0.7.0
================

General
-------
- Add sketch `generic.ino`_ as a module composition breadboard.
- Add Terrine_, an application boilerplate for convenient MCU development.
- Run generic sketch on x86_64 incl. pseudo radio link
  communication using RadioHead RH_TCP driver.

BERadio
-------
- Bring libberadio in shape as `BERadio C++`_.
- *BERadioMessage* does generic Bencode_ serialization on top
  of EmBencode_ using a map for holding sensor family values.
- Automatic message fragmentation based on MTU (maximum
  transfer unit) size for BERadioMessage.
- Last mile from BERadio message to radio transmission,
  use RadioHead RH_TCP_ driver on Unix.
- Move non-BERadio code to Terrine.

Libraries
---------
- Add RadioHead_ library, we are using the RH_RF69_ and RH_RF95_ drivers.
- Switch from vanilla EmBencode_ to Hiveeyes fork.
- Switch to branch “oo-improvements” of EmBencode fork.
- Add "Standard C++ for Arduino" library StandardCplusplus_.
- Switch from vanilla StandardCplusplus_ to Hiveeyes fork
  fixing an issue with gcc5.
- Switch submodule EmBencode_ to branch “oo-improvements”
- Make submodules EmBencode and StandardCplusplus track
  specific feature branches of Hiveeyes forks.
- Switch submodule EmBencode_ to branch “header-guard”
  fixing ``redefinition of 'class EmBencode’`` errors.
- Add guard to BERadio header file fixing ``redefinition
  of 'class BERadioMessage’`` errors.
- Switch to EmBencode branch "fix-typeinfo-reference".


2016-06-17 0.6.1
================
- Add Firmware-Builder helper to Linux Makefile for `node-gprs-any.ino`_.
- Update docs.


2016-06-16 0.6.0
================

General
-------
- Add project tooling `Arduino-Makefile`_ and `Bare-Arduino-Project`_ as git submodules under ``tools/``.

- Adapt vanilla Makefiles from Bare-Arduino-Project for vendor ``node-gprs-any``.

- Add Hiveeyes foundation libraries as proposal
  for decoupling sensor and transmitter domains.

- Add convention for automated firmware builder that
  ``make --file Makefile-Xyz.mk all-plus-firmware-info`` should
  output the path to the target HEX- and ELF files, e.g.::

      TARGET_HEX: ../bin/node-gprs-any/pro328/node-gprs-any.hex
      TARGET_ELF: ../bin/node-gprs-any/pro328/node-gprs-any.elf


Open Hive GPRSbee
-----------------
This piece of software supports the :ref:`openhive-seeeduino-stalker`
carrying a GPRSbee_ transceiver using the software components
Terkin_, `Hiveeyes-Lib`_ and `OpenHive-Lib`_.

- Add ArduinoJson_ and `GPRSbee library`_ as git submodules under ``libraries/``.

- Add example sketch `node-gprs-any.ino`_ demonstrating
  simple HTTP+JSON communication over GPRSbee.

- Update ``HE_API_URL`` to use different url for non-TLS
  communication: http://swarm.hiveeyes.org/api-notls/.
  The default GPRSbee devices don't do SSL.

- Connect to access point with authentication by default,
  using ``GPRSBEE_AP_USER`` and ``GPRSBEE_AP_PASS``.
  Disable authenticated mode by setting ``GPRSBEE_AP_AUTH``
  to ``false``.


Documentation
-------------
- Fix make target “docs-html”
- Upgrade to Sphinx 1.4.1
- Add “sphinx_rtf_theme==0.1.9” to requirements-dev.txt due to error
  "sphinx_rtd_theme is no longer a hard dependency since version 1.4.0. Please install it manually."
- Add label ``hiveeyes-arduino`` and ``changes`` to documentation
  for referencing from other Sphinx instances.
- Disable caching of remote intersphinx inventories


2016-05-26 0.5.2
================
- Add :ref:`libberadio` to evaluate whether a Standard C++ based firmware
  will still fit into the 32 KB flash memory of the little ATmega328_.
- Get Sphinx documentation in shape


2016-04-16 0.5.1
================
- Add Sphinx for documentation


2016-04-16 0.5.0
================
- Get real with HX711
- Polish repository for publishing on GitHub


2016-02-20 0.4.1
================
- Fix nasty battery draining


2016-02-03 0.4.0
================
- Add RADIO_ATC capability
- Update RFM69 library
- Get the node into the field
- Interval tuning


2015-11-12 0.3.0
================
- Update RFM69 library
- Add code for DS18B20 sensor
- Add MCU sleep modes (LowPower)
- Add serial-to-mqtt forwarding
- Design BERadio communication paradigm
- Work on update-over-the-air (WirelessProgramming)
- Add code for HX711 and DHT22 sensors
- Add RF-to-UART gateway code, see `gateway-rfm69-beradio.ino`_


2015-08-19 0.2.0
================
- Add Bencode [cccamp2015]


2015-02-03 0.1.0
================
- Switch to RFM69, see `node-rfm69-beradio.ino`_.
  This supports the :ref:`HEnode` hardware implementing a sensor node.


2014-11-04 0.0.0
================
- Start working on Hiveeyes Arduino using RFM12

