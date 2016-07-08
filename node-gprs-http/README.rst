.. include:: ../../resources.rst

.. _open-hive-firmware:

##########################
Firmware for Open Hive Box
##########################

.. tip::

    You might want to `read this document on our documentation space <docs-open-hive-firmware_>`_,
    all inline links will be working there.

.. _docs-open-hive-firmware: https://hiveeyes.org/docs/arduino/firmware/node-gprs-http/README.html


************
Introduction
************
This firmware runs on the `Open Hive Box`_, a beehive
monitoring sensor node with GPRS transceiver.
Telemetry data is transmitted using HTTP.

.. figure:: https://ptrace.hiveeyes.org/2016-07-08_open-hive_box-with-electronics.jpg
    :alt: Open Hive Box with electronics
    :width: 640px


*****
Build
*****
.. highlight:: bash


Build on your workstation
=========================
::

    # Get hold of the source code repository including all dependencies
    git clone --recursive https://github.com/hiveeyes/arduino

    # Select this firmware
    cd node-gprs-http

    # Select appropriate Makefile
    ln -s Makefile-Linux.mk Makefile

    # Build firmware
    make

    # Upload to MCU
    make upload

.. note:: You might want to adjust the appropriate Makefile to match your environment.


Build using the firmware builder
================================
You may build your flavors of the firmware without having any toolchain installed
on your workstation by issuing a HTTP POST request to the :ref:`firmware-builder`
subsystem.

The source code of `node-gprs-http.ino`_ is pulled from the master branch of the
`Hiveeyes Arduino repository`_ on GitHub. Specify appropriate parameters to match
your environment.

.. todo::

    How to configure the sensor node address (here ``testdrive/area-42/node-1``)
    and other firmware parameters?


HTTPie
------
Issue a POST request from the command line using HTTPie_::

    time http --timeout=120 POST https://swarm.hiveeyes.org/api/hiveeyes/testdrive/area-42/node-1/firmware.hex \
        ref=master path=node-gprs-http makefile=Makefile-FWB.mk \
        GPRSBEE_AP_NAME=internet.eplus.de GPRSBEE_AP_USER=barney@blau.de GPRSBEE_AP_PASS=789 \
        --download

    Downloading to "hiveeyes_node-gprs-http_pro328-atmega328p_af495adf-GPRSBEE_AP_NAME=internet.eplus.de,GPRSBEE_AP_PASS=789,HE_SITE=area-42,GPRSBEE_AP_USER=barney@blau.de,HE_USER=testdrive,HE_HIVE=node-1.hex"
    Done. 72.18 kB in 0.17865s (404.03 kB/s)

    real	0m19.154s

Just upload the file ``hiveeyes_node-gprs-http_pro328-atmega328p_*.hex`` to the MCU, YMMV.


HttpRequester
-------------
Issue a POST request from the command line using the HttpRequester_ add-on for Firefox
for acquiring a firmware from the :ref:`firmware-builder` (`Kotori Firmware Builder`_).
Please set the appropriate firmware build- and configuration parameters on the left side.

.. figure:: https://ptrace.hiveeyes.org/2016-07-07_Kotori%20Firmware%20Builder%20HttpRequester.jpg
    :alt: Kotori Firmware Builder HttpRequester
    :width: 1024px

After sending the POST request by hitting "Submit", just copy/paste the
response content on the right side into a .hex file and upload to the MCU.

.. tip::

    As the available configuration parameters and possible values are currently not documented
    in detail, please have a look at the source code (`node-gprs-http.ino`_). All preprocessor
    variables (``#define ...``) can be changed through HTTP POST parameters.


Upload to MCU
-------------

.. todo::

    What's the best way to upload the downloaded firmware hex file
    to the MCU, when not having any toolchain installed, actually?

