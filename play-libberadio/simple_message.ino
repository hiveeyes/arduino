/* -*- coding: utf-8 -*-
 *
 * (c) 2015 Andreas Motl <andreas.motl@elmyra.de>
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

// main program
#include <EmBencode.h>
#include <BERadio.h>

int main() {

    // message object with nodeid=999
    BERadioMessage message(999);

    // enable debugging
    message.debug(true);

    // collect some measurements
    FloatList temperature = collect(21.63, 19.25, 10.92, 13.54);
    FloatList humidity    = collect(488.0, 572.0);
    FloatList weight      = collect(106.77);

    // transfer measurements to message
    message.temperature(temperature);
    //message.add("t", temperature);

    std::string payload = message.encode();
    _l("payload: ");
    _d(payload);

    /*
    IntegerList something = collect(1, 2, 3);
    message.something(something);
    */

    // send message
    //message.send();

    // TODO: enable auto fragmentation by setting mtu value (maximum transfer unit)
    //message.send_mtu(61)

    return 0;
}
