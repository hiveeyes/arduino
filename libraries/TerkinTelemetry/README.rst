.. include:: ../resources.rst

.. _lib-terkin-telemetry:

###################
TerkinTelemetry C++
###################

.. tip::

    Please `continue reading this document on our documentation space
    <https://hiveeyes.org/docs/arduino/TerkinTelemetry/README.html>`_,
    all inline links will be working there.

.. highlight:: cpp


*****
About
*****

TerkinTelemetry C++ provides an universal interface to user-space Arduino code,
to communicate with a range of different telemetry backends.

TerkinTelemetry C++ provides backend drivers for supporting different telemetry
communication paths, similar to what RadioHead provides as adapters to different
radio link hardware, and TinyGSM provides as adapters to different modems.

It is being conceived to provide pluggable telemetry components to firmware programs
for scientific and industrial applications, and to support firmware programmers on
corresponding tasks.

For collecting telemetry data, and for marshalling it into different output formats,
the library uses `TerkinData C++`_.

The software is currently alpha quality. Here be dragons.


********
Synopsis
********

Transmit sensor measurement readings from Arduino and libc environments, for humans.
::

    telemetry->transmit(measurement);

.. note::

    The idea to aim for such a slim interface is derived from the telemetry subsystem of
    the `Terkin MicroPython Datalogger`_.


*******
Details
*******

There are three classes you will use to configure a telemetry client instance, each with
a specific subset of connectivity parameters. This design aims to make the library as
modular as intended, without overcomplicating it.

- ``ChannelAddress``: You will use it to define the data acquisition channel your device will
  submit data to. It reflects the venerable :ref:`sensorwan` channel addressing scheme, that is
  used for assigning communication channels to individual sensor node devices in a wide-area
  sensor network scenario.

- ``XyzTransmitter``: You can select one of the available transmitter components, in order to
  define which kind of outbound transport you are intending to use. There are different kinds
  of transmitter components included into TerkinTelemetry, but it is possible to bring your own.

- ``TelemetryClient``: The telemetry client instance your code will interact with, in order to
  actually submit measurement/metric values, after reading your sensors.

.. note::

    We are currently exploring this abstraction level to see if it will turn out well, by
    adding a few more specific outbound transmitter adapter components. More details are
    available on the discussion forum at `Telemetriesubsystem für Arduino Firmwares
    generalisieren`_.


*****
Usage
*****

Setup
=====
::

    // The address of the data acquisition channel.
    ChannelAddress* address = new ChannelAddress(CHANNEL_REALM, CHANNEL_OWNER, CHANNEL_SITE, CHANNEL_DEVICE);

    // The actual transmitter element.
    JsonHttpTransmitter* transmitter = new JsonHttpTransmitter(KOTORI_URL);

    // The telemetry client.
    telemetry = new TelemetryClient(transmitter, address);

Collect and transmit
====================
::

    // Collect sensor data.
    TerkinData::Measurement measurement;
    measurement.data["temperature"] = 42.42;
    measurement.data["humidity"] = 80;

    // Transmit measurement to telemetry data collection server.
    telemetry->transmit(measurement);


*****
Usage
*****

.. highlight:: bash

Build for embedded targets
==========================
::

    # Acquire source code repository.
    git clone https://github.com/hiveeyes/arduino

    # Select firmware.
    cd arduino/libraries/TerkinTelemetry/examples

    # Build examples for all designated platforms.
    make

Build individual programs::

    PLATFORMIO_SRC_DIR=http-cratedb pio run --environment=esp32

Build and run on POSIX
======================
Build and run all examples on your workstation::

    make run

Run individual examples::

    make http-cratedb
    make http-kotori



********
Download
********

| Please download a recent version from GitHub:
| https://github.com/hiveeyes/arduino/tree/main/libraries/TerkinTelemetry


************
Dependencies
************

- `ArduinoSTL`_ by Mike Matera.
- `ArduinoJSON`_ by Benoît Blanchon.
- `TerkinData`_ and `Terrine`_.


*********
Etymology
*********

Terkin
======
| Epitomizes the greatest, the most immense, the supreme and the paramount one.
| Basically, the fundamental individual.
|
| -- https://www.urbandictionary.com/define.php?term=Terkin


.. _ArduinoJSON: https://github.com/bblanchon/ArduinoJson
.. _ArduinoSTL: https://github.com/mike-matera/ArduinoSTL
.. _Telemetriesubsystem für Arduino Firmwares generalisieren: https://community.hiveeyes.org/t/telemetriesubsystem-fur-arduino-firmwares-generalisieren/181/14
