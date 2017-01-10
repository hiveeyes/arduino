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


*****************
Breadboard wiring
*****************
These Fritzing schematics describe the breadboard hardware wiring
of the MCU with its sensor equipment. It features:

- `Adafruit Feather HUZZAH`_ with an ESP8266_ MCU
- HX711_ ADC weigh scale breakout board
- DS18B20_ digital thermometer
- DHT33_ (RHT04_) digital humidity/temperature sensor


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


Build on your workstation
=========================
::

    # Get hold of the source code repository including all dependencies
    git clone --recursive https://github.com/hiveeyes/arduino

    # Select this firmware
    cd node-wifi-mqtt

.. todo::

    Describe how to build using

        - https://github.com/thunderace/Esp8266-Arduino-Makefile or
        - https://github.com/plerup/makeEspArduino


Upload to MCU
-------------

.. todo::

    Describe how to upload to the MCU using ``esptool``.

