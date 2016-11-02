.. include:: ../../resources.rst

.. _open-hive-firmware:

.. _open-hive-firmware-gprs:

##########################
Open Hive GPRS sensor node
##########################

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/node-gprs-http/README.html>`_,
    all inline links will be working there.


************
Introduction
************
The `Open Hive Box`_ is a beehive monitoring sensor node with GPRS transceiver.
Telemetry data is transmitted using HTTP.
The most recent firmware version is available at `node-gprs-http.ino`_.

.. figure:: https://ptrace.hiveeyes.org/2016-07-08_open-hive_box-with-electronics.jpg
    :alt: Open Hive Box with electronics
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
    cd node-gprs-http

    # Select appropriate Makefile
    ln -s Makefile-Linux.mk Makefile

    # Build firmware
    make

    # Upload to MCU
    make upload

.. note:: You might want to adjust the appropriate Makefile to match your environment.

