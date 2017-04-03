.. _node-wifi-mqtt-homie-battery-changes:

#######
CHANGES
#######


in-progress
===========
- Set some custom settings as default, limited at the moment by issue:
 https://github.com/marvinroger/homie-esp8266/issues/323

2017-03-27 0.9.3
=================
- Add connection error handling to avoid battery drain (WIFI/MQTT Connection issues) Thanks to a broken WIFI and  @amotl.

2017-03-27 0.9.2
=================
- Disable 3 of 4 default settings due to bug in homie-esp8266

2017-03-24 0.9.1
=================
- Make use of onHomieEvent to ensure a clean disconnect from broker before deepSleep
- Implement a battery check, create a low battery alarm topic to notify user and make the ESP sleep "forever"
- All needed settings are now settable via HomieSetting
- Modularized code
- Add versioning scheme

2017-03-18 0.9.0
=================
- Add JSON output for hiveeyes.org compatibility

