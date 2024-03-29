.. include:: ../resources.rst

.. _lib-terkin-data:

##############
TerkinData C++
##############

.. tip::

    Please `continue reading this document on our documentation space
    <https://hiveeyes.org/docs/arduino/TerkinData/README.html>`_,
    all inline links will be working there.

.. highlight:: cpp


*****
About
*****

The TerkinData C++ library offers a generic interface for collecting sensor
readings, and for marshalling them into different output formats, like CSV,
JSON, SQL, or x-www-form-urlencoded.

It helps to decouple the sensor reading domain from the telemetry domain in a
typical data logger application.

The `TerkinTelemetry C++`_ library provides adapters for different telemetry backends
in form of transmitter components, and uses `TerkinData C++`_ for marshalling.


********
Synopsis
********
::

    // Data container for collecting sensor data.
    TerkinData::Measurement measurement;

    // Optionally set timestamp (any format).
    measurement.time = "2017-01-11T14:48:56Z";

    // Collect sensor readings.
    measurement.data["dht.0.temp"] = 42.42f;
    measurement.data["dht.0.hum"]  = 84.84f;


*******
Details
*******

The library addresses a few important obstacles usually encountered when doing
telemetry.

CSV marshalling
===============

- When serializing to CSV, the order of fields is important.
  Therefore, an ordered list of field names is required for proper operation.
- When serializing a measurement reading into CSV, and some sensor values are missing,
  possibly due to sensor defects, or other errors, this **must not** have a negative
  impact on the outbound CSV record. Optionally, the CSV header line can be prefixed
  with an arbitrary string on serialization.

Multiple values per sensor reading
==================================

Some sensors, like the `DHT33`_/`RHT04`_ digital humidity/temperature sensor,
deliver **two values** in a single reading cycle: Humidity and temperature.

The data channeling from reading the sensor to telemetry submission has to
account for that. As with the `DS18B20`_ digital thermometer or other `1-Wire`_
devices, multiple sensors of the same type might be attached, the data
channeling also has to account for that.

This is achieved by one level of indirection between the sensor reading and
the telemetry domain by introducing a convenient mapping between low-level
sensor values and the designated high-level telemetry field names, which is
resolved and applied on serialization.

- A timestamp string in arbitrary format can be attached to a single
  measurement.

The software is currently alpha quality as we didn't hunt down potential memory
leaks yet. It has also not been run on embedded MCU hardware yet, as this is
just a design draft by now. Helping hands on that are very welcome!


************
Environments
************

TerkinData C++ has been compiled successfully for/with:

- AVR ATmega328: avr-g++ 7.3.0
- ARM: xPack GNU Arm Embedded GCC 9.3.1
- ESP8266: xtensa-lx106-elf-gcc 4.8.2
- ESP32: xtensa-esp32-elf-gcc 8.4.0
- macOS: Apple clang version 12.0.0

On AVR, the framework relies on the `STL`_ implementation `ArduinoSTL`_, because
it uses the ``map`` and ``vector`` data containers and its corresponding iterators.

The example programs `<csv_basic.cpp_>`_, `<json_basic.cpp_>`_, and
`<urlencoded_basic.cpp_>`_ have been tested successfully at runtime on x86_64.


*****
Usage
*****

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

::

    // Serialize data into JSON format
    std::string data_record = datamgr->json_data(*measurement);

    // Use as character array:
    // - data_record.c_str()

    // Free memory
    delete measurement;


SQL
---

::

    // Define table name.
    std::string table_name = "testdrive";

    // Serialize data into SQL CREATE statement.
    datamgr->sql_create(table_name, *measurement);

    // Serialize data into SQL INSERT statement.
    datamgr->sql_insert(table_name, *measurement);


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
    cd arduino/libraries/TerkinData/examples

    # Build examples for all designated platforms.
    make

Build individual programs::

    PLATFORMIO_SRC_DIR=csv pio run --environment=avr328

Build and run on POSIX
======================
Rebuild and run all examples::

    make run

Run individual example::

    make csv
    make json
    make urlencoded


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


.. _terkindata-sql-example:

SQL
===

::

    ==============================
    TerkinData SQL DDL+DML example
    ==============================

    -- Test DDL
    data:   CREATE TABLE testdrive ('time' DATETIME WITH TIMEZONE,'weight' FLOAT,'temperature_outside' FLOAT,'humidity_outside' FLOAT,'temperature_inside' FLOAT,'voltage' FLOAT);

    -- Test DML single reading (complete)
    data:   INSERT INTO testdrive (time,weight,temperature_outside,humidity_outside,temperature_inside,voltage) VALUES ('2023-06-03T13:14:24Z',85.000000,42.419998,84.839996,33.330002,3.843000);

    -- Test DML single reading (incomplete)
    data:   INSERT INTO testdrive (time,temperature_outside,humidity_outside,voltage) VALUES ('2023-06-03T13:14:24Z',42.419998,84.839996,3.843000);

.. seealso:: Full source of `<sql_basic.cpp_>`_.


********
Download
********

| Please download a recent version from GitHub:
| https://github.com/hiveeyes/arduino/tree/main/libraries/TerkinData


************
Dependencies
************

- `ArduinoSTL`_ by Mike Matera.
- `ArduinoJSON`_ by Benoît Blanchon.


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
.. _csv_basic.cpp: https://github.com/hiveeyes/arduino/blob/main/libraries/TerkinData/examples/csv/csv_basic.cpp
.. _json_basic.cpp: https://github.com/hiveeyes/arduino/blob/main/libraries/TerkinData/examples/json/json_basic.cpp
.. _urlencoded_basic.cpp: https://github.com/hiveeyes/arduino/blob/main/libraries/TerkinData/examples/urlencoded/urlencoded_basic.cpp
.. _sql_basic.cpp: https://github.com/hiveeyes/arduino/blob/main/libraries/TerkinData/examples/urlencoded/sql_basic.cpp
