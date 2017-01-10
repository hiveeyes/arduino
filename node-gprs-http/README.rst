.. include:: ../../resources.rst

.. _open-hive-firmware:
.. _open-hive-firmware-gprs:
.. _node-gsm-http:
.. _node-wifi-http:

###################################
Open Hive GSM and WiFi sensor nodes
###################################
.. highlight:: bash

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/node-gprs-http/README.html>`_,
    all inline links will be working there.


************
Introduction
************
This hybrid firmware supports two different hardware devices.
Telemetry data is transmitted using HTTP. Both HX711_ and ADS1231_ load cell sensor chips are supported.
The most recent firmware version is available at `node-gprs-http.ino`_.

Platform and supported peripherals
==================================

Board
-----
- Open Hive GSM: `Seeeduino Stalker v2.3`_ with an ATmega328_ MCU or
- Open Hive WiFi: `Adafruit Feather HUZZAH`_ with an ESP8266_ MCU

Sensors
-------
- HX711_ ADC weigh scale breakout board or
- ADS1231_ ADC weigh scale breakout board
- DS18B20_ digital thermometer
- DHT33_ (RHT04_) digital humidity/temperature sensor


*******
Details
*******

Open Hive GSM
=============
The `Open Hive Box`_ is a beehive monitoring sensor node with GPRS transceiver based on an AVR ATmega328_.

.. figure:: https://ptrace.hiveeyes.org/2016-07-08_open-hive_box-with-electronics.jpg
    :alt: Open Hive Box with electronics
    :width: 640px

Open Hive WiFi
==============
A beehive monitoring sensor node based on the `Adafruit Feather HUZZAH`_, featuring an ESP8266_ MCU.

.. figure:: https://ptrace.hiveeyes.org/2016-06-17_openhive-huzzah.jpg
    :alt: Open Hive Adafruit Feather HUZZAH
    :width: 640px

Wiring
------
These Fritzing schematics describe the breadboard hardware wiring
of the MCU with its sensor equipment.

.. figure:: https://ptrace.hiveeyes.org/2016-11-01_esp8266-feather-hx711-ds18b20-dht33_breadboard_01-top_v0.3.png
    :alt: ESP8266 with HX711, DS18B20 and DHT33 on breadboard, top view.
    :width: 640px

    Top view

----

.. figure:: https://ptrace.hiveeyes.org/2016-11-01_esp8266-feather-hx711-ds18b20-dht33_breadboard_02-bottom_v0.3.png
    :alt: ESP8266 with HX711, DS18B20 and DHT33 on breadboard, bottom view.
    :width: 640px

    Bottom view.


*****
Setup
*****


Clone git repository
====================
::

    # Get hold of the source code repository including all dependencies
    git clone --recursive https://github.com/hiveeyes/arduino

    # Select this firmware
    cd node-gprs-http


Configure firmware
==================
Have a look at the source code `node-gprs-http.ino`_ and adapt feature flags according to your environment:

When using a HX711_ sensor::

    #define isScaleHX711

When using an ADS1231_ sensor::

    #define isScaleADS1231

Enable AVR ATmega328 with GPRSbee_ GSM modem::

    #define isGSM

    #define APN "internet.eplus.de"

Enable ESP8266 with integrated WiFi::

    #define isWifi

    #define WLAN_SSID  "your-ssid"
    #define WLAN_PW    "your-pw"


*****
Build
*****

Build for AVR
=============
::

    # Select appropriate Makefile
    ln -s Makefile-Linux.mk Makefile

    # Build firmware
    make

.. note:: You might want to adjust the appropriate Makefile to match your environment.


Upload to MCU
-------------
::

    make upload



Build for ESP8266
=================
Setup SDK::

    mkdir ~/sdk; cd ~/sdk
    git clone https://github.com/esp8266/Arduino esp8266-arduino

    # Download appropriate Espressif SDK
    cd esp8266-arduino/tools
    ./get.py

Announce path to SDK::

    export ESP_ROOT=~/sdk/esp8266-arduino

Build firmware::

    make -f Makefile-ESP8266.mk

Output when successful::

    # ...
    node-gprs-http.ino
    # ...
    Creating core archive
    Linking /tmp/mkESP/node-gprs-http_generic/node-gprs-http.bin
      Versions: 0.10.0-16-g62487f1-dirty, 2.3.0-88-g0291a6e

    Memory usage
      Ram:    34096 bytes
      Flash: 252637 bytes

    Build complete. Elapsed time: 2 seconds

Enable more verbose output::

    export VERBOSE=true


Upload to MCU
-------------
::

    make -f Makefile-ESP8266.mk upload

