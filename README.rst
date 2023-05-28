.. include:: resources.rst

.. _README:

#######################
Hiveeyes Arduino README
#######################

.. image:: https://github.com/hiveeyes/arduino/actions/workflows/platformio-ci.yaml/badge.svg
    :target: https://github.com/hiveeyes/arduino/actions/workflows/platformio-ci.yaml
    :alt: PlatformIO CI outcome

» `Documentation`_
| `Forum`_
| `Issues`_
| `Source code`_

.. tip::

    Please `continue reading this document on our documentation space
    <https://hiveeyes.org/docs/arduino/>`_,
    all inline links will be working there.


************
Introduction
************

This is the documentation for the `Hiveeyes Arduino repository`_ of the `Hiveeyes project`_,
conceiving an open source and open hardware beehive monitoring system.

All Arduino Core based firmware source code is living here.
It supports different hardware, so there are different flavors
of firmwares, reflected by the directory naming convention
``<purpose>-<transport>-<protocol>``.

There are firmwares for ATmega328_, ESP8266_, and ESP32_ MCUs.

* ``purpose``   role name, either *node* or *gateway*
* ``transport`` name of the physical transport mechanism, e.g. *rfm69*, *gprs* or *wifi*.
* ``protocol``  name of the transmission protocol, e.g. *beradio*, *mqtt*, *http* or *any*.

For building, all programs are using `PlatformIO`_.


*****
Setup
*****
.. highlight:: bash

Source code
===========
Acquire source code repository::

    git clone https://github.com/hiveeyes/arduino

Toolchain
=========

PlatformIO
----------
We recommend to use `PlatformIO`_. Installing all dependency libraries will be managed
automatically, accurately, and reproducibly, because each programs' subdirectory contains
a ``platformio.ini`` file, which enumerates the list of dependencies within the
``env.lib_deps`` sections, like:

.. code-block:: ini

    [env]
    lib_deps =
        lowpowerlab/RFM69@^1.5
        lowpowerlab/SPIFlash@^101
        https://github.com/hiveeyes/embencode

You will be able to choose between CLI and UI operations, see `PlatformIO IDE`_.

When using PlatformIO, we recommend to turn off telemetry, by running that command
within your Python environment you are running ``pio`` in::

    pio settings set enable_telemetry false

Arduino IDE
-----------
If you want to use the Arduino IDE, you will have to install all necessary libraries
manually. We do not recommend it, because it can produce arbitrary results when
building and running the firmware.

If you absolutely have to use it, to give you a clue about the required dependencies
for each individual firmware, please inspect the ``env.lib_deps`` sections within the
``platformio.ini`` files.


*****
Usage
*****

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
.. _Documentation: https://hiveeyes.org/docs/arduino/
.. _Forum: https://community.hiveeyes.org/
.. _Issues: https://github.com/hiveeyes/arduino/issues
.. _Source code: https://github.com/hiveeyes/arduino
