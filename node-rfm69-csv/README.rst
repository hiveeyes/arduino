.. include:: ../../resources.rst

.. _open-hive-firmware-rfm69-node:

###########################
Open Hive RFM69 sensor node
###########################

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/node-rfm69-csv/README.html>`_,
    all inline links will be working there.


************
Introduction
************
The `Open Hive Box`_ RFM69 is a sensor node transmitting data using radio link communication.
Telemetry data is serialized using CSV and transmitted
to the receiver node :ref:`open-hive-firmware-rfm69-gateway-sdcard`
or :ref:`open-hive-firmware-rfm69-gateway-uart`.
The most recent firmware version is available at `node-rfm69-csv.ino`_.

.. figure:: https://ptrace.hiveeyes.org/2016-07-08_open-hive_box-with-electronics.jpg
    :alt: Open Hive Box with electronics
    :width: 640px

.. graphviz:: synopsis.dot


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
    cd node-rfm69-csv

    # Select appropriate Makefile
    ln -s Makefile-Linux.mk Makefile

    # Build firmware
    make

    # Upload to MCU
    make upload

.. note:: You might want to adjust the appropriate Makefile to match your environment.

