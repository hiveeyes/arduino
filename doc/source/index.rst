.. include:: resources.rst

.. Hiveeyes Arduino documentation master file, created by
   sphinx-quickstart on Sat Apr 16 23:19:45 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

.. _hiveeyes-arduino:

##############################
Hiveeyes Arduino documentation
##############################

This is the firmware source code documentation of the `Hiveeyes project`_,
a beehive monitoring system made of open source software and hardware components.
For an overview about the whole system, please have a look at :ref:`hiveeyes-system`.
Enjoy reading and exploring this space.

.. toctree::
    :maxdepth: 1
    :hidden:

    README

    firmwares
    development

    CONTRIBUTORS
    Changelog <CHANGES>
    LICENSE

    BERadio/README
    TerkinData/README
    firmware-builder


************
Introduction
************
Please have a look at the :ref:`README` about how to setup this thing.
It also describes the structure of the `Hiveeyes Arduino repository`_,
which supports different kinds of firmwares matching different environments.


*********
Firmwares
*********
There are different firmwares for different flavours of sensor nodes.

- :ref:`firmware-overview`

Stable:

- :ref:`WiFi sensor node <node-wifi-mqtt>`
- :ref:`WiFi sensor node based on Homie <node-wifi-mqtt-homie>`
- :ref:`GPRS sensor node <open-hive-firmware-gprs>`
- :ref:`Radio sensor and gateway nodes <firmwares-beradio>`
- :ref:`scale-adjust-firmware`

In development:

- :ref:`generic-firmware`
- :ref:`open-hive-firmware-rfm69-node`
- :ref:`open-hive-firmware-rfm69-gateway-sdcard`
- :ref:`open-hive-firmware-rfm69-gateway-uart`


.. Indices and tables
.. ==================

.. * :ref:`genindex`
.. * :ref:`modindex`
.. * :ref:`search`

