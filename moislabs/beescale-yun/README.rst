.. include:: ../../../resources.rst

.. _mois-node:
.. _mois-firmware:
.. _node-yun-http:
.. _beescale-yun:

######################
Mois Labs Beescale Yún
######################

.. tip::

    Please `continue reading this document on our documentation space
    <https://hiveeyes.org/docs/arduino/firmware/moislabs/beescale-yun/README.html>`_,
    all inline links will be working there.


.. highlight:: bash

*****
About
*****

The second generation `Mois Box`_ is a beehive monitoring system based on the
`Arduino Yún Shield`_-compatible `Dragino Yun Shield v2.4`_.

.. figure:: https://ptrace.hiveeyes.org/2017-03-21_mois-node-yun-http.jpg
    :alt: Mois Box with electronics
    :width: 350px
    :align: left

    `Mois Box`_

.. figure:: http://www.dragino.com/media/k2/galleries/105/YunShieldv2.4_10.png
    :target: https://wiki1.dragino.com/index.php/Yun_Shield
    :alt: Dragino Yun Shield v2.4
    :width: 350px
    :align: right

    `Dragino Yun Shield v2.4`_

|clearfix|

Project repository
==================

`@bee-mois`_ is maintaining the full sources, including all auxiliary helper programs,
within the `beescale repository`_.


********
Hardware
********

Board
=====
- `Dragino Yun Shield v2.4`_

Sensors
=======
- ADS1231_ ADC weigh scale breakout board
- DS18B20_ digital thermometer
- DHT22_ (RHT03_) digital humidity/temperature sensor
- `TSL25911`_ Ambient Light Sensor aka.
  `Adafruit TSL2591 STEMMA QT`_ High Dynamic Range Digital Light Sensor



********
Firmware
********

The most recent firmware version is available at `beescale-yun.ino`_.

Acquire source code
===================
::

    # Acquire source code repository
    git clone https://github.com/hiveeyes/arduino

    # Select this firmware
    cd arduino/moislabs/beescale-yun


Configure
=========
.. highlight:: c++

Have a look at the source code `beescale-yun.ino`_ and adapt feature flags and setting
variables according to your environment.

Configure load cell calibration settings::

    // Use sketches "scale-adjust-hx711.ino" or "scale-adjust-ads1231.ino" for calibration

    // The raw sensor value for "0 kg"
    const long loadCellZeroOffset = 38623;

    // The raw sensor value for a 1 kg weight load
    const long loadCellKgDivider  = 11026;

.. tip::

    Please use the corresponding :ref:`scale-adjust-firmware` to determine those values.

Build
=====

.. highlight:: bash

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

    In order to make changes to the firmware, edit the ``beescale-yun.ino`` file, and
    invoke ``make build`` to build it again.


*********************
Telemetry and backend
*********************

The `Bridge Library for Yún devices`_ enables communication between the `ATmega328P`_ MCU
on the `Arduino Uno`_, and the `AR9331`_, in order to submit data from the sensor domain
to the Linux machine. It will enable transparent HTTP communication using the venerable
Arduino `HttpClient`_ library.

.. code-block:: c++

    #include <Bridge.h>
    #include <HttpClient.h>

    HttpClient client;
    client.post(url);

Using this HTTP client (example implementation at `Yún HTTP Client`_), telemetry data is
transmitted to a `custom PHP receiver program`_, and is also stored on the SD card attached
to the device.


.. _AR9331: http://en.techinfodepot.shoutwiki.com/wiki/Atheros_AR9331
.. _@bee-mois: https://github.com/bee-mois
.. _beescale repository: https://github.com/bee-mois/beescale
.. _custom PHP receiver program: https://github.com/bee-mois/beescale/blob/master/add_line2.php
.. _HttpClient: https://www.arduino.cc/reference/en/libraries/httpclient/
