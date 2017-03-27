.. include:: resources.rst

.. _README:

######
README
######

This is the low power battery version of :ref:`node-wifi-mqtt-homie`
To run this sketch e.g. on the PCB Design from https://www.imker-nettetal.de/bienen-nsa/ESP8266-BeeScale1_1.fzz
you should remove and shortout the input polarity protection diode to
avoid unnecessary voltage drop.
Also you should remove any unnecessary power indicator LED to save
a couple om mA. (e.g. on the ESP-07 the red power indicator LED). This will increase
battery lifetime a lot.



