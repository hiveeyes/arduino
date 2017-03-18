/**
 *
 * TerkinData: Flexible data collection for decoupling sensor reading and telemetry domains
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
#include "TerkinData.h"

#if defined(HAVE_ARDUINO_JSON)
    #include <ArduinoJson.h>
#endif


using namespace TerkinData;
using namespace TerkinUtil;


DataManager::DataManager() {
    this->sensor_field_mapping = new FieldMap();
    this->csv_header_prefix = new std::string();
    this->setup();
}


/**
 *
 * Translate lowlevel sensor value names to telemetry field names
 * resolved through ``this->sensor_field_mapping``.
 *
**/
void DataManager::map_fields(Measurement& measurement) {
    FieldMap *fieldmap = this->sensor_field_mapping;
    for (auto item: measurement.data) {

        std::string key = item.first;
        float value = item.second;

        // #include <stdio.h>
        //std::cout << "key: " << key << std::endl;

        if (key_exists(*fieldmap, key)) {

            // ArduinoSTL: "error: ‘TerkinData::FieldMap’ has no member named ‘at’"
            //std::string key_translated = fieldmap->at(key);

            std::string key_translated = (*fieldmap)[key];
            measurement.data[key_translated] = value;

            // FIXME: Boom: "section `.text' will not fit in region `iram1_0_seg'"
            //measurement.data.erase(key);

            // TODO: Check if deleting elements works while iterating through the map, see also:
            // - http://stackoverflow.com/questions/4600567/how-can-i-delete-elements-of-a-stdmap-with-an-iterator
            // - http://stackoverflow.com/questions/19970531/properly-destroying-pointers-in-an-stdmap

        }
    }
}


/**
 *
 * Serialize header field names to CSV format
 *
**/
std::string DataManager::csv_header() {
    std::string header_line = join(*this->field_names, ',');
    return *this->csv_header_prefix + header_line;
}


/**
 *
 * Serialize measurement data to CSV format
 *
**/
std::string DataManager::csv_data(Measurement& measurement) {

    // Translate lowlevel sensor value names to telemetry field names
    this->map_fields(measurement);

    // Container for CSV data elements
    std::vector<std::string> items;

    // Iterate header field names
    for (std::string name: *this->field_names) {

        // #include <stdio.h>
        //std::cout << "name: " << name << std::endl;

        // Handle .time specially
        if (name == "time") {
            items.push_back(measurement.time);
            continue;
        }

        // Add reading values in order of
        if (key_exists(measurement.data, name)) {
            float value = measurement.data[name];
            items.push_back(to_string(value));
        } else {
            items.push_back("");
        }
    }
    return join(items, ',');

}


/**
 *
 * Serialize measurement data to JSON format
 *
**/
std::string DataManager::json_data(Measurement& measurement) {

#if defined(HAVE_ARDUINO_JSON)

    // Translate lowlevel sensor value names to telemetry field names
    this->map_fields(measurement);

    // Container object for JSON data elements
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    // Iterate header field names
    for (std::string name: *this->field_names) {

        // #include <stdio.h>
        //std::cout << "name: " << name << std::endl;

        // Handle .time specially
        if (name == "time") {
            json[name] = measurement.time;
            continue;
        }

        // Add values of sensor readings
        if (key_exists(measurement.data, name)) {
            float value = measurement.data[name];
            json[name] = to_string(value);
        }
    }

    // Serialize to JSON string
    char payload[256];
    json.printTo(payload, sizeof(payload));

    return payload;

#else

    return "JSON serialization unavailable, please compile using '-DHAVE_ARDUINO_JSON'.";

#endif

}
