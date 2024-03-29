.. include:: ../../../resources.rst

.. _open-hive-firmware-rfm69-gateway-uart:
.. _openhive-rfm69-gateway-uart:

###############################
Open Hive JeeLink radio gateway
###############################

.. tip::

    Please `continue reading this document on our documentation space
    <https://hiveeyes.org/docs/arduino/firmware/openhive/rfm69-gateway-uart/README.html>`_,
    all inline links will be working there.


************
Introduction
************
The Open Hive JeeLink radio gateway is a receiver/gateway node using radio link communication
based on the `JeeLink v3c`_ (`shop <JeeLink v3c shop_>`_).
Telemetry data is received from the :ref:`openhive-rfm69-node`,
decoded from CSV and forwarded to the UART interface suitable for further downstreaming
to MQTT using the :ref:`beradio-python <beradio:beradio-python>` gateway.
The most recent firmware version is available at `rfm69-gateway-uart.ino`_.

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

    # Acquire source code repository
    git clone https://github.com/hiveeyes/arduino

    # Select this firmware
    cd arduino/openhive/rfm69-gateway-uart

    # Build firmware
    make build

    # Upload to MCU
    make upload

.. tip:: Please inspect the ``platformio.ini`` file, and adjust it to match your setup.
