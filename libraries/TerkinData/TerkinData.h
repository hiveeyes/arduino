/**
 *
 * TerkinData: Flexible data collection for decoupling sensor reading and
 *             telemetry domains.
 *
 *  Copyright (C) 2017-2021  Andreas Motl <andreas.motl@elmyra.de>
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
    #if ARDUINO_ARCH_AVR
        #include <ArduinoSTL.h>
    #endif

#else

    //#include <stdio.h>
    #include <time.h>

#endif

// Dynamic data containers
#include <string>
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

            // At which decimal to round floats
            int float_precision = 2;

            // Translate lowlevel sensor value names to telemetry field names
            void map_fields(Measurement&);

            // Serialize header field names to CSV format
            std::string csv_header();

            // Serialize measurement data to CSV format
            std::string csv_data(Measurement&);

            // Serialize measurement data to JSON format
            std::string json_data(Measurement&);

            // Serialize measurement data to SQL statements
            std::string sql_create(std::string, Measurement&);
            std::string sql_insert(std::string, Measurement&);

            // Serialize measurement data to x-www-urlencoded format
            std::string urlencode_data(Measurement&);

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

    template <typename T> std::string to_string(const T& t, int precision) {
        // This is currently Arduino-only
        #if defined(ARDUINO)
            return std::string(String(t).c_str());
        #else
            //return std::to_string(t, precision);
            std::string buffer(16, '\0');
            std::string format = "%." + to_string(precision) + "f";
            //auto result = std::snprintf(&buffer[0], buffer.size(), "%.2f", t);
            auto result = std::snprintf(&buffer[0], buffer.size(), format.c_str(), t);
            if (result < 0) {
                // ERROR
            }
            return buffer.c_str();
        #endif
    }

    // http://www.cplusplus.com/reference/ctime/strftime/
    static std::string now_iso() {
        #if defined(ARDUINO)
        return std::string("FIXME: ISO timestamp on AVR");
        /*
        //#include <time.h>

        //TimeElements tm;
        //tm tms;
        //time_t t = makeTime(tm);
        time_t t = mktime();
        //#include "time.h"
        time_t t = now();
        char buff[] = "20180810T143000Z";
        sprintf(buff, "%02d-%02d-%02dT%02d:%02d:%02d", year(t), month(t), day(t), hour(t), minute(t), second(t));
        //return std::string(isotime().c_str());
        //return std::string(t);
        return std::string(buff);
        */
        #else
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, 80, "%FT%TZ", timeinfo);

        return std::string(buffer);
        #endif
    }



    // https://github.com/zenmanenergy/ESP8266-Arduino-Examples/blob/master/helloWorld_urlencoded/urlencode.ino

    /*
    ESP8266 Hello World urlencode by Steve Nelson
    URLEncoding is used all the time with internet urls. This is how urls handle funny characters
    in a URL. For example a space is: %20
    These functions simplify the process of encoding and decoding the urlencoded format.

    It has been tested on an esp12e (NodeMCU development board)
    This example code is in the public domain, use it however you want.

    Prerequisite Examples:
    https://github.com/zenmanenergy/ESP8266-Arduino-Examples/blob/master/helloWorld_urlencoded/helloWorld_urlencoded.ino
    */

    static unsigned char h2int(char c);

    static std::string urldecode(std::string str)
    {

        std::string encodedString="";
        char c;
        char code0;
        char code1;
        for (unsigned int i = 0; i < str.length(); i++){
            c=str.at(i);
          if (c == '+'){
            encodedString+=' ';
          }else if (c == '%') {
            i++;
            code0=str.at(i);
            i++;
            code1=str.at(i);
            c = (h2int(code0) << 4) | h2int(code1);
            encodedString+=c;
          } else{

            encodedString+=c;
          }
        }

       return encodedString;
    }

    static std::string urlencode(std::string str)
    {
        std::string encodedString="";
        char c;
        char code0;
        char code1;
        //char code2;
        for (unsigned int i = 0; i < str.length(); i++){
          c=str.at(i);
          if (c == ' '){
            encodedString+= '+';
          } else if (isalnum(c)){
            encodedString+=c;
          } else{
            code1=(c & 0xf)+'0';
            if ((c & 0xf) >9){
                code1=(c & 0xf) - 10 + 'A';
            }
            c=(c>>4)&0xf;
            code0=c+'0';
            if (c > 9){
                code0=c - 10 + 'A';
            }
            //code2='\0';
            encodedString+='%';
            encodedString+=code0;
            encodedString+=code1;
            //encodedString+=code2;
          }
        }
        return encodedString;

    }

    static unsigned char h2int(char c)
    {
        if (c >= '0' && c <='9'){
            return((unsigned char)c - '0');
        }
        if (c >= 'a' && c <='f'){
            return((unsigned char)c - 'a' + 10);
        }
        if (c >= 'A' && c <='F'){
            return((unsigned char)c - 'A' + 10);
        }
        return(0);
    }

}

#endif
