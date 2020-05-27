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
There are firmwares for ATmega328_ and ESP8266_ MCUs.

* ``purpose``   role name, either *node* or *gateway*
* ``transport`` name of the physical transport mechanism, e.g. *rfm69*, *gprs* or *wifi*.
* ``protocol``  name of the transmission protocol, e.g. *beradio*, *mqtt*, *http* or *any*.

Most programs use `Arduino-Makefile`_ for building while there are still some based on Ino_.


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

Arduino IDE
-----------
If you want to use the Arduino IDE, you have to install all necessary libraries manually.
The sources of the libraries can be found in the header of each sketch (.ino).
In this case, copy the desired sketch into your Arduino IDE and pick the appropriate
libraries from the ``libraries/`` folder.

.. seealso:: https://www.arduino.cc/en/Guide/Libraries


Debian packages
---------------
::

    apt-get install arduino-core


Archlinux packages
------------------
* aur/arduino 1:1.6.8
* community/avr-libc


Mac OS X Homebrew packages
--------------------------
::

    # GNU AVR Toolchain as formulae for Homebrew
    # https://github.com/osx-cross/homebrew-avr
    brew tap osx-cross/avr
    brew install avr-gcc

::

    # The Arduino HAL
    brew cask install arduino


*******
Operate
*******
For operative tasks like compiling, `Arduino-Makefile`_ is in the loop.


Compile
=======
::

    cd node-gprs-any
    make --file Makefile-Linux.mk

After a successful build, find the firmware at ``bin/node-gprs-any/pro328/node-gprs-any.hex``.


Upload to MCU
=============
To build a firmware hex file, upload it to the MCU and
finally start a serial port monitor, do::

    cd generic

    # Prepare appropriate Makefile (use Makefile-OSX.mk on OSX)
    ln -s Makefile-Linux.mk Makefile

    # Compile and upload to chip
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

