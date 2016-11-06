.. include:: ../../resources.rst

.. _node-wifi-mqtt-homie:

##################################
Open Hive WiFi / Homie sensor node
##################################

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
A beehive monitoring sensor node based on a ESP8266-07 module and a custom made PCB.
Telemetry data is transmitted using WiFi/MQTT.
The standard firmware is based on the `ESP8266 framework for Homie`_, its most recent
version is available at `node-wifi-mqtt-homie.ino`_.
However, the PCB (`ESP8266-BeeScale.fzz`_) can be used independently of the Homie/Hiveeyes
software stack.

.. seealso::

    - `ESP8266 Stockwaage – erste Eindrücke`_
    - https://www.imker-nettetal.de/category/stockwaage/

.. figure:: https://www.imker-nettetal.de/wp-content/gallery/esp8266-gh/PCB-Built-in.jpg
    :alt: Open Hive ESP8266 BeeScale
    :width: 640px


***
PCB
***

The PCB can be downloaded in Fritzing format from `ESP8266-BeeScale.fzz`_.

.. figure:: https://www.imker-nettetal.de/wp-content/gallery/esp8266-beescale/BienenESP_Leiterplatte.png
    :alt: ESP8266 BeeScale PCB - schematics - top view
    :width: 640px

    PCB - Schematics Top view

----

.. figure:: https://www.imker-nettetal.de/wp-content/gallery/esp8266-gh/PCB-Front.jpg
    :alt: ESP8266 BeeScale PCB - top view
    :width: 640px

    PCB - Top view.

----

.. figure:: https://www.imker-nettetal.de/wp-content/gallery/esp8266-gh/PCB-Back.jpg
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

