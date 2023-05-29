.. include:: ../../../resources.rst

.. _open-hive-firmware:
.. _open-hive-firmware-gprs:
.. _node-gsm-http:
.. _node-wifi-http:
.. _openhive-gsm-wifi:

###################################
Open Hive GSM and WiFi sensor nodes
###################################
.. highlight:: bash

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/node-gprs-http/README.html>`_,
    all inline links will be working there.


************
Introduction
************
This hybrid firmware supports two different hardware devices.
Telemetry data is transmitted using HTTP. Both HX711_ and ADS1231_ load cell sensor chips are supported.
The most recent firmware version is available at `openhive-gsm-wifi.ino`_.

.. figure:: https://ptrace.hiveeyes.org/2016-07-08_open-hive_box-with-electronics.jpg
    :alt: Open Hive Box with electronics
    :width: 450px
    :align: left

    `Open Hive Box`_ GSM


.. figure:: https://ptrace.hiveeyes.org/2016-06-17_openhive-huzzah.jpg
    :alt: Open Hive WiFi Node on workbench
    :width: 450px
    :align: right

    Open Hive WiFi Node


|clearfix|


Platform and supported peripherals
==================================

Board
-----
- `Open Hive Box`_ GSM: `Seeeduino Stalker v2.3`_ with ATmega328_ MCU and GPRSbee_ GSM modem or
- Open Hive WiFi Node: `Adafruit Feather HUZZAH`_ with ESP8266_ MCU (:ref:`Fritzing wiring <esp8266-fritzing-wiring>`)

Sensors
-------
- HX711_ ADC weigh scale breakout board or
- ADS1231_ ADC weigh scale breakout board
- DS18B20_ digital thermometer
- DHT33_ (RHT04_) digital humidity/temperature sensor

.. seealso::

    - Nodes with identical hardware: :ref:`node-wifi-mqtt` and :ref:`node-wifi-mqtt-homie`


*****
Setup
*****

Clone git repository
====================
::

    # Acquire source code repository
    git clone https://github.com/hiveeyes/arduino

    # Select this firmware
    cd arduino/openhive/openhive-gsm-wifi


Configure firmware
==================
.. highlight:: c++

Have a look at the source code `openhive-gsm-wifi.ino`_ and adapt
feature flags and setting variables according to your environment:

When using a HX711_ sensor::

    #define isScaleHX711

When using an ADS1231_ sensor::

    #define isScaleADS1231

Enable AVR ATmega328 with GPRSbee_ GSM modem::

    #define isGSM

    #define APN "internet.eplus.de"

Enable ESP8266 with integrated WiFi::

    #define isWifi

    #define WLAN_SSID  "your-ssid"
    #define WLAN_PW    "your-pw"

Configure load cell calibration settings::

    // Use sketches "scale-adjust-hx711.ino" or "scale-adjust-ads1231.ino" for calibration

    // The raw sensor value for "0 kg"
    const long loadCellZeroOffset = 38623;

    // The raw sensor value for a 1 kg weight load
    const long loadCellKgDivider  = 11026;

.. tip::

    Read about :ref:`scale-adjust-firmware` to get these values.


******************
Build instructions
******************

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
