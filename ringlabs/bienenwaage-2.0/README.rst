.. include:: ../../../resources.rst

.. _node-esp8266-generic:
.. _bienenwaage-2.0:

#######################
Bienenwaage 2.0 NodeMCU
#######################

.. tip::

    Please `continue reading this document on our documentation space
    <https://hiveeyes.org/docs/arduino/firmware/ringlabs/bienenwaage-2.0/README.html>`_,
    all inline links will be working there.


.. highlight:: bash

*****
About
*****
A beehive monitor firmware for ESP8266.

:Author: Stefan Ulmer <ulmi [ät] gmx.de>

Details
=======

A beehive monitoring sensor node based on the NodeMCU_ hardware, featuring an ESP8266_ MCU.
Telemetry data is transmitted using MQTT over WiFi or GSM.
The most recent firmware version is available at `bienenwaage-esp8266.ino`_.


********
Features
********
- Equipped for NodeMCU_, but suitable to run on any ESP8266_ device
- Sensors: 2x Temperature via DS18B20_, 2x Weight via HX711_, Battery level via ADC
- GSM/GPRS via SIM800/TinyGSM
- WiFi via ESP8266
- Telemetry via MQTT


*************
Documentation
*************
- https://community.hiveeyes.org/t/inbetriebnahme-und-weiterentwicklung-der-firmware-fur-arduino-node-esp8266-generic-node-esp32-generic/3189


*************
Configuration
*************

General
=======
In order to toggle different features and modes, please have a look
at the list of ``#define`` constants at the top of the sketch.

Weight scale
============
Adjust ``SCALE_DOUT_PIN_{A,B}``, ``SCALE_SCK_PIN_{A,B}``, ``Taragewicht_{A,B}`` and ``Skalierung_{A,B}``.
To find out about appropriate values for ``Taragewicht_X`` and ``Skalierung_X``,
please use a weight scale adjustment sketch like
https://github.com/hiveeyes/arduino/tree/main/openhive/scale-adjust/HX711.

::

    // Scale settings
    #define WEIGHT                  true
    float Taragewicht_A = 226743;
    float Skalierung_A = 41.647;
    float Taragewicht_B = -238537
    float Skalierung_B = -42.614;

Temperature sensors
===================
Adjust ``SENSOR_DS18B20``, ``DS18B20_PIN`` and the device addresses ``SensorX``.

More sensors
============
Adjust ``SENSOR_BATTERY_LEVEL``.

WiFi
====
To configure WiFi SSID and credentials, please use the captive portal
available on X.X.X.X when the device starts in AP mode.

GSM
===
To configure GSM/GPRS connectivity, please adjust the ``GSM_ENABLED``,
``apn[]``, ``user[]``, ``pass[]`` and ``simPIN[]`` variables.

MQTT
====
To configure MQTT telemetry, adjust the ``MQTT_BROKER``, ``MQTT_PORT``,
``MQTT_USERNAME``, ``MQTT_PASSWORD`` and ``MQTT_TOPIC`` variables.

Weather Underground
===================
To enable Weather Underground for getting nearby temperature and humidity
information, adjust ``WUG_API_KEY`` and ``WUG_STATION_ID``.


******************
Build instructions
******************

Clone git repository
====================
::

    # Acquire source code repository.
    git clone https://github.com/hiveeyes/arduino

    # Select this firmware.
    cd arduino/ringlabs/bienenwaage-2.0


Build
=====

The build system is based on PlatformIO_.

Build firmware::

    make

After successfully building it, you will find firmware images at

- .pio/build/nodemcu/firmware.bin
- .pio/build/nodemcu/firmware.elf

Upload to MCU
=============
::

    export MCU_PORT=/dev/ttyUSB0
    make upload
