.. include:: resources.rst

.. _firmware-overview:

#################
Firmware overview
#################

There are different firmwares for different flavours of sensor nodes.
We have single sensor nodes with GPRS uplink and tandem nodes using
RF radio links to communicate between multiple sensor nodes
and a single gateway node.

.. contents::
    :local:
    :depth: 2


----


******
Stable
******


GSM
===
List of GSM-based firmwares.

.. list-table::
    :widths: 20 40
    :header-rows: 1
    :class: table-generous

    * - Name
      - Description

    * - :ref:`open-hive-firmware-gprs`
      - `node-gprs-http.ino`_ is the firmware source code powering
        the `Open Hive Box`_. It transmits sensor data by HTTP
        using the GPRSbee_ GSM modem.


WiFi
====
List of WiFi-based firmwares, all are based on the ESP8266_.

.. list-table::
    :widths: 20 40
    :header-rows: 1
    :class: table-generous

    * - Name
      - Description

    * - :ref:`node-wifi-http`
      - `node-gprs-http.ino`_ is the firmware source code powering
        the `Open Hive WiFi Solar`_ sensor node based on the `ESP8266 core for Arduino`_.
        It transmits sensor data using WiFi/HTTP.

    * - :ref:`node-wifi-mqtt-homie`
      - `node-wifi-mqtt-homie.ino`_ is the firmware
        based on the `ESP8266 framework for Homie`_ for powering
        the »`ESP8266 beehive scale by Alexander Wilms <ESP8266 Stockwaage – erste Eindrücke_>`_«.
        It transmits sensor data using WiFi/MQTT.
        See also the alternative variant :ref:`node-wifi-mqtt-homie-battery`.

    * - :ref:`node-wifi-mqtt`
      - `node-wifi-mqtt.ino`_ is the source code for a preliminary/testing firmware powering
        the Open Hive `Adafruit Feather HUZZAH`_ based on the `ESP8266 core for Arduino`_.
        It transmits sensor data using WiFi/MQTT.


Yún
===
List of Yún-based firmwares.

.. list-table::
    :widths: 20 40
    :header-rows: 1
    :class: table-generous

    * - Name
      - Description

    * - :ref:`node-yun-http`
      - `node-yun-http.ino`_ is the firmware source code powering
        the `Mois Box`_. It transmits sensor data by HTTP
        using the `Bridge Library for Yún devices`_.


.. _firmwares-beradio:

Radio: BERadio
==============
List of radio-based firmwares using the :ref:`beradio:beradio` protocol.

.. list-table::
    :widths: 20 40
    :header-rows: 1
    :class: table-generous

    * - Name
      - Description

    * - node-rfm69-beradio
      - `node-rfm69-beradio.ino`_ collects sensor data, encodes it using
        :ref:`beradio:beradio` and sends it through a RFM69 radio
        transceiver module to a gateway node.
        This is the first version of the firmware running on
        :ref:`hiveeyes:hiveeyes-one` since February 2016.

    * - gateway-rfm69-beradio
      - `gateway-rfm69-beradio.ino`_ receives sensor data over radio
        and forwards it to the UART interface, which is connected to
        the serial port of the gateway machine.
        In turn, :ref:`beradio-python <beradio:beradio-python>` reads
        this data, decodes the :ref:`beradio:beradio` protocol and
        forwards it to the MQTT_ bus.


Radio: CSV
==========
List of radio-based firmwares using CSV serialization.

.. list-table::
    :widths: 20 40
    :header-rows: 1
    :class: table-generous

    * - Name
      - Description

    * - :ref:`open-hive-firmware-rfm69-node`
      - `node-rfm69-csv.ino`_ collects sensor data, encodes it using
        CSV and sends it through a RFM69 radio
        transceiver module to a gateway node.

    * - :ref:`open-hive-firmware-rfm69-gateway-uart`
      - `gateway-rfm69-csv-uart.ino`_ receives sensor data over radio
        and forwards it to the UART interface suitable for further downstreaming
        to MQTT using the :ref:`beradio-python <beradio:beradio-python>` gateway.

    * - :ref:`open-hive-firmware-rfm69-gateway-sdcard`
      - `gateway-rfm69-csv-sdcard.ino`_ receives sensor data over radio
        and stores it to a SD card on an Arduino Yún.

Tools
=====
List of auxiliary firmwares.

.. list-table::
    :widths: 20 40
    :header-rows: 1
    :class: table-generous

    * - Name
      - Description

    * - :ref:`scale-adjust-firmware`
      - Use these firmwares for adjusting the scale / load cell unit.


----


***********
Development
***********
The next generation firmwares are currently a work in progress.

.. list-table::
    :widths: 20 40
    :header-rows: 1
    :class: table-generous

    * - Name
      - Description

    * - :ref:`generic-firmware`
      - We started working on `generic.ino`_, a flexible software breadboard
        for RFM-based nodes, transceivers and gateways in June 2016.

    * - node-pipa
      - We started working on the *Hiveeyes Pipa Datalogger*, a playground
        for building a generic C++ datalogger framework in January 2017.
