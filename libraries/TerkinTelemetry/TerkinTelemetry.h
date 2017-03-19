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
#ifndef TerkinTelemetry_h
#define TerkinTelemetry_h

#if TELEMETRY_DEVICE_GPRSBEE
#include <GPRSbee.h>                    // https://github.com/SodaqMoja/GPRSbee
#endif

#if TELEMETRY_DEVICE_ESP8266
#include <ESP8266HTTPClient.h>
#endif

namespace TerkinTelemetry {

    // Forward declarations
    class GenericTransmitter;
    class TelemetryManager;

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
    class NodeAddress {
        public:
            NodeAddress(const char *realm, const char *network, const char *gateway, const char *node);
            const char *realm;
            const char *network;
            const char *gateway;
            const char *node;
    };

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
    class TelemetryNode {
        public:
            TelemetryNode(TelemetryManager& manager, NodeAddress& address);
            bool transmit(const char *payload);
        protected:
            TelemetryManager& _manager;
            NodeAddress& _address;
    };

    /**
     *
     * The TelemetryManager obtains a transmitter component
     * on creation and offers a single method "transmit".
     * It gets consumed by a TelemetryNode and dispatches
     * calls to the transmitter instance.
     *
     * When creating an instance, a transmitter object
     * and a target URI resource have to be supplied.
     *
     * When aiming at HTTP, this would be the full URL to
     * your data collection API endpoint.
     *
    **/
    class TelemetryManager {
        public:
            TelemetryManager(GenericTransmitter& transmitter, const char *http_uri);
            TelemetryManager(GenericTransmitter& transmitter, const char *http_uri, const char *auth_token);
            bool transmit(const char *address_path, const char *payload);
        protected:
            GenericTransmitter& transmitter;
            const char *http_uri;
            const char *auth_token;
    };



    // ==========================================
    //               Transmitters
    // ==========================================


    /**
     *
     * Abstract class defining the
     * interface for all telemetry drivers.
     *
    **/
    class GenericTransmitter {
        public:
            virtual bool transmit(const char *uri, const char *payload);
    };

    /**
     *
     * Transmitter component wrapping the GPRSbeeClass of the Sodaq GSM modem driver.
     *
    **/
    class GPRSbeeTransmitter : public GenericTransmitter {
#if TELEMETRY_DEVICE_GPRSBEE
        public:
            GPRSbeeTransmitter(GPRSbeeClass& driver, const char *apn);
            GPRSbeeTransmitter(GPRSbeeClass& driver, const char *apn, const char *apnuser, const char *apnpwd);
            bool transmit(const char *uri, const char *payload);
        protected:
            GPRSbeeClass& _driver;
            const char *_apn;
            const char *_apnuser;
            const char *_apnpwd;
            bool _authenticated = false;
#endif
    };


    /**
     *
     * Transmitter component wrapping the HTTPClient of the ESP8266 SDK.
     *
    **/
    class ESPHTTPClientTransmitter : public GenericTransmitter {
        public:
            ESPHTTPClientTransmitter(const char *content_type);
            bool transmit(const char *uri, const char *payload);
        protected:
            const char *_content_type;
    };


}

#endif
