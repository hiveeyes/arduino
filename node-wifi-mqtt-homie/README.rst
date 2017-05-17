.. include:: ../../resources.rst

.. _node-wifi-mqtt-homie:

####################################
ESP8266 beehive scale based on Homie
####################################

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
A beehive monitoring sensor node based on a ESP8266 module and a custom made PCB.
Telemetry data is transmitted using WiFi/MQTT.
The standard firmware is based on the `ESP8266 framework for Homie`_,
its most recent version is available at `node-wifi-mqtt-homie.ino`_.
However, the PCB (`ESP8266-BeeScale.fzz`_) can be used with any firmware.

.. figure:: https://www.imker-nettetal.de/wp-content/gallery/esp8266-gh/PCB-Built-in.jpg
    :alt: Open Hive ESP8266 BeeScale
    :width: 640px

Platform and supported peripherals
==================================
- AI-THINKER ESP8266-07 module with an ESP8266_ MCU
- HX711_ ADC weigh scale breakout board
- DS18B20_ digital thermometer

.. seealso::

    - `ESP8266 Stockwaage – erste Eindrücke`_
    - https://www.imker-nettetal.de/category/stockwaage/
    - Nodes with identical hardware: :ref:`node-wifi-http` and :ref:`node-wifi-mqtt`

.. note::

    There's also an alternative firmware :ref:`node-wifi-mqtt-homie-battery`.


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


Clone git repository
====================
::

    # Get hold of the source code repository including all dependencies
    git clone --recursive https://github.com/hiveeyes/arduino

    # Select this firmware
    cd node-wifi-mqtt-homie


Configure
=========
.. highlight:: c++

Have a look at the source code `node-wifi-mqtt-homie.ino`_ and adapt
setting variables according to your environment.

Configure load cell calibration settings::

    // Use sketch BeeScale-Calibration to get these values
    const float offset = 85107.00;    // Offset load cell
    const float cell_divider = 22.27; // Load cell divider


Build
=====
The build system is based on `makeESPArduino`_, a Makefile for ESP8286 Arduino projects.

.. highlight:: bash

Setup SDK::

    mkdir ~/sdk; cd ~/sdk
    git clone https://github.com/esp8266/Arduino esp8266-arduino

    # Download appropriate Espressif SDK
    cd esp8266-arduino/tools
    ./get.py

Build firmware::

    # Announce path to SDK
    export ESP_ROOT=~/sdk/esp8266-arduino

    # Run Makefile
    make

Enable more verbose output::

    export VERBOSE=true


Upload to MCU
=============
::

    make upload



*************
Configuration
*************
.. highlight:: bash

The Homie firmware offers different ways of runtime configuration.


HTTP API
========
When starting the ESP8266 device with Homie firmware, it will come up in ``configuration`` mode
and spawn a secure WiFi access point named ``Homie-xxxxxxxxxxxx``, like ``Homie-c631f278df44``.
Connect to it.

Configure the device by uploading the `JSON configuration file <json-configuration-file_>`_ to the `HTTP JSON API <http-json-api_>`_ at http://192.168.1.1::

    # 1. Clone real configuration from blueprint
    cp example.config.json config.json

    # 2. Edit configuration file
    sublime config.json

    # 3. Configure device
    curl -X PUT http://192.168.1.1/config --header "Content-Type: application/json" -d @config.json

.. seealso::

    `Connecting to the AP and configuring the device <https://homie-esp8266.readme.io/docs/getting-started#section-connecting-to-the-ap-and-configuring-the-device>`_

.. _http-json-api: https://homie-esp8266.readme.io/docs/http-json-api
.. _json-configuration-file: https://homie-esp8266.readme.io/docs/json-configuration-file


SPIFFS
======
The configuration file ``config.json`` can also be uploaded to the device's
SPIFFS filesystem as ``/homie/config.json``.

`makeESPArduino`_ also has the ``upload_fs`` target, so::

    # 1. Clone real configuration from blueprint
    cp example.config.json data/homie/config.json

    # 2. Edit configuration file
    sublime data/homie/config.json

    # 3. Configure device
    make upload_fs


.. seealso::

    - `The data/homie folder`_
    - `How to upload files to the ESP8266 SPIFFS file system`_
    - `How to build a file system with makeEspArduino`_

.. _The data/homie folder: https://github.com/marvinroger/homie-esp8266/tree/develop/data/homie
.. _How to upload files to the ESP8266 SPIFFS file system: https://esp8266.github.io/Arduino/versions/2.3.0/doc/filesystem.html#uploading-files-to-file-system
.. _How to build a file system with makeEspArduino: https://github.com/plerup/makeEspArduino#building-a-file-system


User interface
==============
When installing the `UI bundle`_ "``ui_bundle.gz``", the configuration UI will be served directly from the device::

    # 1. Download ui_bundle.gz
    wget --directory-prefix=data/homie http://setup.homie-esp8266.marvinroger.fr/ui_bundle.gz

    # 2. Configure device
    make upload_fs

.. _UI bundle: https://homie-esp8266.readme.io/docs/ui-bundle


****
Todo
****
- Improve documentation at all
- Document how to determine calibration values (i.e. "Use sketch BeeScale-Calibration to get these values").
  Would it be possible to build it into the main firmware and run it conditionally somehow?
- Document how to determine scale's temperature compensation values.


.. _Homie custom settings: https://homie-esp8266.readme.io/docs/custom-settings

