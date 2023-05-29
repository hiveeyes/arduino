.. include:: ../../../resources.rst

.. _open-hive-firmware-rfm69-gateway-sdcard:
.. _openhive-rfm69-gateway-sdcard:

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
Telemetry data is received from the :ref:`openhive-rfm69-node`,
decoded from CSV and stored to a SD card.
The most recent version is available at `rfm69-gateway-sdcard.ino`_.

.. graphviz:: ../rfm69-node/synopsis.dot

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

    # Acquire source code repository
    git clone https://github.com/hiveeyes/arduino

    # Select this firmware
    cd arduino/openhive/rfm69-gateway-sdcard

    # Build firmware
    make build

    # Upload to MCU
    make upload

.. tip:: Please inspect the ``platformio.ini`` file, and adjust it to match your setup.
