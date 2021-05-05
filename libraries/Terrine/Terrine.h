/* -*- coding: utf-8 -*-

Terrine: Application boilerplate for convenient MCU development

Copyright (C) 2015-2016  Andreas Motl <andreas.motl@elmyra.de>
Copyright (C) 2015-2016  Richard Pobering <einsiedlerkrebs@ginnungagap.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see:
<http://www.gnu.org/licenses/gpl-3.0.txt>,
or write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/
#ifndef Terrine_h
#define Terrine_h

#if defined ARDUINO
    #include <Arduino.h>
#elif defined SIMULAVR
    #include <simulavr.h>
#elif defined __unix__
    #include <RHutil/simulator.h>
#endif


#include <execvf.h>
#include <varargs.h>
/*
#include <string>
#include <vector>
#include <map>
#include <iterator>
*/

#define TERRINE_SERIAL_DELAY 100

#if defined(__unix__)
    #include <sstream>
    namespace std {
        std::string to_string(size_t n) {
            std::ostringstream s;
            s << n;
            return s.str();
        }
    }
#endif
#if defined(__unix__)
    extern char *ultoa(unsigned long int __val, char *__s, int __radix) {
        snprintf(__s, __radix, "%d", __val);
        return __s;
    }
#endif

// Macro for supporting variadic argument processing.
// Drives the "varargs" template to convert a variable list
// of function arguments into a vector containing all items.
#define collect(...) varargs(VA_LENGTH(__VA_ARGS__), __VA_ARGS__)

// Shortcuts for standard vectors containing items of various types
#define FloatList std::vector<double>
#define IntegerList std::vector<int>

// Generic "dump vector" utility function
//template<typename T>
//void dump_vector(std::string prefix, std::vector<T> vec);

class Terrine {

    public:

        // Enable/disable debugging
        //void debug(bool enabled);

        //void dprint(const char *message);
        void log(const char *message, bool newline=true);
        void log(int value);
        void log(bool newline=true);

        int memfree();
        void logmem();

    private:
        bool DEBUG = false;

};

extern "C" Terrine terrine;

#endif
