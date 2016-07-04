/* -*- coding: utf-8 -*-
 *
 * (c) 2015-2016 Andreas Motl <andreas.motl@elmyra.de>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License.
 *
 */

// --------
// Platform
// --------

//#ifdef ARDUINO

    // Arduino platform
    #include <Arduino.h>

    // Standard C++ for Arduino
    #include <new.cpp>
    #include <StandardCplusplus.h>
    #include <func_exception.cpp>
    #include <MemoryFree.h>

    #include <simulavr.h>
    #include <simulavr.cpp>

//#endif

// Terrine: Application boilerplate for convenient MCU development
#include <Terrine.h>
#include <Terrine.cpp>

//extern "C" Terrine terrine;
Terrine terrine;


// Main program
#include <EmBencode.h>
#include <BERadio.h>
#include <BERadio.cpp>



/*
void setup() {
    Serial.begin(115200);
}

void loop() {
    Serial.println("Hello world!");
    delay(100);
}
*/

int main() {

    terrine.log("Start program");

    //int fm = freeMemory();

    //_l("freeMemory"); dprint(freeMemory());


    //dprint("Setup message");

    // Message object with nodeid=999
    BERadioMessage *message = new BERadioMessage(999);

    // Enable debugging
    //message->debug(true);

    // Configure auto fragmentation by setting MTU size (maximum transfer unit)
    // The default is 61 bytes, suitable for sending via RFM69
    // FIXME: MTU size is currently limited to MTU_SIZE_MAX 61 in BERadio.h, improve!
    //message->set_mtu_size(96);

    message->dprint("Read sensors");

    // Collect some measurements en bloc
    FloatList temperature = collect(21.63, 19.25, 10.92, 13.54, 42.42);
    FloatList humidity    = collect(488.0, 572.0, 78.23);
    //FloatList weight      = collect(106.77, 99.22);
    //FloatList weight      = collect(106.77, 99.22, 234.4);

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
    FloatList radar;

    // Add measurement values to message
    message->dprint("Values to message");
    message->add("t", temperature);
    message->add("h", humidity);
    message->add("r", radar);
    message->add("w", *weight);

    // Yak shaving
    message->dprint("Transmit message");
    message->transmit();

    delete weight;
    delete message;

    return 0;

}


void BERadioMessage::send(char* buffer, int length) {

    /*
    This will yield valid payload fragments, e.g.:

    Sending payload: d1:#i88e1:_2:h11:hli48800ei57200ei7823ee1:tli2163ei1925eee
    Sending payload: d1:#i88e1:_2:h12:t2li1092ei1354ei4242ee1:wli10677ei9922eee
    Sending payload: d1:#i88e1:_2:h12:w2li23440eee

    The fragments are self-contained, so each one can be used on the
    receiver side even if a single fragment gets lost during transmission.

    */

    terrine.log("msg: ", false);
    terrine.log(buffer);

    //manager69.sendtoWait(payload.c_str(), payload.length(), RH69_TRANSCEIVER_ID);

    // C++ std::string
    //uint8_t *buf69 = new uint8_t[payload.length() + 1];
    //strcpy(buf69, payload.c_str());

    // C buffer
    // https://stackoverflow.com/questions/25360893/convert-char-to-uint8-t/25360996#25360996
    uint8_t *rh_buffer = new uint8_t[length + 1];
    memcpy(rh_buffer, (const char*)buffer, length);

    // Radio transmission (RadioHead)
    //managerTCP.sendtoWait(rh_buffer, length, RHTCP_GATEWAY_ID);

    // Clean up
    delete rh_buffer;
}

void BERadioMessage::send(std::string &payload) {

    // TODO: Also support this interface

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

    /*
    bool sendtoWait(uint8_t* buf, uint8_t len, uint8_t address) {
        //
    }
    */

}


void BERadioMessage::dprint(const char *message, bool newline) {
    terrine.log(message, newline);
}

void BERadioMessage::dprint(int value) {
    terrine.log(value);
}
