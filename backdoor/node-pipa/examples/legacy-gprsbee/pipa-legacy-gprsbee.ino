/*
   
                 Hiveeyes Pipa Datalogger for GPRSbee
 
   Collect sensor data, serialize as JSON and send it through GPRSbee.

   Copyright (C) 2015-2016  Clemens Gruber <clemens@hiveeyes.org>
   Copyright (C) 2015-2016  Richard Pobering <richard@hiveeyes.org>
   Copyright (C) 2015-2021  Andreas Motl <andreas@hiveeyes.org>

   <https://hiveeyes.org>

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

   This is an Arduino sketch for the Hiveeyes bee monitoring system.
   The purpose is to collect vital data of a bee hive and transmit it
   to the data collection server.

   The sensor data could be temperature (DS18B20), humidity (BME280)
   or a load cell (H30A with HX711). Other sensors can easily be added.

   After the sensor data is collected, it gets serialized to JSON,
   which will be the transmitted in the body of a HTTP POST request.

   The creation of this code is strongly influenced by other projects,
   so credits goes to them.

   Feel free to adapt this code to your own needs.

-------------------------------------------------------------------------   

   Futher informations can be obtained at:

   hiveeyes                     https://hiveeyes.org/
   documentation                https://hiveeyes.org/docs/system/
   repository                   https://github.com/hiveeyes/
   open hive                    http://open-hive.org/


-------------------------------------------------------------------------   
*/


// ******************************************
//             User configuration
// ******************************************

// Communication
// =============

// Telemetry
// ---------
#define HE_API_URL      "http://swarm.hiveeyes.org/api-notls/"
#define HE_USER         "testdrive"
#define HE_SITE         "area-42"
#define HE_HIVE         "pipa-gprsbee-01"


// GPRSbee
// -------
#define USE_GPRSBEE         true
#define GPRSBEE_AP_NAME     "internet.eplus.de"     // https://en.wikipedia.org/wiki/Access_Point_Name
#define GPRSBEE_AP_AUTH     true
#define GPRSBEE_AP_USER     "testuser"
#define GPRSBEE_AP_PASS     "12345"

#define GPRSBEE_VCC     7    // 23
#define GPRSBEE_ONOFF   -1
#define GPRSBEE_STATUS  8


// Sensors
// =======

// TODO: No sensors yet. This is just a telemetry demo program.


// ******************************************
//           Developer configuration
// ******************************************

// Debugging
// ---------
/*
#define DEBUG
#define DEBUG_RADIO
#define DEBUG_FLASH
#define DEBUG_SENSORS
#define DEBUG_SERIAL
#define DEBUG_ENCODE
*/


// ******************************************
//                  Modules
// ******************************************


// Libraries
// ---------
#include <ArduinoJson.h>                // https://github.com/bblanchon/ArduinoJson
#if USE_GPRSBEE
#include <GPRSbee.h>                    // https://github.com/SodaqMoja/GPRSbee
#endif
#include <Terkin.h>                     // https://github.com/hiveeyes/arduino/tree/master/libraries/Terkin
#include <Hiveeyes.h>                   // https://github.com/hiveeyes/arduino/tree/master/libraries/Hiveeyes
#include <OpenHive.h>                   // https://github.com/hiveeyes/arduino/tree/master/libraries/OpenHive
#include <Wire.h>                       // Arduino



// Imports
// -------
using namespace Terkin;
using namespace Hiveeyes;
using namespace OpenHive;



// ******************************************
//                  Sensors
// ******************************************



// ******************************************
//                Main program
// ******************************************


// Main telemetry API
TelemetryNode *telemetry;

// Forward declarations
TelemetryNode& setup_telemetry_gprsbee(bool wait_usb);


// Setup function
void setup() {

    // Telemetry setup
#if USE_GPRSBEE
    telemetry = &setup_telemetry_gprsbee(true);
#endif

    // Sensor setup
    // FIXME: No sensors yet. This is just a telemetry demo program.

}

// Main loop
void loop() {


    // 1. Read sensors

    // Collect basic sensor data
    // TODO: No sensors yet. This is just a telemetry demo program.
    JsonObject& data_basic = openhive_sensors.read_demo();

    // Collect sensor data from temperature array
    // TODO: No sensors yet. This is just a telemetry demo program.
    JsonObject& data_temparray = openhive_temparray.read_demo();


    // 2. Transmit data

    // Transmit telemetry data to data collection server
    telemetry->transmit(data_basic);
    telemetry->transmit(data_temparray);

    // TODO: Start sleeping cycle, see "node-rfm69-beradio.ino"
    delay(5000);

}

/**
 *
 * Setup function for initializing a TelemetryNode instance
 * with appropriate components in individual setups.
 *
 * Here, the machinery is instructed to
 * communicate using a Sodaq GPRSbee device.
 *
**/
#if USE_GPRSBEE
TelemetryNode& setup_telemetry_gprsbee(bool wait_usb = false) {


    /* Setup transmitter hardware */

    // GPRS setup
    Serial.begin(19200);          // Serial is connected to SIM900 GPRSbee

    // Initialize GPRSbee device
    gprsbee.initAutonomoSIM800(Serial, GPRSBEE_VCC, GPRSBEE_ONOFF, GPRSBEE_STATUS);

    // Make sure the GPRSbee is switched off when starting up
    gprsbee.off();
    if (wait_usb) {
        Serial.println(F("Please disconnect USB within 5 seconds."));
        delay(5000);
    }


    /* Setup telemetry client with pluggable components */

    // Transmitter
    #if GPRSBEE_AP_AUTH
        GPRSbeeTransmitter transmitter(gprsbee, GPRSBEE_AP_NAME, GPRSBEE_AP_USER, GPRSBEE_AP_PASS);
    #else
        GPRSbeeTransmitter transmitter(gprsbee, GPRSBEE_AP_NAME);
    #endif

    // Telemetry manager
    TelemetryManager manager(transmitter, HE_API_URL);

    // Telemetry node
    HiveeyesNodeAddress address(HE_USER, HE_SITE, HE_HIVE);
    TelemetryNode node(manager, address);

    return node;

}
#endif
