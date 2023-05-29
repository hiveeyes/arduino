.. include:: ../../../resources.rst

.. _mois-node:
.. _mois-firmware:
.. _node-yun-http:
.. _openhive-yun:

################
Mois sensor node
################

.. highlight:: bash

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/node-yun-http/README.html>`_,
    all inline links will be working there.


************
Introduction
************

This firmware powers the `Mois Box`_, based on the `Arduino Yún Shield`_-compatible `Dragino Yun Shield v2.4`_.
Telemetry data is transmitted to a `custom PHP receiver program <https://github.com/bee-mois/beescale/blob/master/add_line2.php>`_
over HTTP using the `Yún HTTP Client`_ from the `Bridge Library for Yún devices`_. It is also stored locally on a SD card.
The most recent firmware version is always available at `node-yun-http.ino`_.


.. figure:: https://ptrace.hiveeyes.org/2017-03-21_mois-node-yun-http.jpg
    :alt: Mois Box with electronics
    :width: 450px
    :align: left

    `Mois Box`_


.. figure:: http://www.dragino.com/media/k2/galleries/105/YunShieldv2.4_10.png
    :target: https://wiki1.dragino.com/index.php/Yun_Shield
    :alt: Dragino Yun Shield v2.4
    :width: 450px
    :align: right

    `Dragino Yun Shield v2.4`_


|clearfix|


Platform and supported peripherals
==================================

Board
-----
- `Dragino Yun Shield v2.4`_

Sensors
-------
- ADS1231_ ADC weigh scale breakout board
- DS18B20_ digital thermometer
- DHT22_ (RHT03_) digital humidity/temperature sensor
- `TSL25911`_ Ambient Light Sensor aka.
  `Adafruit TSL2591 STEMMA QT`_ High Dynamic Range Digital Light Sensor


*****
Setup
*****

Clone git repository
====================
::

    # Acquire source code repository
    git clone https://github.com/hiveeyes/arduino

    # Select this firmware
    cd arduino/openhive/openhive-yun


Configure firmware
==================
.. highlight:: c++

Have a look at the source code `node-yun-http.ino`_ and adapt feature flags and setting
variables according to your environment.

Configure load cell calibration settings::

    // Use sketches "scale-adjust-hx711.ino" or "scale-adjust-ads1231.ino" for calibration

    // The raw sensor value for "0 kg"
    const long loadCellZeroOffset = 38623;

    // The raw sensor value for a 1 kg weight load
    const long loadCellKgDivider  = 11026;

.. tip::

    Use :ref:`scale-adjust-firmware` to determine those values.


*****
Usage
*****

.. highlight:: bash

Build
=====

The build system is based on `PlatformIO`_, which will install toolchains and build
your customized firmware without efforts. All you need is a Python installation.

::

    make build

Upload to MCU
=============
::

    export MCU_PORT=/dev/ttyUSB0
    make upload


.. tip::

    If you need to build for different targets, or if you want to modernize your dependencies,
    you may want to adjust the ``platformio.ini`` file, to match your needs.

    In order to make changes to the firmware, edit the ``node-yun-http.ino`` file, and
    invoke ``make build`` to build it again.
