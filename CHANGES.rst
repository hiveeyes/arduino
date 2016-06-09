.. include:: resources.rst

.. _changes:

#######
CHANGES
#######


in-progress
===========

General
-------
- Add project tooling as git submodules under ``tools/``

    - https://github.com/sudar/Arduino-Makefile
    - https://github.com/ladislas/Bare-Arduino-Project

- Adapt vanilla Makefiles from Bare-Arduino-Project for vendor ``node-gprs-any``

- Add Hiveeyes foundation libraries as proposal
  for decoupling sensor and transmitter domains

- Add convention for automated firmware builder that
  ``make --file Makefile-Xyz.mk firmware-info`` should
  output the path to the target ELF file, e.g.::

      TARGET_ELF: ../bin/node-gprs-any/pro328/node-gprs-any.elf


Open Hive GPRSbee
-----------------
- Add dependency libraries as git submodules under ``libraries/``

    - https://github.com/bblanchon/ArduinoJson
    - https://github.com/SodaqMoja/GPRSbee

- Add example sketch ``node-gprs-any`` demonstrating HTTP+JSON
  communication with the Hiveeyes backend over GPRSbee

- Use non-SSL endpoint http://swarm.hiveeyes.org/api/,
  the default GPRSbee devices don't do SSL

- ``node-gprs-any.ino``: Update ``HE_API_URL`` to use different url
  for non-TLS communication: http://swarm.hiveeyes.org/api-notls/


Documentation
-------------
- Fix make target “docs-html”
- Upgrade to Sphinx 1.4.1
- Add “sphinx_rtf_theme==0.1.9” to requirements-dev.txt due to

    .. error:: sphinx_rtd_theme is no longer a hard dependency since version 1.4.0. Please install it manually.

- Add label :ref:`hiveeyes-arduino` to documentation for referencing from other Sphinx instances
- Add label to CHANGES.rst
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


2015-08-19 0.2.0
================
- Add Bencode [cccamp2015]


2015-02-03 0.1.0
================
- Transmit temperature using RFM69


2014-11-04 0.0.0
================
- Start working on Hiveeyes Arduino using RFM12
