.. include:: resources.rst

.. _firmware-builder:

################
Firmware builder
################

.. contents::
    :local:
    :depth: 1

----

************
Introduction
************
The build system is integrated with a flexible infrastructure for hands-off firmware building.

Customized firmwares can be built without having any toolchain installed
on your workstation by just issuing a HTTP POST request to the
:ref:`firmware builder subsystem <kotori:firmware-builder>`.

This infrastructure pulls the source code including dependencies from the
`Hiveeyes Arduino repository`_, replaces user-defined variables in the main
sketch as well as the ``Makefile`` and delivers customized firmwares based
on an universal code base.

The main use case for this is to enable everyone to build her own firmwares
without installing any toolchain at all. The most prominent example for customizing
user-defined variables would be the Hiveeyes WAN device address in form of the triple
(``HE_USER``, ``HE_SITE``, ``HE_HIVE``) or the access point configuration parameters
for setting up a GPRS device for communication in form of the quadruple
(``GPRSBEE_AP_NAME``, ``GPRSBEE_AP_AUTH``, ``GPRSBEE_AP_USER`` and ``GPRSBEE_AP_PASS``).

For getting an idea about the variable replacements we are aiming at with an example sketch
suitable for automatic building using the variables described above, please have a look at
`node-gprs-any.ino, line 81 ff. <https://github.com/hiveeyes/arduino/blob/master/node-gprs-any/node-gprs-any.ino#L81#>`_

.. attention::

    This is a work in progress, don't expect everything to work without effort yet.
    Testers and contributors are very welcome!

    The blueprint is the fine Cloud Build Service of the NodeMCU community:

        - https://nodemcu.readthedocs.io/en/master/en/build/
        - https://nodemcu-build.com/


********************
Build node-gprs-http
********************

The firmware source code `node-gprs-http.ino`_ of the :ref:`open-hive-firmware-gprs`
is pulled from the master branch of the `Hiveeyes Arduino repository`_ on GitHub.
Specify appropriate parameters to match your environment.

.. todo::

    How to configure the sensor node address (here ``testdrive/area-42/node-1``)
    and other firmware parameters?


HTTPie
======
HTTPie_ is a cURL-like tool for humans for conveniently issuing HTTP requests from the command line.

Setup
-----
::

    # Debian-based systems
    aptitude install httpie

    # Mac OS X
    sudo port install httpie


Build
-----
Issue a HTTP POST request to build and obtain the firmware hex file::

    time http --timeout=120 POST https://swarm.hiveeyes.org/api/hiveeyes/testdrive/area-42/node-1/firmware.hex \
        ref=master path=node-gprs-http makefile=Makefile-FWB.mk \
        GPRSBEE_AP_NAME=internet.eplus.de GPRSBEE_AP_USER=barney@blau.de GPRSBEE_AP_PASS=789 \
        --download

    Downloading to "hiveeyes_node-gprs-http_pro328-atmega328p_af495adf-GPRSBEE_AP_NAME=internet.eplus.de,GPRSBEE_AP_PASS=789,HE_SITE=area-42,GPRSBEE_AP_USER=barney@blau.de,HE_USER=testdrive,HE_HIVE=node-1.hex"
    Done. 72.18 kB in 0.17865s (404.03 kB/s)

    real	0m19.154s


HttpRequester
=============
The HttpRequester_ add-on for Firefox is a graphical user interface for issuing HTTP requests.

Issue a HTTP POST request to acquire a firmware from the :ref:`firmware builder subsystem <kotori:firmware-builder>`.
Please set the appropriate firmware build- and configuration parameters on the left side of the screen.

.. figure:: https://ptrace.hiveeyes.org/2016-07-07_Kotori%20Firmware%20Builder%20HttpRequester.jpg
    :alt: Kotori Firmware Builder HttpRequester
    :width: 1024px

After sending the HTTP POST request by hitting "Submit", just copy/paste the
response content on the right side of the screen into a .hex file and upload to the MCU.

.. tip::

    As the available configuration parameters and possible values are currently not documented
    in detail, please have a look at the source code (`node-gprs-http.ino`_). All preprocessor
    variables (``#define ...``) can be changed through HTTP POST parameters.



*******************
Build node-gprs-any
*******************
.. highlight:: bash

Just send a HTTP POST request to the ``..../firmware.hex``
endpoint using the HTTP client of your choice. Pass the
user-defined variables in JSON or x-www-form-urlencoded
format in the request body.

Setup HTTP client::

    # Debian-based systems
    aptitude install httpie

    # Mac OS X
    sudo port install httpie

Acquire firmware::

    export DEVICE_TOPIC=Hotzenplotz/Buxtehude/Raeuberhoehle

    http --timeout=120 --download POST \
        https://swarm.hiveeyes.org/api/hiveeyes/$DEVICE_TOPIC/firmware.hex \
        ref=master path=node-gprs-any makefile=Makefile-Linux.mk \
        GPRSBEE_AP_NAME=internet.eplus.de GPRSBEE_AP_USER=barney@blau.de GPRSBEE_AP_PASS=123

This should deliver a hex file ready for programming::

    Downloading to "hiveeyes_node-gprs-any_pro328-atmega328p_a05548bf-GPRSBEE_AP_NAME=internet.eplus.de,GPRSBEE_AP_PASS=123,HE_SITE=area-42,HE_HIVE=node-1,HE_USER=testdrive,GPRSBEE_AP_USER=barney@blau.de.hex"
    Done. 53.57 kB in 0.00064s (81.48 MB/s)

Given, the filename is long, but it includes every parameter
to distinguish different build artifacts from each other.

.. tip::

    - There's also an endpoint with suffix ``firmware.elf`` which can be used to obtain
      a firmware binary in `ELF format <ELF_>`_.
    - When running the :ref:`firmware builder subsystem <kotori:firmware-builder>` on your workstation,
      you might want to use ``Makefile-OSX.mk`` as designated Makefile.



*************
Upload to MCU
*************

.. todo::

    What's the best way to upload the downloaded firmware hex file
    to the MCU, when not having any toolchain installed, actually?


Windows
=======
- http://m8051.blogspot.de/2015/01/avrdude-on-windows-long-time-after.html
- http://arduinodev.com/arduino-uploader/
- http://www.hobbytronics.co.uk/arduino-xloader
- http://blog.zakkemble.co.uk/avrdudess-a-gui-for-avrdude/

Mac OS X
========
- https://www.pololu.com/docs/0J36/5.c
- https://www.obdev.at/products/crosspack/



********
Appendix
********

Arduino 1.0.x vs. 1.6.x

1.0.x::

    BOARD_TAG         = pro328
    BOARD_SUB         = atmega328p

1.6.x::

    BOARD_TAG         = pro
    BOARD_SUB         = 8MHzatmega328

On the server, there's currently Arduino 1.0.5::

    root@elbanco:~# aptitude show arduino-core | grep Version
    Version: 2:1.0.5+dfsg2-4

So the appropriate settings for Arduino 1.0.x should be used inside the respective Makefile.

