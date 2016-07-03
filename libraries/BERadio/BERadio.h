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
#ifndef BERadio_h
#define BERadio_h

#include <string>
#include <vector>
#include <map>
#include <iterator>

#include <EmBencode.h>
#include <Terrine.h>

// Maximum of transmission payloads, used for static buffer
#define MTU_SIZE_MAX 61


class BERadioEncoder: public EmBencode {
    /*
    Inherits the "EmBencode" class and implements the "PushChar" method.
    Provides public accessible "buffer" and "length" attributes.
    */

    public:
        char buffer[MTU_SIZE_MAX];
        int length = 0;
        void reset();

    protected:
        void PushChar(char ch);
};

class BERadioShadowEncoder: public BERadioEncoder {
    /*
    Same as BERadioEncoder but with smaller buffer size.
    */

    public:
        // Note: Encoding a maximum representable float like -1234567.1234
        // takes 9 bytes plus prefix (i) and suffix (e) = 11 bytes.
        // The machinery needs one more character for placing a nullbyte
        // at the end of the buffer, so the buffer size needs to be 11.
        char buffer[11];
        int length = 0;
        void reset();

};

class BERadioMessage {

    public:

        int nodeid;
        std::string profile;
        // TODO: TelemetryTransmitter transmitter;
        int mtu_size;

        // Constructor variants
        BERadioMessage(int nodeid) : BERadioMessage(nodeid, "h1", 61) {}
        BERadioMessage(int nodeid, std::string profile) : BERadioMessage(nodeid, profile, 61) {}
        BERadioMessage(int nodeid, std::string profile, int mtu_size) {
            this->nodeid  = nodeid;
            this->profile = profile;
            this->mtu_size = mtu_size;
        };

        // Enable/disable debugging
        void debug(bool enabled);

        // Set MTU (maximum transfer unit) size
        void set_mtu_size(int size);

        // Add list of measurement values
        void add(char *family, FloatList &values);

        // Call to trigger the encoding and transmission process
        void transmit();

        // Get called with serialized payload to put on the wire
        void send(std::string &payload);
        void send(char* buffer, int length);

        //void dprint(const char *message);
        void dprint(const char *message, bool newline=true);
        void dprint(int value);

    private:
        bool DEBUG = false;

        // Internal data store
        std::map< char *, std::vector<double> > _store;

        // Start message envelope
        void start_message(BERadioEncoder &encoder);
        void fragment_and_send(BERadioEncoder &encoder);
        void continue_list(BERadioEncoder &encoder, std::string family, int index);

};

#endif
