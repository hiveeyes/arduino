.. include:: ../../resources.rst

.. _node-wifi-mqtt-homie-battery:

####################################################
Battery-powered ESP8266 beehive scale based on Homie
####################################################

.. contents::
   :local:
   :depth: 1

----

This is the low power battery version of :ref:`node-wifi-mqtt-homie`.
The most recent version is available at `node-wifi-mqtt-homie-battery.ino`_.
To run this sketch e.g. on the PCB Design from `ESP8266-BeeScale.fzz`_,
you should remove and shortout the input polarity protection diode to
avoid unnecessary voltage drop.
Also you should remove any unnecessary power indicator LED to save
a couple of mA. (e.g. on the ESP-07 the red power indicator LED). This will increase
battery lifetime a lot.


.. Todo::

    - Describe how to determine scale's temperature compensation values.


.. include:: CHANGES.rst

