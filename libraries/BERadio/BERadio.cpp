/* -*- coding: utf-8 -*-

BERadio: Convenient and flexible telemetry messaging for Hiveeyes

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
#include <BERadio.h>
#include <simulavr.h>
#include <Arduino.h>


void BERadioEncoder::reset() {
    length = 0;
}

void BERadioEncoder::PushChar(char ch) {

    // Discard overflow data. This is bad.
    if (length >= MTU_SIZE_MAX) {
        //_d("Payload limit reached, aborting serialization");
        return;
    }

    buffer[length] = ch;
    length += 1;
}


void BERadioMessage::set_mtu_size(int size) {
    // Set maximum transfer unit (MTU) size
    mtu_size = size;
}

void BERadioMessage::add(std::string family, std::vector<double> values) {
    // Store list of values into map, keyed by single char family identifier (t, h, w, r)
    _store[family] = values;
}

void BERadioMessage::encode_and_transmit() {

    // Encoder machinery wrapping EmBencode
    // Main message encoder
    BERadioEncoder encoder;

    // Shadow encoder for serializing single elements
    BERadioEncoder shadow;

    // Initialize message, add header
    start_message(encoder);

    // Encode internal data store
    bool do_fragment = false;

    // Iterate data store mapping single-char family identifiers to value lists
    for (auto iterator = _store.begin(); iterator != _store.end(); iterator++) {
    Serial.print("iterator || \n");

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
        Serial.print("family || \n");
        // Encode list of values, apply forward-scaling by * 100

        if (length == 1) {
            // List compression: Lists with just one element don't
            // need to be wrapped into Bencode list containers.
            // TODO: Also care about fragmentation here
            encoder.push(values[0] * 100);

        } else {

            // Lists with two or more elements
            encoder.startList();

            // Iterate list of measurement values
            for (auto value_iterator = values.begin(); value_iterator != values.end(); value_iterator++) {

                // Get list index and value from iterator
                int index = value_iterator - values.begin();
                double value = *value_iterator;
                Serial.println("took values");


                // Simulate Bencode serialization to compute length of encoded element
                shadow.reset();
                shadow.push(value * 100);

                // Compute whether message should be fragmented right here
                int close_padding = 2;       // Two levels of nestedness: dict / list
                do_fragment = encoder.length + shadow.length + close_padding > mtu_size;

                if (do_fragment) {

                    // Close current list
                    Serial.print("do_fragment \n");
                    encoder.endList();

                    // Send out data
                    Serial.println("about to enter fragment_and_send\n");
                    delay(200);
                    fragment_and_send(encoder);

                    // Start new message
                    Serial.println("about to enter start_message\n");
                    start_message(encoder);

                    // Open new list context where we currently left off
                    Serial.println("about to enter continue_list\n");
                    continue_list(encoder, family, index);

                }

                // Just push if it's safe
                encoder.push(value * 100);

                // Refresh "do_fragment" state
                do_fragment = encoder.length + shadow.length + close_padding > mtu_size;

            }
            encoder.endList();

        }

        // End of family values
        if (do_fragment) {

            // Send out data
            fragment_and_send(encoder);

            // Start new message
            start_message(encoder);
        }

    }

    // Regular ending
    if (!do_fragment) {
        // Send out data
        fragment_and_send(encoder);
    }

    // Ready.
}

void BERadioMessage::start_message(BERadioEncoder &encoder) {

    encoder.reset();

    // Open envelope
    encoder.startDict();

    // Add node identifier (integer)
    encoder.push("#");
    encoder.push(nodeid);

    // Add profile identifier (string)
    encoder.push("_");
    encoder.push(profile.c_str());
}

void BERadioMessage::fragment_and_send(BERadioEncoder &encoder) {

    // Close envelope
    Serial.println("inside fragment_and_send\n");
    delay(200);
    encoder.endDict();

    // Convert character buffer of known length to standard string
    std::string payload(encoder.buffer, encoder.length);

    // Debugging
    //_l("payload: "); _d(payload);

    // Transmit message before starting with new one
    send(payload);
    Serial.println(payload.c_str());

}

void BERadioMessage::continue_list(BERadioEncoder &encoder, std::string family, int index) {

    // Augment family identifier with current index, e.g. "t3"

    // FIXME: How to convert from Integer to String more conveniently?
    char intbuf[10];
    std::sprintf(intbuf, "%d", index);

    std::string family_with_index = family + std::string(intbuf);

    // Reopen list

    // Encode the augmented family identifier of this value list
    encoder.push(family_with_index.c_str());

    // Restart list container
    encoder.startList();
}


void BERadioMessage::debug(bool enabled) {
    DEBUG = enabled;
    //_l("Node id: "); _d(nodeid);
    //_l("Profile: "); _d(profile);
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
