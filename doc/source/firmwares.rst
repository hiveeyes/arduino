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


GPRS
====
List of GPRS-based firmwares.

.. list-table::
    :widths: 20 40
    :header-rows: 1
    :class: table-generous

    * - Name
      - Description

    * - :ref:`open-hive-firmware-gprs`
      - `node-gprs-http.ino`_ is the firmware source code powering
        the `Open Hive Box`_. It transmits sensor data by HTTP
        using a GPRSbee_ GPRS modem.


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
List of tool firmware.

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

    * - node-gprs-any
      - We started working on `node-gprs-any.ino`_, a playground sketch
        for building a generic transmitter machinery in June 2016.

    * - :ref:`generic-firmware`
      - We started working on `generic.ino`_, a flexible software breadboard
        for nodes, transceivers and gateways in June 2016.

