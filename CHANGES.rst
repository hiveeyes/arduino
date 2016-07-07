.. include:: resources.rst

.. _changes:

#######
CHANGES
#######


in-progress
===========
- Make building “generic” sketch for x86_64 a bit easier, just do ``cd generic; ./sim/build``.
- Add vanilla Open Hive sensor node sketch from Clemens Gruber as “node-gprs-http.ino”. Thanks, Clemens!
- Satisfy dependencies of “node-gprs-http” per git submodules (mostly), accompanied by Arduino Makefile yak shaving.
- Add documentation about “node-gprs-http” at :ref:`open-hive-firmware`.
- Add load cell adjustment sketches. Thanks again, Clemens!


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
- Bring libberadio in shape as `BERadio C++ <BERadio-Lib_>`_.
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

