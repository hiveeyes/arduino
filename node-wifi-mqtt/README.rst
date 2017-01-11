.. include:: ../../resources.rst

.. _node-wifi-mqtt:

###############################
Open Hive WiFi/MQTT sensor node
###############################

.. contents::
   :local:
   :depth: 1

----

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/node-wifi-mqtt/README.html>`_,
    all inline links will be working there.


************
Introduction
************
A beehive monitoring sensor node based on the `Adafruit Feather HUZZAH`_, featuring an ESP8266_ MCU.
Telemetry data is transmitted using WiFi/MQTT.
The most recent firmware version is available at `node-wifi-mqtt.ino`_.

.. figure:: https://ptrace.hiveeyes.org/2016-06-17_openhive-huzzah.jpg
    :alt: Open Hive Adafruit Feather HUZZAH
    :width: 640px


Platform and supported peripherals
==================================

- `Adafruit Feather HUZZAH`_ with an ESP8266_ MCU
- HX711_ ADC weigh scale breakout board
- DS18B20_ digital thermometer
- DHT33_ (RHT04_) digital humidity/temperature sensor



*******
Details
*******

Wiring
======
These Fritzing schematics describe the breadboard hardware wiring
of the MCU with its sensor equipment. It features:

.. figure:: https://ptrace.hiveeyes.org/2016-11-01_esp8266-feather-hx711-ds18b20-dht33_breadboard_01-top_v0.3.png
    :alt: ESP8266 with HX711, DS18B20 and DHT33 on breadboard, top view.
    :width: 640px

    Top view

----

.. figure:: https://ptrace.hiveeyes.org/2016-11-01_esp8266-feather-hx711-ds18b20-dht33_breadboard_02-bottom_v0.3.png
    :alt: ESP8266 with HX711, DS18B20 and DHT33 on breadboard, bottom view.
    :width: 640px

    Bottom view.


********
Firmware
********
.. highlight:: bash


Clone git repository
====================
::

    # Get hold of the source code repository including all dependencies
    git clone --recursive https://github.com/hiveeyes/arduino

    # Select this firmware
    cd node-wifi-mqtt


Configure
=========
Have a look at the source code `node-wifi-mqtt.ino`_ and adapt setting variables according to your environment.

Configure WiFi and MQTT settings::

    // WiFi parameters
    #define WLAN_SSID       "change-to-your-ssid"
    #define WLAN_PASS       "change-to-your-pw"

    // MQTT server
    #define MQTT_BROKER     "swarm.hiveeyes.org"
    #define MQTT_PORT       1883
    #define AIO_USERNAME    ""
    #define AIO_KEY         ""


Build
=====
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
    node-wifi-mqtt.ino
    # ...
    Creating core archive
    Linking /tmp/mkESP/node-wifi-mqtt_generic/node-wifi-mqtt.bin
      Versions: 0.11.0-4-gd839c68-dirty, 2.3.0-88-g0291a6e

    Memory usage
      Ram:    35744 bytes
      Flash: 274241 bytes

    Build complete. Elapsed time: 8 seconds

Enable more verbose output::

    export VERBOSE=true


Upload to MCU
=============
::

    make -f Makefile-ESP8266.mk upload

