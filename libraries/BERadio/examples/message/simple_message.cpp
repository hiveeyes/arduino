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
#include <MemoryFree.h>

// Radio
/*
#include <RH_RF95.h>
#include <RHReliableDatagram.h>
#include <SPI.h>
*/


bool sendtoWait(uint8_t* buf, uint8_t len, uint8_t address) {
    //
}

void BERadioMessage::dprint(const char *message) {
    #ifdef SIMULAVR
        _d(message);
    #else
        Serial.println(message);
    #endif
}
void BERadioMessage::dprint(int value) {
    #ifdef SIMULAVR
        _d(value);
    #else
        Serial.println(value);
    #endif
}

void BERadioMessage::send(std::string &payload) {

    //_d("abc");
    dprint("Sending payload: ");
    dprint(payload.c_str());

    /*
    Currently, this will yield three valid payload fragments:

    Sending payload: d1:#i88e1:_2:h11:hli48800ei57200ei7823ee1:tli2163ei1925eee
    Sending payload: d1:#i88e1:_2:h12:t2li1092ei1354ei4242ee1:wli10677ei9922eee
    Sending payload: d1:#i88e1:_2:h12:w2li23440eee

    The fragments are self-contained, so each one can be used on the
    receiver side even if a single fragment gets lost during transmission.

    */

    // TODO: Pluck this into RadioHead
    //RH_RF69 driver;
    //RH_RF69 driver(15, 16); // For RF69 on PJRC breakout board with Teensy 3.1
    //RH_RF69 rf69(4, 2); // For MoteinoMEGA https://lowpowerlab.com/shop/moteinomega

    // Class to manage message delivery and receipt, using the driver declared above
    //RHReliableDatagram manager(driver, CLIENT_ADDRESS);


    // Convert from std::string to mutable (not “const”) C char array to pass from C++ into C code
    /*
    char *buf69 = new char[payload.length() + 1];
    strcpy(buf69, payload.c_str());
    */

    // Pointer magic: Is this really correct?
    // https://stackoverflow.com/questions/25360893/convert-char-to-uint8-t/25365098#25365098
    //uint8_t cc = *buf69;

    // Convert from char * to uint8_t
    //uint8_t payload_c = *(payload.c_str());
    //sendtoWait(&payload_c, payload.length(), 5);

}

int main() {

    //dprint("Start program");


    //dprint("Hello world");

    //int fm = freeMemory();

    //_l("freeMemory"); dprint(freeMemory());


    //dprint("Setup message");

    // Message object with nodeid=999
    BERadioMessage *message = new BERadioMessage(999);

    // Enable debugging
    message->debug(true);

    // Configure auto fragmentation by setting MTU size (maximum transfer unit)
    // The default is 61 bytes, suitable for sending via RFM69
    //message->set_mtu_size(96);

    message->dprint("Read sensors");

    // Collect some measurements en bloc
    /*
    FloatList temperature = collect(21.63, 19.25, 10.92, 13.54, 42.42);
    FloatList humidity    = collect(488.0, 572.0, 78.23);
    //FloatList weight      = collect(106.77, 99.22);
    FloatList weight      = collect(106.77, 99.22, 234.4);
    */

    // Add yet another value
    FloatList *weight = new FloatList();
    weight->push_back(1234567.1234);
    weight->push_back(1234567.1234);
    weight->push_back(1234567.1234);
    weight->push_back(1234567.1234);
    weight->push_back(1234567.1234);
    weight->push_back(1234567.1234);
    weight->push_back(1234567.1234);
    weight->push_back(1234567.1234);
    weight->push_back(1234567.1234);

    // Empty lists won't get serialized
    //FloatList radar;

    // Add measurement values to message
    message->dprint("Values to message");
    /*
    message->add("t", temperature);
    message->add("h", humidity);
    message->add("r", radar);
    */
    message->add("w", *weight);

    // Yak shaving
    message->dprint("Transmit message");
    message->encode_and_transmit();

    delete weight;
    delete message;

    return 0;

}
