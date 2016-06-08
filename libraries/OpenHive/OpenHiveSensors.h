/**
 *
 * OpenHiveSensors - decouple sensor reading for the Open Hive hardware
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
 * OpenHiveSensors provides a wrapper around the reading of
 * basic sensors like temperature, humidity and weight.
 *
 * You would normally do this in your main sketch, now it
 * gets a new home to decouple sensor and telemetry domains.
 *
**/
#ifndef OpenHiveSensors_h
#define OpenHiveSensors_h

#include <ArduinoJson.h>                // https://github.com/bblanchon/ArduinoJson

namespace OpenHive {

    /**
     *
     * OpenHiveSensors just wraps the consecutive reading of
     * multiple basic environment sensors into a single method
     * call "read" and provides the caller with a JsonObject
     * ready for pushing into the transmitter machinery.
     *
     * It demonstrates the idea by means of method "read_demo".
     *
    **/
    class OpenHiveSensors {
        public:
            OpenHiveSensors();
            JsonObject& read();
            JsonObject& read_demo();
    };

}

// A singleton instance of the OpenHiveSensors
// class exported to user space code.
using namespace OpenHive;
extern OpenHiveSensors openhive_sensors;
#endif
