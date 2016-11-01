.. include:: ../../resources.rst

.. _open-hive-firmware-rfm69-gateway-uart:

###############################
Open Hive JeeLink radio gateway
###############################

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/gateway-rfm69-csv-uart/README.html>`_,
    all inline links will be working there.


************
Introduction
************
The Open Hive JeeLink radio gateway is a receiver/gateway node using radio link communication
based on the `JeeLink v3c`_ (`shop <JeeLink v3c shop_>`_).
Telemetry data is received from the :ref:`open-hive-firmware-rfm69-node`,
decoded from CSV and forwared to the UART interface suitable for further downstreaming
to MQTT using the :ref:`beradio-python <beradio:beradio-python>` gateway.
The most recent firmware version is available at `gateway-rfm69-csv-uart.ino`_.

.. figure:: https://ptrace.hiveeyes.org/2016-11-01_jeelink-top_large.jpg
    :alt: Open Hive JeeLink Radio Gateway
    :width: 640px
    :target: `JeeLink v3c shop`_

    JeeLink v3c


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
    cd gateway-rfm69-csv-uart

    # Select appropriate Makefile
    ln -s Makefile-Linux.mk Makefile

    # Build firmware
    make

    # Upload to MCU
    make upload

.. note:: You might want to adjust the appropriate Makefile to match your environment.

