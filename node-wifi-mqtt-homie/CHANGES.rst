.. include:: resources.rst

.. _changes:

#######
CHANGES
#######


in-progress
===========
- Set some custom settings as default, limited at the moment by issue:
 https://github.com/marvinroger/homie-esp8266/issues/323


2017-03-28 0.9.2
=================
Align with feature set of ../node-wifi-mqtt-homie-battery/node-wifi-mqtt-homie-battery.ino
- Disable 3 of 4 default settings due to bug in homie-esp8266
- All needed settings are now settable via HomieSetting
- Modularized code
- Add versioning scheme
- Add JSON output for hiveeyes.org compatibility
