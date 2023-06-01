/**
 *
 * TerkinData: A small data collection framework for decoupling sensor reading and telemetry domains.
 * SQL DML example: Collect measurement readings and serialize to `SQL INSERT` statement.
 *
 *
 *  Copyright (C) 2023  Andreas Motl <andreas.motl@panodata.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  https://www.gnu.org/licenses/lgpl-3.0.txt
 *
**/

// Terrine: Application boilerplate.
#include <Terrine.h>
Terrine terrine;

// TerkinData: A small data collection framework for decoupling sensor reading and telemetry domains.
#include <TerkinData.h>
using namespace TerkinData;
using namespace TerkinUtil;

// Miscellaneous utilities.
using std::string;
std::string separator = std::string(42, '=');


// -------------------------
// DataManager configuration
// -------------------------
void DataManager::setup() {

    // List of field names
    this->field_names  = new DataHeader({"time", "weight", "temperature_outside", "humidity_outside", "temperature_inside", "voltage"});

    // Map names of lowlevel sensor values to highlevel telemetry data fields
    (*this->sensor_field_mapping)[string("dht.0.temp")]   = string("temperature_outside");
    (*this->sensor_field_mapping)[string("dht.0.hum")]    = string("humidity_outside");
    (*this->sensor_field_mapping)[string("ds18b20.0")]    = string("temperature_inside");

}

DataManager *datamgr = new DataManager();

void getTimestamp(Measurement& measurement) {
    //measurement.time = "2012-03-02T04:07:34.0218628Z";
    measurement.time = now_iso();
}

// Forward declarations
void invoke_create(Measurement *measurement);
void invoke_insert(Measurement *measurement);


void basic_create() {

    terrine.log("-- Test DDL");

    // Data container to define schema.
    Measurement *measurement = new Measurement();

    // Display SQL CREATE statement.
    invoke_create(measurement);

    // Free memory
    delete measurement;

}

void basic_insert_single() {

    terrine.log("-- Test DML single reading (complete)");

    // Data container to collect one reading
    Measurement *measurement = new Measurement();

    // Fill timestamp
    getTimestamp(*measurement);

    // Fill dummy values
    measurement->data["weight"]      = 85.00f;
    measurement->data["dht.0.temp"]  = 42.42f;
    measurement->data["dht.0.hum"]   = 84.84f;
    measurement->data["ds18b20.0"]   = 33.33f;
    measurement->data["voltage"]     = 3.843f;

    // Display SQL INSERT statement.
    invoke_insert(measurement);

    // Free memory
    delete measurement;

}

void basic_insert_missing() {

    terrine.log("-- Test DML single reading (incomplete)");

    // Data container to collect one reading
    Measurement *measurement = new Measurement();

    // Fill timestamp
    getTimestamp(*measurement);

    // Fill dummy values
    measurement->data["dht.0.temp"]  = 42.42f;
    measurement->data["dht.0.hum"]   = 84.84f;
    measurement->data["voltage"]     = 3.843f;

    // Display SQL INSERT statement.
    invoke_insert(measurement);

    // Free memory
    delete measurement;

}

void invoke_create(Measurement *measurement) {

    // Serialize data into SQL INSERT statement.
    std::string data_record = datamgr->sql_create("testdrive", *measurement);

    // Output
    terrine.log("data:   ", false);
    terrine.log(data_record.c_str());

    terrine.log();
}

void invoke_insert(Measurement *measurement) {

    // Serialize data into SQL INSERT statement.
    std::string data_record = datamgr->sql_insert("testdrive", *measurement);

    // Output
    terrine.log("data:   ", false);
    terrine.log(data_record.c_str());

    terrine.log();
}

// Program entrypoint for glibc.
int main() {

    terrine.log("==============================");
    terrine.log("TerkinData SQL DDL+DML example");
    terrine.log("==============================");
    terrine.log();

    basic_create();
    basic_insert_single();
    basic_insert_missing();

}

// Program entrypoints for Arduino.
void setup() {
    main();
}
void loop() {
}
