.. include:: resources.rst

.. Hiveeyes Arduino documentation master file, created by
   sphinx-quickstart on Sat Apr 16 23:19:45 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. _hiveeyes-arduino:

##########################
Hiveeyes Arduino Firmwares
##########################

.. image:: https://github.com/hiveeyes/arduino/actions/workflows/platformio-ci.yaml/badge.svg
    :target: https://github.com/hiveeyes/arduino/actions/workflows/platformio-ci.yaml
    :alt: PlatformIO CI outcome

» `Documentation`_
| `Community Forum`_
| `Issues`_
| `Source code`_
| `Licenses`_

.. toctree::
    :maxdepth: 1
    :hidden:

    firmware/overview
    BERadio/README
    TerkinData/README
    firmware-builder
    project/index


*****
About
*****

This is the firmware source code documentation of the `Hiveeyes project`_, which is
developing a flexible beehive monitoring infrastructure toolkit and platform based
on affordable hardware, wireless telemetry and modern software.

As a FLOSS system, it is assembled from a range of open source software and hardware
components. For an overview about the whole system, please have a look at
:ref:`hiveeyes-system`.

Please also visit the `Hiveeyes Community Forum`_, where all development discussions
are taking place. If you want to participate in one way or another, you are most
welcome to join us.

Overview
========

The repository includes different firmware implementations for different scenarios
and use cases. The :ref:`firmware-overview` page enumerates all of them.

Coverage
========

MCUs
====

- `ATmega328`_, `ESP8266`_, `ESP32`_

Sensors
=======

-  `HX711`_, `BME280`_, `DS18B20`_, `DHT22`_, `TSL25911`_

Telemetry transports
====================

-  `Cellular`_: `GSM`_, `GPRS`_, `LTE Cat M1`_, `LTE Cat NB1`_.
-  `ISM radio`_: Sub-1GHz transmitter, RF packet radio `RFM69`_ and `RFM95`_.
-  `ISM radio`_: `LoRa`_ and `LoRaWAN`_ telemetry.
-  Classic `Wi-Fi`_.

Telemetry protocols
===================

- :ref:`BERadio <beradio:beradio-spec>`, `MQTT`_, `HTTP`_

----

.. note::

    Some firmware names reflect their purpose using a naming convention like
    ``<purpose>-<transport>-<protocol>``. Otherwise, please inspect their documentation
    and source code to find out about their ingredients more exactly.


*****
Usage
*****

This repository uses `PlatformIO`_ as a build system, see :ref:`about-platformio`.

Install
=======
.. highlight:: bash

Acquire source code repository::

    git clone https://github.com/hiveeyes/arduino


Build
=====

Select a firmware according to your needs, and build it::

    cd arduino/ringlabs/bienenwaage-5.0
    make build

For selecting the right firmware for your purposes, please inspect the
:ref:`firmware-overview` page.

Upload
======

To build a firmware hex file, upload it to the MCU and
finally start a serial port monitor, invoke::

    export MCU_PORT=/dev/ttyUSB0
    make upload

    # Run serial port monitor
    make monitor



.. _about-platformio:

****************
About PlatformIO
****************

For building, we recommend to use `PlatformIO`_. Installing all dependency packages
will be managed automatically, accurately, and reproducibly, because each programs'
subdirectory contains a ``platformio.ini`` file, which exactly enumerates its list of
dependencies. You will be able to choose between CLI and UI operations, see `PlatformIO IDE`_.

.. note::

    A ``platformio.ini`` dependency list, configured per ``env.lib_deps`` setting,
    typically looks like this. You can see that libraries can be pulled from both
    packages hosted on PlatformIO, and from Git repositories.

    For both variants, you will be able to specify exactly which version of the package
    to use (version pinning), where on Git repositories, you can refer to both tags and
    branches. This is very convenient for development operations, and also provides a
    lightweight way for bundling/shipping to production environments.

    .. code-block:: ini

        [env]
        lib_deps =
            lowpowerlab/RFM69@^1.5
            lowpowerlab/SPIFlash@^101
            https://github.com/hiveeyes/aerowind-ads1231@^0.1.0
            https://github.com/hiveeyes/embencode#next

.. attention::

    We do not recommend to use the Arduino IDE, because you will have to install all
    necessary libraries manually, sometimes by choosing libraries from the library
    manager, another time by downloading Zip archives and extracting their content
    to certain directories.

    This procedure is error-prone, and can produce arbitrary results when building and
    running the firmware. It will not be reproducible, and you are on your own when
    running into any problems around library dependencies, or missing hardware support.

    Specifically, if you are working on different projects with different sets of
    dependencies across MCU architectures, framework SDKs, and libraries, you will
    find yourself entangled in a web of unknown library dependencies, and, in order
    to re-gain your sanity, you will more often than not need to wipe your whole
    development environment or even the whole Arduino installation, and start from
    scratch.

    **Software should not be developed and assembled this way in 2023.**

    `PlatformIO`_ will give you peace of mind on this matters, because each project's
    build environment is isolated from each other.

    The `PlatformIO Registry`_ hosts packages and SDKs for 50 development platforms
    and >13,000 libraries. If you absolutely can't find a specific library there,
    just include it with your program, or upload it to a Git repository.

    If you *absolutely have to use* the Arduino IDE, you may want to inspect the
    ``env.lib_deps`` sections within the corresponding ``platformio.ini`` files,
    in order to learn about the required dependencies for each individual firmware.

.. tip::

    Depending on your use case, when using PlatformIO, you may want to turn off telemetry,
    by running that command within the Python environment you are running ``pio`` in:

    .. code-block:: shell

        pio settings set enable_telemetry false


.. _Community Forum: https://community.hiveeyes.org/
.. _Documentation: https://hiveeyes.org/docs/arduino/
.. _Hiveeyes Community Forum: https://community.hiveeyes.org/
.. _Issues: https://github.com/hiveeyes/arduino/issues
.. _Licenses: https://hiveeyes.org/docs/arduino/project/licenses.html
.. _Source code: https://github.com/hiveeyes/arduino
