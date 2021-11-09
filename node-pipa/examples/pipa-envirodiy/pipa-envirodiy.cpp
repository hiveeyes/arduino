/*
   
                       Hiveeyes Pipa Datalogger
 
   A software spike to probe sensor data with EnviroDIY's ModularSensors
   framework, collect and transmit using TerkinData and TerkinTelemetry.

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


// ==========================
// Connectivity configuration
// ==========================

// Telemetry
// ---------
#define HE_API_URL      "http://swarm.hiveeyes.org/api-notls/"
#define HE_USER         "testdrive"
#define HE_SITE         "area-42"
#define HE_HIVE         "pipa-envirodiy-01"



// ====================
// Sensor configuration
// ====================

#define SENSOR_BME280   true
#define SENSOR_HX711    true

// --------------------------
// BME280: Temperature sensor
// --------------------------
// The BME280 can be addressed either as 0x77 (Adafruit default) or
// 0x76 (Grove default). Either can be physically modified for the other
// address.
#define BME280_PIN_POWER    -1
#define BME280_I2C_ADDRESS  0x77

// -------------------
// HX711: Scale sensor
// -------------------
#define HX711_PIN_DOUT  14 // DT
#define HX711_PIN_PDSCK 12 // SCK


// ---------------
// General - scale
// ---------------

// Properly using the load cell requires definition of individual configuration values.
// This is not just specific to the *type* of the load cell as even
// load cells of the *same* type / model have individual characteristics.

// To measure these values, please have a look at the firmwares for load cell adjustment:
// https://hiveeyes.org/docs/arduino/firmware/scale-adjust/README.html


// This is the raw sensor value for "0 kg".
// Write down the sensor value of the scale sensor with no load and adjust it here.
#define LOADCELL_ZERO_OFFSET    38623.0f

// This is the raw sensor value for a load weighing exactly 1 kg.
// Add a load with known weight in kg to the scale sensor, note the
// sensor value, calculate the value for a 1 kg load and adjust it here.
#define LOADCELL_KG_DIVIDER     22053

// Note: Use value * 0.5 for asymmetric / single side measurement,
// so that 1 kg is displayed as 2 kg.
//#define LOADCELL_KG_DIVIDER     11026


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

//Variable* variableList_at1min[] = {new AOSongAM2315_Humidity(&am2315),
//                                   new AOSongAM2315_Temp(&am2315)};

#if SENSOR_BME280

// Example sensor from EnviroDIY ModularSensors.
// https://github.com/EnviroDIY/ModularSensors
#include <sensors/BoschBME280.h>

// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove
// default). Either can be physically modified for the other address.
const int8_t BME280Power = BME280_PIN_POWER;  // Power pin (-1 if unconnected)
uint8_t      BMEi2c_addr = BME280_I2C_ADDRESS;

// Create a Bosch BME280 sensor object.
BoschBME280 bme280(BME280Power, BMEi2c_addr);

// Create four variable pointers for the BME280.
Variable* bme280Humid =
        new BoschBME280_Humidity(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
Variable* bme280Temp =
        new BoschBME280_Temp(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
Variable* bme280Press =
        new BoschBME280_Pressure(&bme280, "12345678-abcd-1234-ef00-1234567890ab");
Variable* bme280Alt =
        new BoschBME280_Altitude(&bme280, "12345678-abcd-1234-ef00-1234567890ab");

#endif


#if SENSOR_HX711

// HX711 implementation for "EnviroDIY ModularSensors".
// https://github.com/EnviroDIY/ModularSensors
#include <AviaSemiHX711.h>

// The BME280 can be addressed either as 0x77 (Adafruit default) or 0x76 (Grove
// default). Either can be physically modified for the other address.
//const int8_t BME280Power = -1;  // Power pin (-1 if unconnected)
//uint8_t      BMEi2c_addr = 0x76;

// Create a Bosch BME280 sensor object.
AviaSemiHX711 hx711(HX711_PIN_PDSCK, HX711_PIN_DOUT, LOADCELL_ZERO_OFFSET, LOADCELL_KG_DIVIDER);

// Create a single variable pointer for the HX711.
Variable* hx711Weight =
    new AviaSemiHX711_Weight(&hx711, "23456789-abcd-1234-ef00-1234567890ab");

#endif



// ******************************************
//                Main program
// ******************************************


// Main telemetry API
TelemetryNode *telemetry;

// Forward declarations
TelemetryNode& setup_telemetry_esphttp_json();


// Setup function
void setup() {

    // Telemetry setup
#if USE_ESPHTTP_JSON
    telemetry = &setup_telemetry_esphttp_json();
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

    // Read some EnviroDIY sensors.
    bme280.update();
    hx711.update();


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
 * communicate using WiFi on an Espressif device.
 *
**/
#if USE_ESPHTTP_JSON
TelemetryNode& setup_telemetry_esphttp_json() {

    /* Setup telemetry client with pluggable components */

    // Transmitter
    ESPHTTPTransmitter transmitter;

    // Telemetry manager
    TelemetryManager manager(transmitter, HE_API_URL);

    // Telemetry node
    HiveeyesNodeAddress address(HE_USER, HE_SITE, HE_HIVE);
    TelemetryNode node(manager, address);

    return node;

}
#endif
