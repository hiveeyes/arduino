/* -*- coding: utf-8 -*-
 *
 * (c) 2015-2016 Andreas Motl <andreas.motl@elmyra.de>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 *
 */

// Platform and Standard C++ for Arduino
#include <Arduino.h>
#include <new.cpp>
#include <StandardCplusplus.h>
#include <func_exception.cpp>

// Main program
#include <BERadio.h>
#include <simulavr.h>

void BERadioMessage::send(std::string payload) {

    _l("Sending payload: "); _d(payload);

    /*
    Currently, this will yield three valid payload fragments:

    Sending payload: d1:#i88e1:_2:h11:hli48800ei57200ei7823ee1:tli2163ei1925eee
    Sending payload: d1:#i88e1:_2:h12:t2li1092ei1354ei4242ee1:wli10677ei9922eee
    Sending payload: d1:#i88e1:_2:h12:w2li23440eee

    The fragments are self-contained, so each one can be used on the
    receiver side even if a single fragment gets lost during transmission.

    */

    // TODO: Pluck this into RadioHead
    //transmitter.send(payload);

}

int main() {

    // Message object with nodeid=999
    BERadioMessage message(999);

    // Enable debugging
    message.debug(true);

    // Configure auto fragmentation by setting MTU size (maximum transfer unit)
    // The default is 61 bytes, suitable for sending via RFM69
    //message.set_mtu_size(96);

    // Collect some measurements en bloc
    FloatList temperature = collect(21.63, 19.25, 10.92, 13.54, 42.42);
    FloatList humidity    = collect(488.0, 572.0, 78.23);
    //FloatList weight      = collect(106.77, 99.22);
    FloatList weight      = collect(106.77, 99.22, 234.4);

    // Add yet another value
    weight.push_back(1234567.1234);

    // Empty lists won't get serialized
    FloatList radar;

    // Add measurement values to message
    message.add("t", temperature);
    message.add("h", humidity);
    message.add("w", weight);
    message.add("r", radar);

    // Yak shaving
    message.encode_and_transmit();

    return 0;

}
