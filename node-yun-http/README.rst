.. include:: ../../resources.rst

.. _mois-node:
.. _mois-firmware:
.. _node-yun-http:

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
- TSL25911_ Ambient Light Sensor aka.
  TSL2591_ High Dynamic Range Digital Light Sensor


*****
Setup
*****

Clone git repository
====================
::

    # Get hold of the source code repository including all dependencies
    git clone --recursive https://github.com/hiveeyes/arduino

    # Select this firmware
    cd node-yun-http


Configure firmware
==================
.. highlight:: c++

Have a look at the source code `node-yun-http.ino`_ and adapt
feature flags and setting variables according to your environment:

Configure load cell calibration settings::

    // Use sketches "scale-adjust-hx711.ino" or "scale-adjust-ads1231.ino" for calibration

    // The raw sensor value for "0 kg"
    const long loadCellZeroOffset = 38623;

    // The raw sensor value for a 1 kg weight load
    const long loadCellKgDivider  = 11026;

.. tip::

    Read about :ref:`scale-adjust-firmware` to get these values.


*****
Build
*****
.. highlight:: bash

Build for AVR
=============
The build system is based on `Arduino-Makefile`_, a Makefile for Arduino projects.

::

    # Select appropriate Makefile
    ln -s Makefile-Linux.mk Makefile

    # Build firmware
    make

.. note:: You might want to adjust the appropriate Makefile to match your environment.


Upload to MCU
-------------
::

    make upload

