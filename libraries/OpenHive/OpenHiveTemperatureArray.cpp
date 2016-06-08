/**
 *
 * OpenHiveTemperatureArray - decouple sensor reading for the Open Hive hardware
 *
 *
 *  Copyright (C) 2015-2016  Andreas Motl <andreas.motl@elmyra.de>
 *  Copyright (C) 2015-2016  Clemens Gruber <mail@clemens-gruber.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  See also "LICENSE.txt" in the root folder of
 *  this repository or distribution package.
 *
 *
 * OpenHiveTemperatureArray provides a wrapper around the reading of
 * sensor values from the Open Hive Temperature Array sensor family.
 *
 * You would normally do this in your main sketch, now it
 * gets a new home to decouple sensor and telemetry domains.
 *
 *
 * ---------------------------------------------------
 *   Software driver for Open Hive Temperature Array
 * ---------------------------------------------------
 *
 * Temperature arrays are special, so they emit special payloads:
 *
 * {"meta": {
 *            "type":       "array",
 *            "subtype":    "2d",
 *            "unit":       "celsius",
 *          },
 *  "data": {
 *            "frame-1": [34.45, 35.56, 36.67],
 *            "frame-2": [34.65, 35.76, 36.87],
 *            "frame-3": [34.85, 35.96, 37.07],
 *          }
 * }
 *
 * Todo:
 *
 *   - Let's see how values are actually read from the hardware
 *   - Generalize sensor reading and looping over frames
 *
**/
#include "OpenHiveTemperatureArray.h"

using namespace OpenHive;


/**
 *
 * OpenHiveTemperatureArray just wraps the consecutive reading of
 * sensor values from the Open Hive Temperature Array sensor family.
 * into a single method "read" and provides the caller with a
 * JsonObject ready for pushing into the transmitter machinery.
 *
 * It demonstrates the idea by means of method "read_demo".
 *
**/

OpenHiveTemperatureArray::OpenHiveTemperatureArray() {};

JsonObject& OpenHiveTemperatureArray::read() {

    /* Talk to hardware sensors and emit sensor readings */

    // Data container object
    JsonObject& data = createContainer();

    // Actually read hardware sensors
    // Maybe use structure from "read_demo"
    // FIXME: Read 2D temperature array sensors, frame by frame

    return data;

}

JsonObject& OpenHiveTemperatureArray::read_demo() {

    /* Emit demonstration payload for testing the WAN communication */

    // Data container object
    JsonObject& container = createContainer();
    JsonObject& data      = container["data"];

    // Example sensor values representing a two-dimensional
    // array of temperature values, frame by frame
    for (int frame_number = 1; frame_number <= 4; frame_number++) {

        // Container for representing a single temperature array
        JsonArray& frame = data.createNestedArray(get_name(frame_number));
        for (int sensor_number = 1; sensor_number <= 8; sensor_number++) {
            frame.add(42.424242, 6);  // 6 is the number of decimals to print
            //frame.add(2.302038);      // if not specified, 2 digits are printed
        }

    }

    return data;

}

// Compute the name used as attribute name
// inside the data container for each frame
const char * OpenHiveTemperatureArray::get_name(int frame_number) {
    /* Frame name builder, e.g. "frame-42" */
    return (String("frame-") + String(frame_number)).c_str();
}

// Create the data container root object
JsonObject& OpenHiveTemperatureArray::createContainer() {
    /* Factory for JSON data container object */

    // Data container objects
    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    JsonObject& meta = jsonBuffer.createObject();
    JsonObject& data = jsonBuffer.createObject();

    // Set metadata values
    meta["type"]    = "array";
    meta["subtype"] = "2d";
    meta["unit"]    = "celsius";

    // Root structure
    root["meta"]    = meta;
    root["data"]    = data;

    return root;
}

// A singleton instance of the OpenHiveTemperatureArray
// class exported to user space code.
OpenHiveTemperatureArray openhive_temparray;
