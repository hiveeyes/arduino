.. include:: ../resources.rst

.. _terkindata:

.. _lib-terkin-data:

##############
TerkinData C++
##############

.. contents::
   :local:
   :depth: 1

----

.. tip::

    You might want to `read this document on our documentation space
    <https://hiveeyes.org/docs/arduino/TerkinData/README.html>`_,
    all inline links will be working there.


************
Introduction
************

TerkinData C++ is a convenient library for handling sensor readings.

It helps to decouple the sensor reading domain from the telemetry domain in a
typical data logger application. While providing a generic interface, it can
serialize measurement values to CSV, x-www-form-urlencoded and JSON formats.

The library solves some important obstacles usually encountered when doing
telemetry:

- When serializing to CSV, the order of fields is important.
  Therefore, an ordered list of field names is required for proper operation.
  When serializing a CSV data line and some sensor values are missing due to
  sensor defects or other errors, this must not have an impact on the CSV data
  line. Optionally, the CSV header line can be prefixed with an arbitrary
  string on serialization.
- Some sensors like the DHT33_ (RHT04_) digital humidity/temperature sensor
  deliver two values in a single reading cycle: Humidity and temperature.
  The data channeling from reading the sensor to telemetry submission has to
  account for that. As with the DS18B20_ digital thermometer or other `1-Wire`_
  devices, multiple sensors of the same type might be attached, the data
  channeling also has to account for that.
  This is achieved by one level of indirection between the sensor reading and
  the telemetry domain by introducing a convenient mapping between lowlevel
  sensor values and the designated highlevel telemetry field names, which is
  resolved and applied on serialization.
- A timestamp string in arbitrary format can be attached to a single
  measurement.

The software is currently alpha quality as we didn't hunt down potential memory
leaks yet. It has also not been run on embedded MCU hardware yet, as this is
just a design draft by now. Helping hands on that are very welcome!


***********
Environment
***********

TerkinData C++ has been compiled successfully for/with:

- AVR ATmega328: avr-g++ 7.3.0
- ARM: xPack GNU Arm Embedded GCC 9.3.1
- ESP8266: xtensa-lx106-elf-gcc 4.8.2
- ESP32: xtensa-esp32-elf-gcc 8.4.0
- macOS: Apple clang version 12.0.0

On AVR, the framework relies on the STL_ implementation ArduinoSTL_ as it uses
the ``map`` and ``vector`` data containers and its corresponding iterators
under the hood.

The example programs `<csv_basic.cpp_>`_, `<json_basic.cpp_>`_,
`<urlencoded_basic.cpp_>`_ and have been tested successfully at runtime on
x86_64 (macOS).


********
Synopsis
********

.. highlight:: cpp

Setup
=====
::

    #include <TerkinData.h>
    using namespace TerkinData;
    using namespace TerkinUtil;

    void DataManager::setup() {

        // List of field names
        this->field_names  = new DataHeader({"time", "temperature", "humidity", "rssi", "voltage"});

        // Map names of lowlevel sensor values to highlevel telemetry data fields
        (*this->sensor_field_mapping)[string("dht.0.temp")]   = string("temperature");
        (*this->sensor_field_mapping)[string("dht.0.hum")]    = string("humidity");

        // Optionally prefix CSV header line with string
        this->csv_header_prefix = "## ";

        // Optionally set float serialization precision
        this->float_precision = 3;

    }

    DataManager *datamgr = new DataManager();

Collect
=======
::

    // Data container for one reading cycle
    Measurement *measurement = new Measurement();

    // Set timestamp (any format)
    measurement.time = "2017-01-11T14:48:56Z";

    // Collect sensor readings
    measurement->data["dht.0.temp"]  = 42.42f;
    measurement->data["dht.0.hum"]   = 84.84f;
    measurement->data["rssi"]        = 72;
    measurement->data["voltage"]     = 3.843f;


Serialize
=========

CSV
---
::

    // Serialize data into CSV format
    std::string data_header = datamgr->csv_header();
    std::string data_record = datamgr->csv_data(*measurement);

    // Use as character array:
    // - data_header.c_str()
    // - data_record.c_str()

    // Free memory
    delete measurement;


x-www-form-urlencoded
---------------------
::

    // Serialize data into x-www-form-urlencoded format
    std::string data_record = datamgr->urlencode_data(*measurement);

    // Use as character array:
    // - data_record.c_str()

    // Free memory
    delete measurement;


JSON
----
.. note:: To use the fine ArduinoJson_ library for JSON serialization, please compile using ``-DHAVE_ARDUINO_JSON``.

::

    // Serialize data into JSON format
    std::string data_record = datamgr->json_data(*measurement);

    // Use as character array:
    // - data_record.c_str()

    // Free memory
    delete measurement;



*****
Usage
*****

.. highlight:: bash

::

    # Get source code.
    git clone --recursive https://github.com/hiveeyes/arduino
    cd arduino/libraries/TerkinData/examples

    # Build examples for all embedded platforms.
    make

    # Build and run examples on host platform.
    make run


********
Examples
********

.. _terkindata-csv-example:

CSV
===
::

    ======================
    TerkinData CSV example
    ======================

    -- Test single reading (complete)
    header: ## time,weight,temperature-outside,humidity-outside,temperature-inside,voltage
    data:   2017-03-17T02:48:15Z,85.000,42.420,84.840,33.330,3.843

    -- Test single reading (incomplete: "weight" and "temperature-inside" missing)
    header: ## time,weight,temperature-outside,humidity-outside,temperature-inside,voltage
    data:   2017-03-17T02:48:15Z,,42.420,84.840,,3.843


.. seealso:: Full source of `<csv_basic.cpp_>`_.


.. _terkindata-urlencoded-example:

x-www-form-urlencoded
=====================

::

    ========================================
    TerkinData x-www-form-urlencoded example
    ========================================

    -- Test single reading (complete)
    data:   time=2017%2D03%2D17T03%3A03%3A57Z&weight=85%2E00&temperature-outside=42%2E42&humidity-outside=84%2E84&temperature-inside=33%2E33&voltage=3%2E84

    -- Test single reading (incomplete: "weight" and "temperature-inside" missing)
    data:   time=2017%2D03%2D17T03%3A03%3A57Z&temperature-outside=42%2E42&humidity-outside=84%2E84&voltage=3%2E84

.. seealso:: Full source of `<urlencoded_basic.cpp_>`_.


.. _terkindata-json-example:

JSON
====

::

    =======================
    TerkinData JSON example
    =======================

    -- Test single reading (complete)
    data:   {"time":"2017-03-17T03:07:25Z","weight":85.00,"temperature-outside":42.42,"humidity-outside":84.84,"temperature-inside":33.33,"voltage":3.84}

    -- Test single reading (incomplete)
    data:   {"time":"2017-03-17T03:07:25Z","temperature-outside":42.42,"humidity-outside":84.84,"voltage":3.84}

.. seealso:: Full source of `<json_basic.cpp_>`_.


.. _csv_basic.cpp: https://github.com/hiveeyes/arduino/blob/master/libraries/TerkinData/examples/csv_basic.cpp
.. _urlencoded_basic.cpp: https://github.com/hiveeyes/arduino/blob/master/libraries/TerkinData/examples/urlencoded_basic.cpp
.. _json_basic.cpp: https://github.com/hiveeyes/arduino/blob/master/libraries/TerkinData/examples/json_basic.cpp


********
Download
********

TerkinData C++
==============
| Please download a recent version from GitHub:
| https://github.com/hiveeyes/arduino/tree/master/libraries/TerkinData
|


************
Dependencies
************

ArduinoSTL
==========
| ArduinoSTL by Mike Matera
| https://github.com/mike-matera/ArduinoSTL
|

ArduinoJson
===========
| ArduinoJson by Benoît Blanchon
| https://github.com/bblanchon/ArduinoJson
|
| This library is optional, but obviously required when serializing to JSON.
|


*******
Backlog
*******

.. todo::

    - Add :ref:`BERadio` serialization format
    - Move to repository https://github.com/daq-tools/terkin

|


*********
Etymology
*********

Terkin
======
| Epitomizes the greatest, the most immenense, the supreme and the paramount one.
| Basically, the fundamental individual.
|
| -- http://www.urbandictionary.com/define.php?term=Terkin
|
