/**
 *
 * TerkinData:  Flexible data collection for decoupling sensor reading and telemetry domains
 * urlencode example: Collect measurement readings and serialize to x-www-form-urlencoded
 *
 *
 *  Copyright (C) 2017  Andreas Motl <andreas.motl@elmyra.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  https://www.gnu.org/licenses/lgpl-3.0.txt
 *
**/

// Terrine: Application boilerplate
#include <Terrine.h>
Terrine terrine;

// TerkinData - flexible data collection for sensor readings to decouple measurement from telemetry domain
#include <TerkinData.h>
using namespace TerkinData;
using namespace TerkinUtil;

// Misc utils
using std::string;
std::string separator = std::string(42, '=');


// -------------------------
// DataManager configuration
// -------------------------
void DataManager::setup() {

    // List of field names
    this->field_names  = new DataHeader({"time", "weight", "temperature-outside", "humidity-outside", "temperature-inside", "voltage"});

    // Map names of lowlevel sensor values to highlevel telemetry data fields
    (*this->sensor_field_mapping)[string("dht.0.temp")]   = string("temperature-outside");
    (*this->sensor_field_mapping)[string("dht.0.hum")]    = string("humidity-outside");
    (*this->sensor_field_mapping)[string("ds18b20.0")]    = string("temperature-inside");

}

DataManager *datamgr = new DataManager();

void getTimestamp(Measurement& measurement) {
    //measurement.time = "2012-03-02T04:07:34.0218628Z";
    measurement.time = now_iso();
}

// Forward declarations
void dump(Measurement *measurement);

void basic_single() {

    terrine.log("-- Test single reading (complete)");

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

    // Display measurement in x-www-form-urlencoded format
    dump(measurement);

    // Free memory
    delete measurement;

}

void basic_missing() {

    terrine.log("-- Test single reading (incomplete)");

    // Data container to collect one reading
    Measurement *measurement = new Measurement();

    // Fill timestamp
    getTimestamp(*measurement);

    // Fill dummy values
    measurement->data["dht.0.temp"]  = 42.42f;
    measurement->data["dht.0.hum"]   = 84.84f;
    measurement->data["voltage"]     = 3.843f;

    // Display measurement in x-www-form-urlencoded format
    dump(measurement);

    // Free memory
    delete measurement;

}

void dump(Measurement *measurement) {

    // Serialize data into x-www-form-urlencoded format
    std::string data_record = datamgr->urlencode_data(*measurement);

    // Output
    terrine.log("data:   ", false);
    terrine.log(data_record.c_str());

    terrine.log();
}

int main() {

    terrine.log("========================================");
    terrine.log("TerkinData x-www-form-urlencoded example");
    terrine.log("========================================");
    terrine.log();

    basic_single();
    basic_missing();

}
