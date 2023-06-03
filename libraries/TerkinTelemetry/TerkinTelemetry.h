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

#ifndef TerkinTelemetry_h
#define TerkinTelemetry_h

#include <ArduinoJson.h>                // https://github.com/bblanchon/ArduinoJson
#if USE_GPRSBEE
#include <GPRSbee.h>                    // https://github.com/SodaqMoja/GPRSbee
#endif

#include <TerkinData.h>


namespace Terkin {

    // Forward declarations: Infrastructure.
    class ChannelAddress;
    class TelemetryClient;

    // Forward declarations: Transmitters.
    class CrateDBTransmitter;
    class JsonHttpTransmitter;


    // ==========================================
    //          Telemetry infrastructure
    // ==========================================

    /**
     *
     * A ChannelAddress stores the topology address of a sensor device.
     * Sensor devices use a quadruple addressing scheme in WAN scenarios.
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
    class ChannelAddress {
        public:
            ChannelAddress(const char *realm, const char *network, const char *gateway, const char *device);
            const char *realm;
            const char *network;
            const char *gateway;
            const char *device;
    };


    /**
     *
     * The TelemetryClient is the main API the user interacts with.
     * It offers a single method "transmit" to call after reading the sensors.
     *
     * When creating an instance, it obtains a transmitter component, and a
     * channel address.
     *
    **/

    enum TransmitterType {
        DUMMY = 0,
        CRATEDB,
        JSON_HTTP
    };

    class TelemetryClient {
        public:
            TelemetryClient(CrateDBTransmitter* transmitter, ChannelAddress* address);
            TelemetryClient(JsonHttpTransmitter* transmitter, ChannelAddress* address);
            bool transmit(JsonObject& data);
            bool transmit(TerkinData::Measurement data);
            // void transmit(EmBencode& data);
        protected:
            void* _transmitter;
            ChannelAddress* _address;
            TransmitterType transmitter_type;
    };



    // ==========================================
    //               Transmitters
    // ==========================================

    /**
     * Transmitters are adapters for specific telemetry drivers.
     *
     * They know how to serialize outbound data and actually
     * submit outbound telemetry data.
     *
     * TODO: For wiring the GPRSbee, we currently use JSON,
     *       let's also aim at BERadio.
    **/

    class CrateDBTransmitter {
    public:
        CrateDBTransmitter(const char *url, const char *username, const char *password);
        ~CrateDBTransmitter() = default;
        bool transmit(const char *database, const char *table, JsonObject& data);
        bool transmit(const char *database, const char *table, TerkinData::Measurement data);
    protected:
        const char *_url;
        const char *_username;
        const char *_password;
    };

    class JsonHttpTransmitter {
    public:
        JsonHttpTransmitter(const char *http_uri);
        ~JsonHttpTransmitter() = default;
        //bool transmit(JsonObject& data);
        bool transmit(const char *path, TerkinData::Measurement data);
    protected:
        const char *http_uri;
    };

    class ESPHTTPTransmitter : public JsonHttpTransmitter {
    public:
        bool transmit(JsonObject& data);
        bool transmit(TerkinData::Measurement data);
    };

    /**
     *
     * Transmitter component wrapping the GPRSbeeClass of the Sodaq GSM modem driver.
     *
    **/
#if USE_GPRSBEE
    class GPRSbeeTransmitter : public JsonHttpTransmitter {
        public:
            GPRSbeeTransmitter(GPRSbeeClass& driver, const char *apn);
            GPRSbeeTransmitter(GPRSbeeClass& driver, const char *apn, const char *apnuser, const char *apnpwd);
            bool transmit(const char *uri, JsonObject& data);
        protected:
            GPRSbeeClass& _driver;
            const char *_apn;
            const char *_apnuser;
            const char *_apnpwd;
            bool _authenticated = false;
    };
#endif

}

#endif
