/*
   
   SQL over HTTP telemetry example.
 
   Demonstrate submitting measurement data to CrateDB using SQL INSERT queries.

   Note: You should not do that in a production system. Please use a message
   bus like MQTT, with a corresponding broker like Mosquitto, and a data
   historian like Kotori, in order to relay your telemetry messages properly.

-------------------------------------------------------------------------

   Copyright (C) 2015-2023  Andreas Motl <andreas.motl@panodata.org>

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

-------------------------------------------------------------------------

   Further information can be obtained at:

   The Hiveeyes Project         https://hiveeyes.org/
   Documentation                https://hiveeyes.org/docs/arduino/
   Source code repository       https://github.com/hiveeyes/arduino
   Community forum              https://community.hiveeyes.org/
                                https://community.panodata.org/


-------------------------------------------------------------------------   
*/


// ******************************************
//                  Modules
// ******************************************

// Libraries
// ---------
#include <TerkinTelemetry.h>
#include <Terrine.h>

// Terrine: Application boilerplate
Terrine terrine;


// Imports
// -------
using namespace Terkin;


// ******************************************
//                Main program
// ******************************************

// Include configuration settings.
#include "config.h"


// Forward declarations.
int main();
void loop();
void setup();
void setup_telemetry();
void setup_sensors();


// Declare telemetry client.
TelemetryClient* telemetry = NULL;


// Program entrypoint for glibc.
int main() {

    terrine.log("===============================");
    terrine.log("TerkinTelemetry CrateDB example");
    terrine.log("===============================");
    terrine.log();

    setup();
    loop();

}

// Program entrypoints for Arduino.

// Arduino main setup function.
void setup() {
    setup_sensors();
    setup_telemetry();
}

// Sensor setup.
void setup_sensors() {
    // Note: No sensors yet. This is just a telemetry demo program.
}

// TODO: Can this configuration style be improved?
void TerkinData::DataManager::setup() {
    this->field_names  = new DataHeader({"temperature", "humidity"});
}

// Telemetry setup with pluggable components.
void setup_telemetry() {

    // The address of the data acquisition channel.
    ChannelAddress* address = new ChannelAddress(CHANNEL_REALM, CHANNEL_OWNER, CHANNEL_SITE, CHANNEL_DEVICE);

    // The actual transmitter element.
    CrateDBTransmitter* transmitter = new CrateDBTransmitter(CRATEDB_URL, CRATEDB_USERNAME, CRATEDB_PASSWORD);

    // The telemetry client.
    telemetry = new TelemetryClient(transmitter, address);
}

// Arduino main loop function.
void loop() {

    // 1. Read sensors

    // Collect sensor data.
    // Note: No sensors yet. This is just a telemetry demo program.
    TerkinData::Measurement measurement;
    measurement.data["temperature"] = 42.42;
    measurement.data["humidity"] = 80;

    // 2. Transmit data

    // Transmit measurement to telemetry data collection server.
    telemetry->transmit(measurement);

    // Start sleeping cycle.
#if defined(ARDUINO)
    delay(2500);
#else
#endif

}
