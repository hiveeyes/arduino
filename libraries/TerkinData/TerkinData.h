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
#ifndef TerkinData_h
#define TerkinData_h

#if defined(ARDUINO)

    #include <Arduino.h>

    // Standard C++ (STL) for Arduino for dynamic data structures like vector and map.
    // Required for AVR only, STL already seems to be included in Espressif SDK.
    #ifndef ARDUINO_ARCH_ESP8266
        #include <ArduinoSTL.h>
    #endif

#else

    //#include <stdio.h>
    #include <time.h>

#endif

// Dynamic data containers
#include <vector>
#include <map>

namespace TerkinData {

    // For storing a list of field names
    typedef std::vector<std::string> DataHeader;

    // For storing measurement values from sensors
    typedef std::map<std::string, float> DataRecord;

    // For mapping lowlevel sensor reading labels to highlevel telemetry field names
    typedef std::map<std::string, std::string> FieldMap;

    // Data container to collect all measurement readings
    class Measurement {
        public:
            // TODO: Automatically fill this->time with now_iso() if empty
            std::string time;
            DataRecord data;
    };

    // The main data munching utility
    class DataManager {
        public:

            // Constructor
            DataManager();

            // List of telemetry field names
            DataHeader* field_names;

            // Optional list of additional field labels
            DataHeader* field_labels;

            // Map lowlevel sensor value names to telemetry field names
            FieldMap* sensor_field_mapping;

            // Optionally prefix CSV header line with string
            //static const std::string csv_header_prefix;
            std::string *csv_header_prefix;

            // Translate lowlevel sensor value names to telemetry field names
            void map_fields(Measurement&);

            // Serialize header field names to CSV format
            std::string csv_header();

            // Serialize measurement data to CSV format
            std::string csv_data(Measurement&);

            // Serialize measurement data to JSON format
            std::string json_data(Measurement&);

        private:
            // Configure the DataManager instance at runtime
            void setup();
    };


    // Define how to map names of lowlevel sensor values to highlevel data fields
    //DataManager::make_sensor_field_mapping

    // We can't use static intializers with ArduinoSTL due to "array used as initializer". Maybe in the future, see also:
    // - https://stackoverflow.com/questions/2172053/c-can-i-statically-initialize-a-stdmap-at-compile-time/2172280#2172280
    // - https://stackoverflow.com/questions/2172053/c-can-i-statically-initialize-a-stdmap-at-compile-time/2172077#2172077

    // Can't initialize this way as variables should go to the heap
    /*
    FieldMap map;
    map["dht.0.hum"]      = "humidity-outside";
    map["ds18b20.0"]      = "temp-brood";
    */

    // Don't define this mapping in global scope to prevent "does not name a type" kinds of errors, see also:
    // - http://electronics.stackexchange.com/questions/72924/error-when-compiling-does-not-name-a-type
    // - http://arduino.stackexchange.com/questions/12304/arduino-raise-the-error-does-not-name-a-type-when-an-object-is-used-outside-o

}


// -----------------
// Various utilities
// -----------------
namespace TerkinUtil {

    // Join std::vector elements to std::string
    // http://stackoverflow.com/questions/1430757/c-vector-to-string
    template< typename T >
    std::string join( const std::vector<T>& values, char delimiter )
    {
        std::string result;
        for( typename std::vector<T>::size_type idx = 0; idx < values.size(); ++idx )
        {
            if( idx != 0 )
                result += delimiter;
            //result += std::to_string( values[idx] );
            result += values[idx];
        }
        return result;
    }

    // Test std::map key for existence
    // http://stackoverflow.com/questions/2781899/how-to-find-whether-an-element-exists-in-stdmap
    // http://stackoverflow.com/questions/1939953/how-to-find-if-a-given-key-exists-in-a-c-stdmap/37348004#37348004
    template <typename T, typename Key>
    bool key_exists(const T& container, const Key& key) {
        return (container.find(key) != container.end());
    }

    /*
    template <typename T> std::string to_string(const T& t) {
       std::ostringstream os;
       os << t;
       return std::string(os.str());
    }
    */

    template <typename T> std::string to_string(const T& t) {

        // This is currently Arduino-only
        #if defined(ARDUINO)
            return std::string(String(t).c_str());
        #else
            return std::to_string(t);
        #endif

        // FIXME: How to convert from Integer to String more conveniently?
        //char buffer[10];
        //snprintf(buffer, 10, "%f", value);
        //dtostrf(value, 8, 3, buffer);

        //return std::string(buffer);

    }

    #if not defined(ARDUINO)

    // http://www.cplusplus.com/reference/ctime/strftime/
    static std::string now_iso() {
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, 80, "%FT%TZ", timeinfo);

        return std::string(buffer);
    }
    #endif

}

#endif
