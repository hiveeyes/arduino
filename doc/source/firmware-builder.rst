.. include:: resources.rst

.. _firmware-builder:

################
Firmware builder
################


*****
Intro
*****
We have a flexible and powerful build system.
This infrastructure pulls code sketches including dependencies from the
`Hiveeyes Arduino repository`_, replaces user-defined variables in the main
sketch as well as the ``Makefile`` and delivers customized firmwares based
on an universal code base.

The main use case for this is to enable everyone to build her own firmwares
without installing any toolchain at all. The most prominent example for customizing
user-defined variables would be the Hiveeyes WAN device address in form of the triple
(``HE_USER``, ``HE_SITE``, ``HE_HIVE``) or the access point configuration parameters
for setting up a GPRS device for communication in form of the quadruple
(``GPRSBEE_AP_NAME``, ``GPRSBEE_AP_AUTH``, ``GPRSBEE_AP_USER`` and ``GPRSBEE_AP_PASS``).

For getting an idea about the variable replacements with an example sketch suitable for
automatic building using the variables described above, please have a look at
`node-gprs-any.ino, line 81 ff. <https://github.com/hiveeyes/arduino/blob/master/node-gprs-any/node-gprs-any.ino#L81#>`_


*****
Usage
*****
.. highlight:: bash

Just send a HTTP POST request to the ``..../firmware.hex``
endpoint using the HTTP client of your choice. Pass the
user-defined variables in JSON or x-www-form-urlencoded
format in the request body.

Setup HTTP client::

    # Debian-based systems
    aptitude install httpie

    # Mac OSX
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

Given, the filename is huge, but it includes every parameter
to distinguish different build artifacts from each other.

.. tip::

    - There's also an endpoint with suffix ``firmware.elf`` which can be used to obtain
      a firmware binary in `ELF format <ELF_>`_.
    - When running Kotori on your workstation, you might want to use ``Makefile-OSX.mk``
      as designated Makefile.

.. todo::

    How to program the firmware (e.g. using avrdude)?

    - Windows: http://m8051.blogspot.de/2015/01/avrdude-on-windows-long-time-after.html
    - Mac OS X: https://www.pololu.com/docs/0J36/5.c, https://www.obdev.at/products/crosspack/


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

So the appropriate settings for Arduino 1.0.x show be used.

