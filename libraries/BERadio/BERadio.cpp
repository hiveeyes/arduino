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
#include <BERadio.h>
#include <EmBencode.h>
#include <simulavr.h>

//#define MAX_PAYLOAD_LENGTH 25
#define MAX_PAYLOAD_LENGTH 256


void BERadioMessage::debug(bool enabled) {
    DEBUG = enabled;
    //_l("Node id: "); _d(nodeid);
    //_l("Profile: "); _d(profile);
}

void BERadioMessage::add(std::string family, std::vector<double> values) {
    _store[family] = values;
}

void BERadioMessage::temperature(FloatList values) {

    // finally store list of parsed items
    d_temperatures = values;

    // debugging
    if (DEBUG) {
        //dump_vector("temp", d_temperatures);
    }

}

void BERadioMessage::something(IntegerList values) {

    // finally store list of parsed items
    d_something = values;

    // debugging
    if (DEBUG) {
        //dump_vector("something", d_something);
    }

}

class BERadioEncoder: public EmBencode {
    /*
    Inherits the "EmBencode" class and implements the "PushChar" method.
    Provides public accessible "buffer" and "length" attributes.
    */

    public:
        char buffer[MAX_PAYLOAD_LENGTH];
        int length = 0;

    protected:
        void PushChar(char ch) {

            // discard overflow data. this is bad
            // TODO: automatically fragment message
            if (length >= MAX_PAYLOAD_LENGTH) {
                _d("Payload limit reached, aborting serialization");
                return;
            }

            buffer[length] = ch;
            length += 1;
        }
};


std::string BERadioMessage::encode() {

    // Encoder machinery wrapping EmBencode
    BERadioEncoder encoder;

    // Open envelope
    encoder.startDict();

    // Add node identifier (integer)
    encoder.push("#");
    encoder.push(nodeid);

    // Add profile identifier (string)
    encoder.push("_");
    encoder.push(profile.c_str());

    // Encode internal data store
    for (auto iterator = _store.begin(); iterator != _store.end(); iterator++) {

        // iterator->first  = key
        // iterator->second = value

        // Decode family identifier and list of values from map element (key/value pair)
        std::string family         = iterator->first;
        std::vector<double> values = iterator->second;

        // Number of elements in value list
        int length = values.size();

        // Skip empty lists
        if (length == 0) {
            continue;
        }

        // Encode the family identifier of this value list
        encoder.push(family.c_str());

        // Encode list of values, apply forward-scaling by * 100

        if (length == 1) {
            // List compression: Lists with just one element don't
            // need to be wrapped into Bencode list containers.
            encoder.push(values[0] * 100);

        } else {
            // Lists with two or more elements
            encoder.startList();
            for (double value: values) {
                encoder.push(value * 100);
            }
            encoder.endList();

        }

    }

    // Close envelope
    encoder.endDict();

    // Convert character buffer of known length to standard string
    std::string payload(encoder.buffer, encoder.length);

    // Ready.
    return payload;
}

/*
template<typename T>
void dump_vector(std::string item_prefix, std::vector<T> vec) {

    typename std::vector<T>::const_iterator it;
    int i = 1;
    for (it = vec.begin(); it != vec.end(); it++) {

        // FIXME: How large should this buffer actually be made?
        char buffer[100];
        sprintf(buffer, "%s%d: ", item_prefix.c_str(), i);
        _l(buffer);

        T value = *it;
        _d(value);

        i++;
    }

}
*/
