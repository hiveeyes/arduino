
.. _open-hive-firmware:

########################################
Firmware for Open Hive Seeeduino Stalker
########################################

*****
Intro
*****
This firmware runs on the `Open Hive Seeeduino Stalker`_,
a beehive monitoring sensor node with GPRS transmitter.
Telemetry data is transmitted using HTTP.


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
on your workstation by issuing a HTTP POST request to the firmware builder subsystem.

The source code of `node-gprs-http.ino`_ is pulled from the master branch of the
`Hiveeyes Arduino repository`_ on GitHub. Specify appropriate parameters to match
your environment.

Issue a POST request from the command line using HTTPie_::

    time http --timeout=120 POST https://swarm.hiveeyes.org/api/hiveeyes/testdrive/area-42/node-1/firmware.hex \
        ref=master path=node-gprs-http makefile=Makefile-FWB.mk \
        GPRSBEE_AP_NAME=internet.eplus.de GPRSBEE_AP_USER=barney@blau.de GPRSBEE_AP_PASS=789 \
        --download

    Downloading to "hiveeyes_node-gprs-http_pro328-atmega328p_af495adf-GPRSBEE_AP_NAME=internet.eplus.de,GPRSBEE_AP_PASS=789,HE_SITE=area-42,GPRSBEE_AP_USER=barney@blau.de,HE_USER=testdrive,HE_HIVE=node-1.hex"
    Done. 72.18 kB in 0.17865s (404.03 kB/s)

Just upload the file ``hiveeyes_node-gprs-http_pro328-atmega328p_*.hex`` to the MCU, YMMV.



.. External resources

.. _Open Hive Seeeduino Stalker: https://hiveeyes.org/docs/system/vendor/open-hive/#open-hive-seeeduino-stalker
.. _node-gprs-http.ino: https://github.com/hiveeyes/arduino/blob/master/node-gprs-http/node-gprs-http.ino
.. _Hiveeyes Arduino repository: https://github.com/hiveeyes/arduino
.. _HTTPie: http://httpie.org

