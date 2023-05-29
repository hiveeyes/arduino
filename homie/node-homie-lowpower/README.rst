.. include:: ../../../resources.rst

.. _node-wifi-mqtt-homie-battery:
.. _node-homie-lowpower:

####################################################
Battery-powered ESP8266 beehive scale based on Homie
####################################################

.. tip::

    Please `continue reading this document on our documentation space
    <https://hiveeyes.org/docs/arduino/firmware/homie/node-homie-lowpower/README.html>`_,
    all inline links will be working there.


*****
About
*****

This is the low-power version of :ref:`node-wifi-mqtt-homie`, in order to run
the device on batteries.

The most recent version is available at `node-homie-lowpower.ino`_.
To run this sketch e.g. on the PCB Design from `ESP8266-BeeScale.fzz`_, you should
remove and short-out the input polarity protection diode to avoid unnecessary voltage
drop.

Also you should remove any unnecessary power indicator LED to save a couple of mA.
For example, on the ESP-07 the red power indicator LED. This will increase battery
life-time a lot.

.. todo::

    - Describe how to determine the scale's temperature compensation values.


*********
Changelog
*********

See :doc:`changelog`.
