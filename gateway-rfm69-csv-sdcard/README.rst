.. include:: ../../resources.rst

.. _open-hive-firmware-rfm69-gateway-sdcard:

#############################
Open Hive Arduino Yún gateway
#############################

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/gateway-rfm69-csv-sdcard/README.html>`_,
    all inline links will be working there.


************
Introduction
************
This firmware runs on the Open Hive RFM69 Arduino Yún,
a receiver node using radio link communication.
Telemetry data is received from the :ref:`open-hive-firmware-rfm69-node`,
decoded from CSV and stored to a SD card.
The most recent version is available at `gateway-rfm69-csv-sdcard.ino`_.

.. graphviz:: ../node-rfm69-csv/synopsis.dot

.. figure:: https://ptrace.hiveeyes.org/2016-06-17_openhive-yun-uno.jpg
    :alt: Open Hive RFM69 Arduino Yún
    :width: 640px


*****
Build
*****
.. highlight:: bash


Build on your workstation
=========================
::

    # Get hold of the source code repository including all dependencies
    git clone --recursive https://github.com/hiveeyes/arduino

    # Select this firmware
    cd gateway-rfm69-csv-sdcard

    # Build firmware
    make build

    # Upload to MCU
    make upload

.. note:: You might want to adjust the appropriate Makefile to match your environment.

