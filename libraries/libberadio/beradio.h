/* -*- coding: utf-8 -*-

libberadio: Convenient and flexible telemetry messaging for Hiveeyes

Copyright (C) 2015  Andreas Motl <andreas.motl@elmyra.de>
Copyright (C) 2015  Richard Pobering <einsiedlerkrebs@ginnungagap.org>

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
#include <execvf.h>
#include <varargs.h>
#include <string>
#include <vector>
#include <iterator>
#include <simulavr.h>


// Macro for supporting variadic argument processing.
// Drives the "varargs" template to convert a variable list
// of function arguments into a vector containing all items.
#define collect(...) varargs(VA_LENGTH(__VA_ARGS__), __VA_ARGS__)

// Shortcuts for standard vectors containing items of various types
#define FloatList std::vector<double>
#define IntegerList std::vector<int>

// Generic "dump vector" utility function
template<typename T>
void dump_vector(std::string prefix, std::vector<T> vec);


class BERadioMessage {

    public:

        // constructor
        BERadioMessage(int nodeid, std::string profile="h1") {
            this->nodeid  = nodeid;
            this->profile = profile;
        };

        // enable/disable debugging
        void debug(bool enabled);

        // measure multiple temperatures
        void temperature(FloatList values);
        void something(IntegerList values);

        std::string encode();

    private:
        bool DEBUG = false;
        int nodeid;
        std::string profile;

        // internal data store
        FloatList d_temperatures;
        IntegerList d_something;

};
