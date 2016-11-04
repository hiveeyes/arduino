.. include:: ../../resources.rst

.. _node-wifi-mqtt-homie:

#############################
Open Hive Feather sensor node
#############################

.. contents::
   :local:
   :depth: 1

----

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/node-wifi-mqtt-homie/README.html>`_,
    all inline links will be working there.


************
Introduction
************
A beehive monitoring sensor node based on the `Adafruit Feather HUZZAH`_, featuring an ESP8266_ MCU.
Telemetry data is transmitted using WiFi/MQTT.
The most recent firmware version is available at `node-wifi-mqtt.ino`_.

.. figure:: https://www.imker-nettetal.de/wp-content/gallery/esp8266-beescale/IMG_6685.JPG
    :alt: Open Hive ESP8266 BeeScale
    :width: 640px


*****************
PCB 
*****************

.. figure:: https://www.imker-nettetal.de/wp-content/gallery/esp8266-beescale/BienenESP_Leiterplatte.png
    :alt: ESP8266 BeeScale PCB - schematics - top view
    :width: 640px

    PCB - Schematics Top view

----

.. figure:: https://www.imker-nettetal.de/wp-content/gallery/esp8266-beescale/IMG_6682.JPG
    :alt: ESP8266 BeeScale PCB - top view
    :width: 640px

    PCB - Top view.

----

.. figure:: https://www.imker-nettetal.de/wp-content/gallery/esp8266-beescale/IMG_6683.JPG
    :alt: ESP8266 BeeScale PCB - bottom view
    :width: 640px

    PCB - Bottom view.

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
    cd node-wifi-mqtt-homie

.. todo::

    Describe how to build using

        - https://github.com/thunderace/Esp8266-Arduino-Makefile or
        - https://github.com/plerup/makeEspArduino


Upload to MCU
-------------

.. todo::

    Describe how to upload to the MCU using ``esptool``.

