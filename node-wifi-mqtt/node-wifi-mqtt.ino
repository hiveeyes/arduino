/*

                             Hiveeyes node-wifi-mqtt

   Collect environmental sensor data and transmit it via WiFi to a MQTT broker.

   Copyright (C) 2016-2017  The Hiveeyes Developers <hello@hiveeyes.org>


   Changes
   -------
   2016-05-18 Initial version
   2016-10-31 Beta release
   2017-01-09 Add more sensors
   2017-02-01 Serialize sensor readings en bloc using JSON
   2017-03-31 Fix JSON serialization: Transmit sensor readings as float values.
              Thanks, Matthias and Giuseppe!
   2017-04-05 Improve efficiency and configuration flexibility.
              Enable connecting to multiple WiFi access points with multiple attempts.
              Read and transmit battery level.
              Thanks, Matthias and Clemens!
   2017-04-06 Read and transmit free heap memory.
              Further improve overall configurability and
              wifi_connect/mqtt_connect regarding retrying.
              Give operating system / watchdog timer more breath.
              Add deep sleep mode.
   2017-04-07 Fix IP address output. Thanks, Matthias!
              When SENSOR_DUMMY is enabled, don't use any real sensors. Thanks, Giuseppe!
              Add comment about connecting GPIO#16 to RST for waking up after deep sleep. Thanks, Giuseppe and Matthias!
              Add sensor ADS1231. Thanks, Clemens!
              Silence debug output by using SerialDebugger to reduce battery drain even more.


   GNU GPL v3 License
   ------------------
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

   Hiveeyes node firmware for ESP8266 based on Arduino Core.

   This is a basic firmware for the Hiveeyes beehive monitoring system.
   The purpose is to collect environmental data and to transmit it
   via WiFi to the MQTT broker at swarm.hiveeyes.org:1883.

   The received sensor readings will get visualized automatically at
   https://swarm.hiveeyes.org/grafana/.

   Feel free to adapt this code to your own needs. Contributions are welcome!

   The area of application of this firmware is manifold. We are happy to see
   it deployed in different scenarios than beehive monitoring. You might want
   to share your stories with us at hello@hiveeyes.org.

   Do you need a flexible backend system for data acquisition and graphing?
   Please have a look at https://getkotori.org/.

   -------------------------------------------------------------------------

   Further information can be obtained at:

   The Hiveeyes Project         https://hiveeyes.org/
   System documentation         https://hiveeyes.org/docs/system/
   Code repository              https://github.com/hiveeyes/arduino/

   Arduino Core for ESP8266     https://github.com/esp8266/Arduino
   Firmware documentation       https://hiveeyes.org/docs/arduino/firmware/node-wifi-mqtt/README.html

   -------------------------------------------------------------------------

   Confirmed to be working on:

   Adafruit Feather HUZZAH      https://www.adafruit.com/product/2471
   NodeMCU AMICA R2             http://www.electrodragon.com/product/nodemcu-lua-amica-r2-esp8266-wifi-board/

   -------------------------------------------------------------------------

   More information:

   Adafruit Feather HUZZAH Pinouts      https://learn.adafruit.com/adafruit-feather-huzzah-esp8266/pinouts
   NodeMCU pin definition               https://nodemcu.readthedocs.io/en/master/en/modules/gpio/
                                        http://crufti.com/content/images/2015/11/nodemcudevkit_v1-0_io.jpg

   -------------------------------------------------------------------------

*/



// =====================
// General configuration
// =====================

// How often to measure and transmit (seconds)
#define MEASUREMENT_INTERVAL    5 * 60

// Whether device should go to deep sleep after measurement.
// To enable wakeup of the MCU, please connect the GPIO#16 pin to the RST pin.
// On NodeMCU boards, the GPIO#16 pin is also labeled "D0".
#define DEEPSLEEP_ENABLED       false

// Enable debug printing to UART
#define DEBUG                   true



// ==========================
// Connectivity configuration
// ==========================

// ----
// WiFi
// ----

// How often to retry connecting to the WiFi network
#define WIFI_RETRY_COUNT    5

// How long to delay between WiFi (re)connection attempts (seconds)
#define WIFI_RETRY_DELAY    1.5f

// The WiFi network credentials for multiple access points
#define WIFI_SSID_1         "wifi-ssid-1"
#define WIFI_PASS_1         "wifi-pass-1"

// Add/remove entries as required
//#define WIFI_SSID_2         "wifi-ssid-2"
//#define WIFI_PASS_2         "wifi-pass-2"
//#define WIFI_SSID_3         "wifi-ssid-3"
//#define WIFI_PASS_3         "wifi-pass-3"


// ----
// MQTT
// ----

// How often to retry connecting to the MQTT broker
#define MQTT_RETRY_COUNT    5

// How long to delay between MQTT (re)connection attempts (seconds)
#define MQTT_RETRY_DELAY    1.5f

// The address of the MQTT broker to connect to
#define MQTT_BROKER         "swarm.hiveeyes.org"
#define MQTT_PORT           1883

// A MQTT client ID, which should be unique across multiple devices for a user.
// Maybe use your MQTT_USERNAME and the date and time the sketch was compiled
// or just use an UUID (https://www.uuidtools.com/) or other random value.
#define MQTT_CLIENT_ID      ""

// The credentials to authenticate with the MQTT broker
#define MQTT_USERNAME       ""
#define MQTT_PASSWORD       ""

// The MQTT topic to transmit sensor readings to.
// Note that the "testdrive" channel is not authenticated and can be used anonymously.
// To publish to a protected data channel owned by you, please ask for appropriate
// credentials at https://community.hiveeyes.org/ or hello@hiveeyes.org.
#define MQTT_TOPIC          "hiveeyes/testdrive/area-42/node-1/data.json"


// ====================
// Sensor configuration
// ====================

// A dummy sensor publishing static values of "temperature", "humidity" and "weight".
// Please turn this on when working without any sensor hardware.
#define SENSOR_DUMMY            false
// TODO: SENSOR_SINE

// The real environmental sensors
#if !SENSOR_DUMMY

// Weight scale sensors, choose one
#define SENSOR_HX711            true
#define SENSOR_ADS1231          false

// DHTxx Humidity/Temperature sensors
#define SENSOR_DHTxx            true

// 1-Wire DallasTemperature sensors
#define SENSOR_DS18B20          true

#endif

// Vital hardware data sensors
#define SENSOR_BATTERY_LEVEL    true
#define SENSOR_MEMORY_FREE      true


// -------------------
// HX711: Scale sensor
// -------------------
#define HX711_PIN_PDSCK             12
#define HX711_PIN_DOUT              14


// ---------------------
// ADS1231: Scale sensor
// ---------------------
#define ADS1231_PIN_DATA            14
#define ADS1231_PIN_SCL             15
#define ADS1231_PIN_POWER           16
#define ADS1231_PIN_CELL_POWER      17

// How often to attempt reading the ADS1231
#define ADS1231_RETRY_COUNT         10


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



// -----------------------------
// DHTxx: Humidity / Temperature
// -----------------------------

// Number of DHTxx devices
const int dht_device_count = 1;

// DHTxx device pins: Pin 2, Pin 4
const int dht_pins[dht_device_count] = {2};

// For more DHTxx devices, configure their pins like...
// DHTxx device pins: Pin 2, Pin 4
//const int dht_pins[dht_device_count] = {2,4};


// --------------------------
// DS18B20: Temperature array
// --------------------------

// Number of temperature devices on the 1-Wire bus
const int ds18b20_device_count = 1;

// Order of the physical array of temperature devices,
// the order is normally defined by the device id hardcoded in
// the device.
// You can physically arrange the DS18B20 in case you
// know the ID and use here {1,2,3,4 ...} or you can try out what
// sensor is on which position and adjust it here accordingly.
const int ds18b20_device_order[ds18b20_device_count] = {0};

// For more DS18B20 devices, configure them like...
//const int ds18b20_device_order[ds18b20_device_count] = {0,1};

// Resolution for all devices (9, 10, 11 or 12 bits)
const int ds18b20_precision = 12;

// 1-Wire pin for the temperature sensors
// Note: Pin 5 corresponds to the physical pin D1 of ESP8266 MCUs
const int ds18b20_onewire_pin = 5;



// =========
// Libraries
// =========

// -------
// General
// -------
// http://playground.arduino.cc/Code/SerialDebugger
// https://github.com/hiveeyes/arduino/tree/node-wifi-mqtt-silent/libraries/SerialDebugger
#include <SerialDebug.h>


// ---------
// Telemetry
// ---------

// ESP8266 WiFi
// https://github.com/esp8266/Arduino
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

// Adafruit MQTT
// https://github.com/adafruit/Adafruit_MQTT_Library
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// JSON serializer
// https://github.com/bblanchon/ArduinoJson
#include <ArduinoJson.h>


// -------
// Sensors
// -------

// HX711 weight scale sensor
#if SENSOR_HX711

    // HX711 library
    #include "HX711.h"

    // Create HX711 object
    HX711 hx711_sensor;

    // Intermediate data structure for reading the sensor values
    float weight;

#endif


// ADS1231 weight scale sensor
#if SENSOR_ADS1231

    // ADS1231 library
    #include <ADS1231.h>

    // Create ADS1231 object
    ADS1231 ads1231_sensor;

    // Intermediate data structure for reading the sensor values
    float weight;

#endif


// DHTxx humidity/temperature sensor
#if SENSOR_DHTxx
    #include <dht.h>

    // Create DHT object
    dht dht_sensor;

    // Intermediate data structure for reading the sensor values
    float dht_temperature[dht_device_count];
    float dht_humidity[dht_device_count];

#endif

// DS18B20 1-Wire temperature sensor
#if SENSOR_DS18B20

    // 1-Wire library
    #include <OneWire.h>

    // DS18B20/DallasTemperature library
    #include <DallasTemperature.h>

    // For communicating with any 1-Wire device (not just DS18B20)
    OneWire oneWire(ds18b20_onewire_pin);

    // Initialize DallasTemperature library with reference to 1-Wire object
    DallasTemperature ds18b20_sensor(&oneWire);

    // Arrays for device addresses
    uint8_t ds18b20_addresses[ds18b20_device_count][8];

    // Intermediate data structure for reading the sensor values
    float ds18b20_temperature[ds18b20_device_count];

#endif

// Battery level
#if SENSOR_BATTERY_LEVEL
    int battery_level;
#endif

// Free heap memory
#if SENSOR_MEMORY_FREE
    int memory_free;
#endif



// ===============
// Telemetry setup
// ===============

// Create an ESP8266 WiFiClient object to connect to the MQTT server
WiFiClient wifi_client;

// Enable connecting to multiple WiFi access points to operate
// device in different environments without reconfiguration
ESP8266WiFiMulti wifi_multi;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details
Adafruit_MQTT_Client mqtt(&wifi_client, MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);

// Setup MQTT publishing handler
Adafruit_MQTT_Publish mqtt_publisher = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPIC);



// ============
// Main program
// ============

// --------------------
// Forward declarations
// --------------------
void measure();
bool wifi_connect();
void wifi_setup();
bool mqtt_connect();
void setup_sensors();
void read_sensors();
void transmit_readings();


void setup() {

    // ...
    Serial.begin(9600);

    if (DEBUG) {

        // If this line is commented out, the binary sketch size will decrease by 332 bytes
        SerialDebugger.begin(9600);

        // Select which UART to use for debugging
        //SerialDebugger.begin(2, 9600);

        SerialDebugger.enable(INFO);
        SerialDebugger.enable(WARNING);
        SerialDebugger.enable(ERROR);

    }

    // Setup WiFi
    wifi_setup();

    // Setup all sensors
    setup_sensors();

    #if DEEPSLEEP_ENABLED

        // Measurement and telemetry
        measure();

        // Go to deep sleep mode
        // http://www.esp8266.com/wiki/doku.php?id=esp8266_power_usage
        // http://www.esp8266.com/viewtopic.php?f=32&t=12901
        if (SerialDebugger.debug(INFO, "duty", "Going to deep sleep mode for")) {
            SerialDebugger.print(MEASUREMENT_INTERVAL).print("seconds");
        }
        ESP.deepSleep(MEASUREMENT_INTERVAL * 1000UL * 1000UL, WAKE_RF_DEFAULT);

    #endif

}

void loop() {

    #if !DEEPSLEEP_ENABLED

        // Measurement and telemetry
        measure();

        // Pause some time. After that, continue with the next measurement cycle.
        if (SerialDebugger.debug(INFO, "duty", "Delaying main loop for")) {
            SerialDebugger.print(MEASUREMENT_INTERVAL).print("seconds");
        }
        delay(MEASUREMENT_INTERVAL * 1000UL);

    #endif

}

void measure() {

    // Connect to WiFi
    if (!wifi_connect()) {
        return;
    }

    // Connect to MQTT broker
    if (!mqtt_connect()) {
        return;
    }

    // Read all sensors
    read_sensors();

    // Transmit all sensor readings
    transmit_readings();

}

void setup_sensors() {

    // Setup scale sensor (single HX711)
    #if SENSOR_HX711

        // Initialize the hardware driver with the appropriate pins
        hx711_sensor.begin(HX711_PIN_DOUT, HX711_PIN_PDSCK);

        // These values are obtained by calibrating the scale sensor with known weights.
        // See the README for further details and - for obtaining them:
        // https://hiveeyes.org/docs/arduino/firmware/scale-adjust/README.html
        hx711_sensor.set_scale(LOADCELL_KG_DIVIDER);
        hx711_sensor.set_offset(LOADCELL_ZERO_OFFSET);

        // Reset the scale sensor to 0
        //hx711_sensor.tare();

        // Give operating system / watchdog timer some breath
        yield();

    #endif


    // Setup scale sensor (single ADS1231)
    #if SENSOR_ADS1231

        // ADS1231 data pins: SCL 15, Data 14, Power 16
        ads1231_sensor.attach(ADS1231_PIN_SCL, ADS1231_PIN_DATA, ADS1231_PIN_POWER);

        // Load cell power pin
        pinMode(ADS1231_PIN_CELL_POWER, OUTPUT);

        // Give operating system / watchdog timer some breath
        yield();

    #endif


    // Setup temperature array (multiple DS18B20 sensors)
    #if SENSOR_DS18B20

        // Start DallasTemperature library
        ds18b20_sensor.begin();

        // Set resolution for all devices
        ds18b20_sensor.setResolution(ds18b20_precision);

        // Assign address manually. The addresses below will need to be changed
        // to valid device addresses on your bus. Device addresses can be retrieved
        // by using either oneWire.search(deviceAddress) or individually via
        // ds18b20_sensor.getAddress(deviceAddress, index)
        //insideThermometer    = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
        //outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

        // Search for devices on the bus and assign based on an index. Ideally,
        // you would do this to initially discover addresses on the bus and then
        // use those addresses and manually assign them (see above) once you know
        // the devices on your bus (and assuming they don't change).

        // method 1: by index
        // change index to order divices as in nature
        // (an other approach can be to order physical devices ascending to device address on cable)
        for (int i=0; i < ds18b20_device_count; i++) {

            // Get address of single device
            if (!ds18b20_sensor.getAddress(ds18b20_addresses[ds18b20_device_order[i]], i)) {
                if (SerialDebugger.debug(WARNING, "setup_sensors", "Unable to find address for temperature array device")) {
                    SerialDebugger.print("Device #" + i);
                }
            }

            // Give operating system / watchdog timer some breath
            yield();

        }

        // method 2: search()
        // search() looks for the next device. Returns 1 if a new address has been
        // returned. A zero might mean that the bus is shorted, there are no devices,
        // or you have already retrieved all of them. It might be a good idea to
        // check the CRC to make sure you didn't get garbage. The order is
        // deterministic. You will always get the same devices in the same order
        //
        // Must be called before search()
        //oneWire.reset_search();
        // assigns the first address found to insideThermometer
        //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
        // assigns the seconds address found to outsideThermometer
        //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

    #endif

}



// DS18B20: Temperature array
void read_temperature_array() {

    #if SENSOR_DS18B20

    SerialDebugger.debug(INFO, "read_temperature_array", "Read temperature array (DS18B20)");

    // Request temperature from all devices on the bus

    // Make it asynchronous
    ds18b20_sensor.setWaitForConversion(false);

    // Initiate temperature retrieval
    ds18b20_sensor.requestTemperatures();

    // Wait at least 750 ms for conversion
    delay(1000);

    // Iterate all DS18B20 devices and read temperature values
    for (int i=0; i < ds18b20_device_count; i++) {

        // Read single device
        float temperatureC = ds18b20_sensor.getTempC(ds18b20_addresses[i]);
        ds18b20_temperature[i] = temperatureC;

        // Give operating system / watchdog timer some breath
        yield();

    }

    #endif

}

// DHTxx: Humidity and temperature
void read_humidity_temperature() {

    #if SENSOR_DHTxx

    SerialDebugger.debug(INFO, "read_humidity_temperature", "Read humidity and temperature (DHTxx)");

    // Iterate all DHTxx devices
    for (int i=0; i < dht_device_count; i++) {

        // Read single device
        int chk = dht_sensor.read33(dht_pins[i]);

        switch (chk) {

            // Reading the sensor worked, let's get the values
            case DHTLIB_OK:
                dht_temperature[i]  = dht_sensor.temperature;
                dht_humidity[i]     = dht_sensor.humidity;
                break;

            // Print debug messages when errors occur

            case DHTLIB_ERROR_CHECKSUM:
                if (SerialDebugger.debug(ERROR, "read_humidity_temperature", "DHT checksum error.")) {
                    SerialDebugger.print("Device #" + i);
                }
                break;

            case DHTLIB_ERROR_TIMEOUT:
                if (SerialDebugger.debug(ERROR, "read_humidity_temperature", "DHT timeout error.")) {
                    SerialDebugger.print("Device #" + i);
                }
                break;

            default:
                if (SerialDebugger.debug(ERROR, "read_humidity_temperature", "DHT unknown error.")) {
                    SerialDebugger.print("Device #" + i);
                }
                break;
        }

        // Give operating system / watchdog timer some breath
        yield();

    }

    #endif

}


void read_weight() {

    // TODO: Maybe refactor to generic weight reading library to reduce code footprint in main program

    // HX711 weight scale sensor
    #if SENSOR_HX711

        SerialDebugger.debug(INFO, "read_weight", "Read weight (HX711)");

        hx711_sensor.power_up();
        weight = hx711_sensor.get_units(5);

        // Debugging
        if (SerialDebugger.debug(INFO, "read_weight", "Read weight (HX711)")) {
            SerialDebugger.print("Weight: ").print(weight);
        }

        // Put the ADC to sleep mode
        hx711_sensor.power_down();

        // Give operating system / watchdog timer some breath
        yield();

    #endif


    // ADS1231 weight scale sensor
    #if SENSOR_ADS1231

        SerialDebugger.debug(INFO, "read_weight", "Read weight (ADS1231)");

        // Power ADS1231 and load cell
        digitalWrite(ADS1231_PIN_CELL_POWER, HIGH);
        ads1231_sensor.power(HIGH);

        // Wait for stabilizing
        delay(2);

        // Wait for ADS1231 being ready
        bool sensor_ready = false;
        for (int i = 0; i < ADS1231_RETRY_COUNT; i++) {
            if (ads1231_sensor.check()) {
                sensor_ready = true;
                break;
            }
            // Give operating system / watchdog timer some breath
            yield();
        }

        // Sensor isn't ready to read values, abort!
        if (!sensor_ready) {
            SerialDebugger.debug(ERROR, "read_weight", "ADS1231 not ready");
            // TODO: Introduce another magic value for signalling invalid readings?
            weight = -1;
            return;
        }

        // Read raw data input of ADS1231
        long weightSensorValue = ads1231_sensor.readData();

        // Switch off ADS1231 and load cell
        ads1231_sensor.power(LOW);
        digitalWrite(ADS1231_PIN_CELL_POWER, LOW);

        // Compute weight in kg
        weight = ((float) weightSensorValue - (float) LOADCELL_ZERO_OFFSET) / (float) LOADCELL_KG_DIVIDER;

    #endif

}

void read_battery_level() {

    #if SENSOR_BATTERY_LEVEL

    // Read the battery level from the ESP8266 analog input pin.
    // Analog read level is 10 bit 0-1023 (0V-1V).
    // Our 1M & 220K voltage divider takes the max
    // LiPo value of 4.2V and drops it to 0.758V max.
    // This means our minimum analog read value should be 580 (3.14V)
    // and the maximum analog read value should be 774 (4.2V).
    int adc_level = analogRead(A0);
    if (SerialDebugger.debug(INFO, "read_battery_level", "Battery ADC: ")) {
        SerialDebugger.print(adc_level);
    }

    // Convert battery level to percentage
    battery_level = map(adc_level, 535, 759, 0, 100);
    if (SerialDebugger.debug(INFO, "read_battery_level", "Battery level: ")) {
        SerialDebugger.print(battery_level).print("%");
    }

    // Give operating system / watchdog timer some breath
    yield();

    #endif
}

void read_memory_free() {
    #if SENSOR_MEMORY_FREE
    memory_free = ESP.getFreeHeap();
    #endif
}

// Read all sensors in sequence
void read_sensors() {
    read_weight();
    read_temperature_array();
    read_humidity_temperature();
    read_battery_level();
    read_memory_free();
}

// Telemetry: Transmit all readings by publishing them to the MQTT bus serialized as JSON
// TODO: Refactor by using TerkinData
void transmit_readings() {

    // Build JSON object containing sensor readings
    // TODO: How many data points actually fit into this?
    StaticJsonBuffer<512> jsonBuffer;


    // Create telemetry payload by manually mapping sensor readings to telemetry field names.
    // Note: For more advanced use cases, please have a look at the TerkinData C++ library
    //       https://hiveeyes.org/docs/arduino/TerkinData/README.html
    JsonObject& json_data = jsonBuffer.createObject();


    #if SENSOR_DUMMY
    json_data["temperature"]                 = 42.42f;
    json_data["humidity"]                    = 84.84f;
    json_data["weight"]                      = 33.33f;
    #endif


    #if SENSOR_HX711
    json_data["weight"]                      = weight;
    #endif

    #if SENSOR_DHTxx
    json_data["airtemperature"]              = dht_temperature[0];
    json_data["airhumidity"]                 = dht_humidity[0];
    if (dht_device_count >= 2) {
        json_data["airtemperature_outside"]  = dht_temperature[1];
        json_data["airhumidity_outside"]     = dht_humidity[1];
    }
    #endif

    #if SENSOR_DS18B20
    json_data["broodtemperature"]            = ds18b20_temperature[0];
    if (ds18b20_device_count >= 2) {
        json_data["entrytemperature"]        = ds18b20_temperature[1];
    }
    #endif

    #if SENSOR_BATTERY_LEVEL
    json_data["battery_level"]               = battery_level;
    #endif

    #if SENSOR_MEMORY_FREE
    json_data["memory_free"]                 = memory_free;
    #endif


    // Debugging
    json_data.printTo(Serial);


    // Serialize data
    int json_length = json_data.measureLength();
    char payload[json_length+1];
    json_data.printTo(payload, sizeof(payload));


    // Publish data
    // TODO: Refactor to TerkinTelemetry
    if (mqtt_publisher.publish(payload)) {
        SerialDebugger.debug(INFO, "transmit_readings", "MQTT publish succeeded");
    } else {
        SerialDebugger.debug(ERROR, "transmit_readings", "MQTT publish failed");
    }

}


// Setup WiFi
void wifi_setup() {

    // Add first WiFi access point
    wifi_multi.addAP(WIFI_SSID_1, WIFI_PASS_1);

    // Add more WiFi access points as required
    #if defined(WIFI_SSID_2) && defined(WIFI_PASS_2)
    wifi_multi.addAP(WIFI_SSID_2, WIFI_PASS_2);
    #endif

    #if defined(WIFI_SSID_3) && defined(WIFI_PASS_3)
    wifi_multi.addAP(WIFI_SSID_3, WIFI_PASS_3);
    #endif

}

// Connect to WiFi
// TODO: Refactor to TerkinTelemetry
bool wifi_connect() {

    // If already connected, don't do anything and signal success
    if (WiFi.status() == WL_CONNECTED) {
        return true;
    }

    // Debugging
    SerialDebugger.debug(INFO, "wifi_connect", "Connecting to WiFi");

    // Try connecting to WiFi
    for (int i = 0; i < WIFI_RETRY_COUNT; i++) {

        // Wait for WiFi connection
        if ((wifi_multi.run() == WL_CONNECTED)) {

            // Debug WiFi
            if (SerialDebugger.debug(INFO, "wifi_connect", "WiFi connected. IP address:")) {
                SerialDebugger.print(WiFi.localIP().toString().c_str());
            }
            return true;

        // Not connected yet
        } else {

            if (SerialDebugger.debug(INFO, "wifi_connect", "Checking for WiFi connection in")) {
                SerialDebugger.print(WIFI_RETRY_DELAY).print("seconds");
            }

            // Wait some time before retrying
            delay(WIFI_RETRY_DELAY * 1000);
        }
    }

    // Giving up on further connection attempts
    return false;

}

// Connect to MQTT broker
// TODO: Refactor to TerkinTelemetry
bool mqtt_connect() {

    // If already connected, don't do anything and signal success
    if (mqtt.connected()) {
        return true;
    }

    SerialDebugger.debug(INFO, "mqtt_connect", "Connecting to MQTT broker");

    // Reconnect loop
    uint8_t retries = MQTT_RETRY_COUNT;
    int8_t ret;
    while ((ret = mqtt.connect()) != 0) {

        if (SerialDebugger.debug(WARNING, "mqtt_connect", "Error connecting to MQTT broker:")) {
            SerialDebugger.print(String(mqtt.connectErrorString(ret)).c_str());
        }

        retries--;
        if (retries == 0) {
            SerialDebugger.debug(ERROR, "mqtt_connect", "Giving up connecting to MQTT broker");
            return false;
        }

        if (SerialDebugger.debug(INFO, "mqtt_connect", "Retrying MQTT connection in")) {
            SerialDebugger.print(MQTT_RETRY_DELAY).print("seconds");
        }

        // Wait some time before retrying
        delay(MQTT_RETRY_DELAY * 1000);
    }

    if (mqtt.connected()) {
        if (SerialDebugger.debug(INFO, "mqtt_connect", "Successfully connected to MQTT broker")) {
            SerialDebugger.print(MQTT_BROKER);
        }
        return true;
    }

    // Giving up on further connection attempts
    return false;
}

