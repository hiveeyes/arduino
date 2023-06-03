/**
 *
 * TerkinTelemetry - a polyglot telemetry client interface.
 *
 * Copyright (C) 2015-2023  Andreas Motl <andreas.motl@panodata.org>
 * Copyright (C) 2015-2016  Richard Pobering <einsiedlerkrebs@ginnungagap.org>
 *
 * TerkinTelemetry C++ provides an universal interface to user-space Arduino code, in order
 * to generalize interfacing with a range of telemetry backends.
 *
 * TerkinTelemetry C++ provides backend drivers for supporting different telemetry communication
 * paths, similar to what RadioHead provides as adapters to different radio link hardware,
 * and TinyGSM provides as adapters to different modems. Examples: Generic JSON, CrateDB,
 * InfluxDB.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  See also "LICENSE.txt" in the root folder of
 *  this repository or distribution package.
 *
**/
#include <Terrine.h>
#include "TerkinTelemetry.h"

#if defined(__unix__) || defined(__APPLE__)
#include <iostream>
#endif


using namespace std;
using namespace Terkin;

// Terrine: Application boilerplate
Terrine terrine2;


// ==========================================
//          Telemetry infrastructure
// ==========================================

/**
 *
 * A ChannelAddress stores the topology address of a sensor device.
 * Sensor devices have a quadruple addressing scheme.
 *
 *   - Realm
 *   - Network
 *   - Gateway
 *   - Device
 *
 * All four address components are made of ASCII characters.
 *
 * Example:
 *
 *   acme-org / testdrive / area-42 / device-3
 *
**/

// Initialize with full quadruple address
ChannelAddress::ChannelAddress(const char *realm, const char *network, const char *gateway, const char *device)
    :
    realm(realm),
    network(network),
    gateway(gateway),
    device(device)
{}


/**
 *
 * The TelemetryClient is the main API the user interacts with.
 * It offers a single method "transmit" to call after reading the sensors.
 *
 * When creating an instance, it obtains a transmitter component, and a
 * channel address.
 *
**/

TelemetryClient::TelemetryClient(CrateDBTransmitter* transmitter, ChannelAddress* address) {
    _transmitter = transmitter;
    _address = address;
    transmitter_type = CRATEDB;
}
TelemetryClient::TelemetryClient(JsonHttpTransmitter* transmitter, ChannelAddress* address) {
    _transmitter = transmitter;
    _address = address;
    transmitter_type = JSON_HTTP;
}

/*
bool TelemetryClient::transmit(JsonObject& data) {
    terrine2.log("FIXME: Implement TelemetryClient::transmit(JsonObject& data)");

    //string uri = string(http_uri) + string(address_path);
    //TerkinData::Measurement measurement;
    //return transmitter->transmit(measurement);

    //return _manager->transmit(address_path.c_str(), data);
    TerkinData::Measurement data_out;
    return transmit(data_out);
}
*/

bool TelemetryClient::transmit(TerkinData::Measurement data) {
    // TODO: Use double-dispatch instead?
    // https://gieseanw.wordpress.com/2018/12/29/stop-reimplementing-the-virtual-table-and-start-using-double-dispatch/

    // JSON over HTTP.
    if (this->transmitter_type == JSON_HTTP) {
        string address_path =
                string(_address->realm) + string("/") +
                string(_address->network) + string("/") +
                string(_address->gateway) + string("/") +
                string(_address->device) + string("/") +
                string("data");
        return ((JsonHttpTransmitter*) _transmitter)->transmit(address_path.c_str(), data);

    // CrateDB over HTTP.
    } else if (this->transmitter_type == CRATEDB) {
        string database =
                string(_address->realm) + string("_") +
                string(_address->network);
        string table =
                string(_address->gateway) + string("_") +
                string(_address->device);
        return ((CrateDBTransmitter*) _transmitter)->transmit(database.c_str(), table.c_str(), data);
    }

    terrine2.log("ERROR: Unable to discover telemetry implementation");
}



// ==========================================
//               Transmitters
// ==========================================


/**
 *
 * GPRSbeeTransmitter is a transmitter component wrapping
 * the GPRSbeeClass of the Sodaq GSM modem driver.
 *
 * Serialize and transmit telemetry data
 * to data collection server using HTTP.
 *
 * Todo:
 *
 *   - Data acquisition by MQTT or FTP
 *   - Bulk uploads
 *
**/

#if USE_GPRSBEE

// Initialize without APN authentication
GPRSbeeTransmitter::GPRSbeeTransmitter(GPRSbeeClass& driver, const char *apn)
    :
    _driver(driver),
    _apn(apn),
    _authenticated(false)
{}

// Initialize with APN authentication
GPRSbeeTransmitter::GPRSbeeTransmitter(GPRSbeeClass& driver, const char *apn, const char *apnuser, const char *apnpwd)
    :
    _driver(driver),
    _apn(apn),
    _apnuser(apnuser),
    _apnpwd(apnpwd),
    _authenticated(true)
{}

// The transmit method obtains a reference to a JsonObject
// which will get serialized before handing the payload
// over to the appropriate driver component.
bool GPRSbeeTransmitter::transmit(const char *uri, JsonObject& data) {

    // Serialize data
    char payload[256];
    data.printTo(payload, sizeof(payload));

    // Transmit data
    // Derived from https://github.com/SodaqMoja/GPRSbee/wiki#do-a-http-post
    //const char testData[] = "testdata3 = hello world with newline\n";
    char response[50];
    memset(response, 0, sizeof(response));
    bool retval;
    if (!_authenticated) {
        // Without APN authentication
        retval = _driver.doHTTPPOSTWithReply(_apn, uri, payload, strlen(payload), response, sizeof(response));
    } else {
        // With APN authentication
        retval = _driver.doHTTPPOSTWithReply(_apn, _apnuser, _apnpwd, uri, payload, strlen(payload), response, sizeof(response));
    }

    // FIXME: Enable debugging
    /*
    if (retval) {
        print(F("Post result: "));
        print('"');
        print(buffer);
        println('"');
    } else {
        println(F("Post failed!"));
    }
    */

    return retval;

}
#endif


#if defined ARDUINO_ARCH_ESP8266 || defined ARDUINO_ARCH_ESP32

// https://github.com/esp8266/Arduino/blob/2.7.4/libraries/ESP8266HTTPClient/examples/BasicHttpClient/BasicHttpClient.ino#L43-L72

#if defined ARDUINO_ARCH_ESP8266
#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#elif defined ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#endif

void sendRequest(const char *uri) {
    WiFiClient client;

    HTTPClient http;

    Serial.print("[HTTP] begin...\n");
    if (http.begin(client, uri)) {


      Serial.print("[HTTP] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = http.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    } else {
      Serial.printf("[HTTP} Unable to connect\n");
    }
}


// The transmit method obtains a reference to a JsonObject
// which will get serialized before handing the payload
// over to the appropriate driver component.
bool ESPHTTPTransmitter::transmit(JsonObject& data) {

    // Serialize data
    char payload[256];
    data.printTo(payload, sizeof(payload));

    // Transmit data
    // Derived from https://github.com/SodaqMoja/GPRSbee/wiki#do-a-http-post
    //const char testData[] = "testdata3 = hello world with newline\n";
    char response[50];
    memset(response, 0, sizeof(response));
    bool retval;

    sendRequest(http_uri);

    /*
    if (!_authenticated) {
        // Without APN authentication
        //retval = _driver.doHTTPPOSTWithReply(_apn, http_uri, payload, strlen(payload), response, sizeof(response));
    } else {
        // With APN authentication
        //retval = _driver.doHTTPPOSTWithReply(_apn, _apnuser, _apnpwd, http_uri, payload, strlen(payload), response, sizeof(response));
    }
    */

    // FIXME: Enable debugging
    /*
    if (retval) {
        print(F("Post result: "));
        print('"');
        print(buffer);
        println('"');
    } else {
        println(F("Post failed!"));
    }
    */

    return retval;

}

bool ESPHTTPTransmitter::transmit(TerkinData::Measurement data) {
    terrine2.log("ESPHTTPTransmitter::transmit");
    return true;
}

#endif


JsonHttpTransmitter::JsonHttpTransmitter(const char *http_uri)
    :
    http_uri(http_uri)
{}


bool JsonHttpTransmitter::transmit(const char *path, TerkinData::Measurement data) {
    terrine2.log("JsonHttpTransmitter::transmit");
    terrine2.log((string("URI:  ") + string(http_uri)).c_str());
    terrine2.log(string("Path: ") + string(path));

    // TODO: Improve.
    TerkinData::DataManager *datamgr = new TerkinData::DataManager();
    std::string data_record = datamgr->json_data(data);
    delete datamgr;

    // Output
    terrine.log("Data: ", false);
    terrine.log(data_record.c_str());

    return true;
}

CrateDBTransmitter::CrateDBTransmitter(const char *url, const char *username, const char *password)
    :
    _url(url),
    _username(username),
    _password(password)
{}

// The transmit method obtains a reference to a JsonObject
// which will get serialized before handing the payload
// over to the appropriate driver component.
bool CrateDBTransmitter::transmit(const char *database, const char *table, TerkinData::Measurement data) {

    terrine.log("CrateDBTransmitter::transmit");
    terrine.log(string("Database: ") + string(database));
    terrine.log(string("Table:    ") + string(table));

    // TODO: Improve.
    TerkinData::DataManager *datamgr = new TerkinData::DataManager();
    std::string data_record = datamgr->sql_insert(table, data);
    delete datamgr;

    // Output
    terrine.log("Data:     ", false);
    terrine.log(data_record.c_str());

    return true;

}
