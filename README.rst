.. include:: resources.rst

.. _README:

######
README
######

.. contents::
   :local:
   :depth: 1

----

.. tip::

    You might want to `continue reading on our documentation space <https://hiveeyes.org/docs/arduino/>`_,
    all inline links will be working there.


************
Introduction
************

This is the documentation for the `Hiveeyes Arduino repository`_ of the `Hiveeyes project`_,
a beehive monitoring system. Enjoy reading!

All Arduino Core based firmware source code is living here.
We support different hardware, so there are different flavors
of the firmware, reflected by the directory naming convention
``<purpose>-<transport>-<protocol>``.

There are firmwares for ATmega328_, ESP8266_ and ESP32_ MCUs.

* ``purpose``   role name, either *node* or *gateway*
* ``transport`` name of the physical transport mechanism, e.g. *rfm69*, *gprs* or *wifi*.
* ``protocol``  name of the transmission protocol, e.g. *beradio*, *mqtt*, *http* or *any*.

For building, most programs are using PlatformIO_, but some are still using
`Arduino-Makefile`_ and even others are still based on Ino_.


*****
Setup
*****
.. highlight:: bash


Source code
===========
Get source code with all dependency libraries and tools::

    git clone --recursive https://github.com/hiveeyes/arduino


Toolchain
=========

PlatformIO_ is the recommended build environment.

Arduino IDE
-----------
If you want to use the Arduino IDE, you have to install all necessary libraries manually.
In each programs subdirectory, there is a ``platformio.ini`` which will give you a clue
about the libraries to use.

PlatformIO
----------
When using PlatformIO_, installing all dependency libraries will be managed automatically.


*******
Operate
*******


Compile
=======
::

    cd node-esp32-generic
    make



Upload to MCU
=============
To build a firmware hex file, upload it to the MCU and
finally start a serial port monitor, do::

    export MCU_PORT=/dev/ttyUSB0
    make upload

    # Run serial port monitor
    make monitor


*******
License
*******


Software
========
All source codes are licensed under the GNU General Public License, see also "LICENSE.txt".


Hardware
========
All hardware designs and related things are licensed under the `CERN Open Hardware Licence v1.2`_.

.. _CERN Open Hardware Licence v1.2: http://www.ohwr.org/licenses/cern-ohl/v1.2


Documentation
=============
Licensed under the Creative Commons `CC-BY-SA 4.0 license`_.

.. _CC-BY-SA 4.0 license: https://creativecommons.org/licenses/by-sa/4.0/
