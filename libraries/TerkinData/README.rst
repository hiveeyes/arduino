.. include:: ../resources.rst

.. _lib-terkin-data:

##############
TerkinData C++
##############

.. contents::
   :local:
   :depth: 1

----

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/TerkinData/README.html>`_,
    all inline links will be working there.


************
Introduction
************
TerkinData C++ is a convenient library for handling sensor readings.
It helps to decouple the sensor reading domain from the
telemetry domain in a typical data logger application.

TerkinData C++ was tested successfully on x86_64 (Mac OS X).
It compiles with *xtensa-lx106-elf-g++ 4.8.2* (xtensa-lx106, crosstool-NG 1.20.0), *avr-g++ 4.9.1* (AVR) and *g++ 5.3.0* (MacPorts)
on Mac OS X. It currently does **not** compile with *clang++ 3.4*.



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

        // Optionally prefix header line with string
        this->csv_header_prefix = "## ";

        // Map names of lowlevel sensor values to highlevel telemetry data fields
        (*this->sensor_field_mapping)[string("dht.0.temp")]   = string("temperature");
        (*this->sensor_field_mapping)[string("dht.0.hum")]    = string("humidity");

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

    // Output
    terrine.log("header: ", false);
    terrine.log(data_header.c_str());

    terrine.log("data:   ", false);
    terrine.log(data_record.c_str());

    // Free memory
    delete measurement;


********
Examples
********
.. highlight:: bash

::

    cd examples
    make run

    ======================
    TerkinData CSV example
    ======================

    -- Test single reading (complete)
    header: ## time,weight,temperature-outside,humidity-outside,temperature-inside,voltage
    data:   2017-01-11T15:00:11Z,85.000000,42.419998,84.839996,33.330002,3.843000

    -- Test single reading (incomplete)
    header: ## time,weight,temperature-outside,humidity-outside,temperature-inside,voltage
    data:   2017-01-11T15:00:11Z,,42.419998,84.839996,,3.843000


.. seealso:: Full source of `<csv_basic.cpp_>`_


.. _csv_basic.cpp: https://github.com/hiveeyes/arduino/blob/master/libraries/TerkinData/examples/csv_basic.cpp


********
Download
********

TerkinData C++
==============
    | Please download from GitHub:
    | https://github.com/hiveeyes/arduino/tree/master/libraries/TerkinData


************
Dependencies
************


ArduinoSTL
==========
    | ArduinoSTL 1.0.1 by Mike Matera
    | https://github.com/mike-matera/ArduinoSTL
    | https://github.com/hiveeyes/ArduinoSTL

