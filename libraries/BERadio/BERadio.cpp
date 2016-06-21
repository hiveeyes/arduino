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
#include <beradio.h>
#include <EmBencode.h>
#include <simulavr.h>

#define NODEID      999
//#define MAX_PAYLOAD_LENGTH 61
#define MAX_PAYLOAD_LENGTH 256


void BERadioMessage::debug(bool enabled) {
    DEBUG = enabled;
    _l("Node id: "); _d(nodeid);
    _l("Profile: "); _d(profile);
}

void BERadioMessage::temperature(FloatList values) {

    // finally store list of parsed items
    d_temperatures = values;

    // debugging
    if (DEBUG) {
        dump_vector("temp", d_temperatures);
    }

}

void BERadioMessage::something(IntegerList values) {

    // finally store list of parsed items
    d_something = values;

    // debugging
    if (DEBUG) {
        dump_vector("something", d_something);
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
            if (length >= MAX_PAYLOAD_LENGTH)
                return;

            buffer[length] = ch;
            length += 1;
        }
};


std::string BERadioMessage::encode() {

    // encoder machinery
    BERadioEncoder encoder;

    // open envelope
    encoder.startDict();

    // add node identifier
    encoder.push("#");
    encoder.push(NODEID);

    // add profile identifier
    encoder.push("_");
    encoder.push(profile.c_str());

    // encode list of temperature values, apply forward-scaling by *100
    encoder.push("t");
    encoder.startList();
    for (double value: d_temperatures) {
        encoder.push(value * 100);
    }
    encoder.endList();

    // close envelope
    encoder.endDict();

    // convert character buffer of known length to standard string
    std::string payload(encoder.buffer, encoder.length);

    // ready to send
    return payload;
}


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
