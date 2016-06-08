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
#ifndef OpenHiveTemperatureArray_h
#define OpenHiveTemperatureArray_h

#include <ArduinoJson.h>                // https://github.com/bblanchon/ArduinoJson

namespace OpenHive {


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
    class OpenHiveTemperatureArray {
        public:
            OpenHiveTemperatureArray();
            JsonObject& read();
            JsonObject& read_demo();
        private:

            // Compute the name used as attribute name
            // inside the data container for each frame
            const char * get_name(int frame_number);

            // Create the data container root object
            JsonObject& createContainer();
    };

}

// A singleton instance of the OpenHiveTemperatureArray
// class exported to user space code.
using namespace OpenHive;
extern OpenHiveTemperatureArray openhive_temparray;
#endif
