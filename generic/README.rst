
.. _generic-firmware:

################
Generic firmware
################

*****
Intro
*****
A flexible software breadboard for sensor-, transceiver- and gateway-nodes.
This is currently a work in progress, as of June 2016.


********
Synopsis
********
Generic sketch `generic.ino`_ running on

- a sensor node emitting telemetry data via RFM60
- a transceiver node receiving RFM69 and emitting RFM95 LoRa
- a gateway node receiving RFM95 LoRa and sending it to the UART interface connected to the gateway SoC

.. figure:: https://ptrace.hiveeyes.org/2016-07-08_Hiveeyes%20generic%20node%20RFM69-to-RFM95%20picocom.png
    :alt: Three picocom instances connected to each of the three networked nodes
    :width: 1024px



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
    cd generic

    # Select appropriate Makefile
    ln -s Makefile-Linux.mk Makefile

    # Build firmware
    make

    # Upload to MCU
    make upload

.. note:: You might want to adjust the appropriate Makefile to match your environment.



.. External resources

.. _generic.ino: https://github.com/hiveeyes/arduino/blob/master/generic/generic.ino

