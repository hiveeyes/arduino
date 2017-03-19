/**
 *
 * TerkinTelemetry - a flexible telemetry client interface layer
 *
 *
 *  Copyright (C) 2015-2017  Andreas Motl <andreas.motl@elmyra.de>
 *  Copyright (C) 2015-2016  Richard Pobering <einsiedlerkrebs@ginnungagap.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  See also "LICENSE.txt" in the root folder of
 *  this repository or distribution package.
 *
 *
 * Terkin Arduino provides drivers for supporting different
 * communication paths, similar to what RadioHead provides
 * as adapters to different radio link hardware.
 *
 * For generalizing user-space Arduino code, Terkin provides
 * an universal interface to the communication subsystems
 * under the hood.
 *
**/
#include <Arduino.h>
#include "TerkinTelemetry.h"

using namespace TerkinTelemetry;


// ==========================================
//          Telemetry infrastructure
// ==========================================

/**
 *
 * A NodeAddress stores the topology address of a sensor node.
 * Sensor nodes have a quadruple addressing scheme:
 *
 *   - Realm
 *   - Network
 *   - Gateway
 *   - Node
 *
 * All four address components are made of ASCII characters.
 *
 * Example:
 *
 *   acme-org / testdrive / area-42 / node-3
 *
**/

// Initialize with full quadruple address
NodeAddress::NodeAddress(const char *realm, const char *network, const char *gateway, const char *node)
    :
    realm(realm),
    network(network),
    gateway(gateway),
    node(node)
{}


/**
 *
 * A TelemetryNode is the main API the user interacts with.
 * It offers a single method "transmit" to call after
 * reading the sensors.
 *
 * When creating an instance, a TelemetryManager
 * and a NodeAddress instance have to be supplied.
 *
**/

// Initialize with TelemetryManager and NodeAddress components
TelemetryNode::TelemetryNode(TelemetryManager& manager, NodeAddress& address)
    :
    _manager(manager),
    _address(address)
{}

// The transmit method on the TelemetryNode object builds the full
// address path from topology address segments before handing the
// data container over to the appropriate TelemetryManager component.
bool TelemetryNode::transmit(const char *payload) {
    String address_path =
        String(_address.realm) + String("/") +
        String(_address.network) + String("/") +
        String(_address.gateway) + String("/") +
        String(_address.node) + String("/") +
        String("data");
    return _manager.transmit(address_path.c_str(), payload);
}


/**
 *
 * The TelemetryManager obtains a transmitter component
 * on creation and offers a single method "transmit".
 * It gets consumed by a TelemetryNode and dispatches
 * calls to the transmitter instance.
 *
 * When creating an instance, a transmitter object,
 * a target URI resource and optionally an authentication token
 * have to be supplied.
 *
 * When aiming at HTTP, this would be the full URL to
 * your data collection API endpoint.
 *
**/
TelemetryManager::TelemetryManager(GenericTransmitter& transmitter, const char *http_uri)
    :
    transmitter(transmitter),
    http_uri(http_uri)
{}
TelemetryManager::TelemetryManager(GenericTransmitter& transmitter, const char *http_uri, const char *auth_token)
    :
    transmitter(transmitter),
    http_uri(http_uri),
    auth_token(auth_token)
{}
bool TelemetryManager::transmit(const char *address_path, const char *payload) {
    String uri = String(http_uri) + String(address_path);
    if (auth_token) {
        uri += String("?token=") + auth_token;
    }
    return transmitter.transmit(uri.c_str(), payload);
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

#if TELEMETRY_DEVICE_GPRSBEE

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

// The transmit method obtains the uri and payload to be
// transmitted using the appropriate driver component.
bool GPRSbeeTransmitter::transmit(const char *uri, const char *payload) {

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



/**
 *
 * ESPHTTPClientTransmitter is a transmitter component
 * wrapping the HTTPClient of the ESP8266 SDK.
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

#if TELEMETRY_DEVICE_ESP8266

// Initialize with Content-Type
ESPHTTPClientTransmitter::ESPHTTPClientTransmitter(const char *content_type)
    :
    _content_type(content_type)
{}

// The transmit method obtains the uri and payload to be
// transmitted using the appropriate driver component.
bool ESPHTTPClientTransmitter::transmit(const char *uri, const char *payload) {

    HTTPClient http;
    http.begin(uri);
    http.addHeader("Content-Type", _content_type);
    int httpCode = http.POST(payload);
    //http.writeToStream(&Serial);

    bool success = false;

    if (httpCode == 200) {
        success = true;
        #ifdef isDebug
            Serial.print("HTTP success: ");
            String payload = http.getString();
            Serial.println(payload);
        #endif
    } else {
        #ifdef isDebug
            Serial.print("HTTP failure: code=" + String(httpCode));
        #endif
    }

    http.end();

    return success;

}

#endif
