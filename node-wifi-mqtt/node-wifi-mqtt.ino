/*

                  Hiveeyes node-wifi-mqtt

   Collect beehive sensor data and transmit via WiFi to a MQTT broker.

   Copyright (C) 2016-2017  The Hiveeyes Developers <hello@hiveeyes.org>


   Changes
   -------
   2016-05-18 Initial version
   2016-10-31 Beta release
   2017-01-09 Add more sensors
   2017-02-01 Serialize sensor readings en bloc using JSON
   2017-03-31 Fix JSON serialization: Transmit sensor readings as float values.
              Thanks, Matthias and Giuseppe!
   2017-04-05 Improve efficiency and flexibility
   2017-04-05 Enable connecting to multiple WiFi access points with multiple attempts
              Read and transmit battery level
              Thanks, Matthias and Clemens!
   2017-04-06 Read and transmit free heap memory


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

   Hiveeyes node sketch for Arduino based platforms. Here: ESP8266.

   This is a Arduino sketch for the Hiveeyes beehive monitoring system.
   The purpose is to collect vital data and to send it via WiFi to
   the MQTT bus at swarm.hiveeyes.org:1883.

   This code is derived from the Adafruit ESP8266 MCU demo sketch,
   see below.

   Feel free to adapt this code to your own needs. Contributions are welcome!

   -------------------------------------------------------------------------

   Further information can be obtained at:

   The Hiveeyes Project         https://hiveeyes.org/
   System documentation         https://hiveeyes.org/docs/system/
   Code repository              https://github.com/hiveeyes/arduino/

   -------------------------------------------------------------------------

*/

/***************************************************
  Adafruit ESP8266 Sensor Module

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino
  Works great with Adafruit's Huzzah ESP board:
  ----> https://www.adafruit.com/product/2471
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/



// =====================
// General configuration
// =====================

// Measure and transmit each five minutes
#define MEASUREMENT_INTERVAL    5 * 60 * 1000


// ==========================
// Connectivity configuration
// ==========================

// ----
// WiFi
// ----
#define WIFI_SSID_1     "wifi-ssid-1"
#define WIFI_PASS_1     "wifi-pass-1"
#define WIFI_SSID_2     "wifi-ssid-2"
#define WIFI_PASS_2     "wifi-pass-2"
#define WIFI_ATTEMPTS   15

// ----
// MQTT
// ----

// The address of the MQTT broker to connect to.
#define MQTT_BROKER     "swarm.hiveeyes.org"
#define MQTT_PORT       1883

// A MQTT client ID, which should be unique across multiple devices for a user.
// Maybe use your MQTT_USERNAME and the date and time the sketch was compiled
// or just use an UUID (https://www.uuidgenerator.net/) or other random value.
#define MQTT_CLIENT_ID  ""

// The credentials to authenticate with the MQTT broker.
#define MQTT_USERNAME   ""
#define MQTT_PASSWORD   ""

// The MQTT topic to transmit sensor readings to.
// Note that the "testdrive" channel is not authenticated and can be used anonymously.
// To publish to a protected data channel owned by you, please ask for appropriate
// credentials at https://community.hiveeyes.org/ or hello@hiveeyes.org.
#define MQTT_TOPIC      "hiveeyes/testdrive/area-42/node-1/data.json"


// ====================
// Sensor configuration
// ====================

// A dummy sensor publishing static values of "temperature", "humidity" and "weight".
// Please turn this off when working with the real sensors.
#define SENSOR_DUMMY            false

// The real sensors
#define SENSOR_HX711            true
#define SENSOR_DHTxx            true
#define SENSOR_DS18B20          true
#define SENSOR_BATTERY_LEVEL    true
#define SENSOR_MEMORY_FREE      true


// -------------------
// HX711: Scale sensor
// -------------------

// HX711.DOUT   - pin #14
// HX711.PD_SCK - pin #12
#define HX711_PIN_DOUT      14
#define HX711_PIN_PDSCK     12

// Define here individual values for the used load cell. This is not type specific!
// Even load cells of the same type / model have individual characteristics.
// To measure these values, please have a look at the firmwares for load cell adjustment:
// https://hiveeyes.org/docs/arduino/firmware/scale-adjust/README.html

// ZeroOffset is the raw sensor value for "0 kg".
// Write down the sensor value of the scale sensor with no load and adjust it here.
const long loadCellZeroOffset = 38623.0f;

// KgDivider is the raw sensor value for a 1 kg weight load.
// Add a load with known weight in kg to the scale sensor, note the
// sensor value, calculate the value for a 1 kg load and adjust it here.
// Note: Use value * 0.5 for asymmetric / single side measurement,
// so that 1 kg is displayed as 2 kg.
//const long loadCellKgDivider  = 22053;
const long loadCellKgDivider  = 11026;



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
const int ds18b20_onewire_pin = 5;



// =========
// Libraries
// =========

// ------------
// Connectivity
// ------------

// ESP8266: https://github.com/esp8266/Arduino
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

// JSON serializer
#include <ArduinoJson.h>

// Adafruit MQTT
// https://github.com/adafruit/Adafruit_MQTT_Library
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


// -------
// Sensors
// -------

// HX711 weight scale sensor
#if SENSOR_HX711
    #include "HX711.h"

    // Create HX711 object
    HX711 hx711_sensor;

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
bool wifi_connect();
void wifi_setup();
void mqtt_connect();
void setup_sensors();
void read_sensors();
void transmit_readings();


void setup() {

    // ...
    Serial.begin(9600);

    // Setup WiFi
    wifi_setup();

    // Connect to WiFi
    wifi_connect();

    // Setup all sensors
    setup_sensors();
}

void loop() {

    // Connect to MQTT broker
    mqtt_connect();

    // Read all sensors
    read_sensors();

    // Transmit all sensor readings
    transmit_readings();

    // Pause some time. After that, continue with the next measurement cycle.
    delay(MEASUREMENT_INTERVAL);

}

void setup_sensors() {

    // Setup scale sensor (single HX711)
    #if SENSOR_HX711
        hx711_sensor.begin(HX711_PIN_DOUT, HX711_PIN_PDSCK);

        // These values are obtained by calibrating the scale sensor with known weights; see the README for details
        hx711_sensor.set_scale(loadCellKgDivider);
        hx711_sensor.set_offset(loadCellZeroOffset);

        //  hx711_sensor.tare();             // reset the scale sensor to 0
        yield();
    #endif


    // Setup temperature array (multiple DS18B20 sensors)
    #if SENSOR_DS18B20

        ds18b20_sensor.begin();  // start DallasTemperature library
        ds18b20_sensor.setResolution(ds18b20_precision);  // set resolution of all devices

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
        //
        // method 1: by index
        // change index to order divices as in nature
        // (an other approach can be to order physical devices ascending to device address on cable)
        for (int i=0; i<ds18b20_device_count; i++) {
            if (!ds18b20_sensor.getAddress(ds18b20_addresses[ds18b20_device_order[i]], i)) {
                Serial.print(F("Unable to find address for temperature array device "));
                Serial.print(i);
                Serial.println();
            }
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

    Serial.println(F("Read temperature array (DS18B20)"));

    // Request temperature on all devices on the bus

    // Make it asynchronous
    ds18b20_sensor.setWaitForConversion(false);

    // Initiate temperature retrieval
    ds18b20_sensor.requestTemperatures();

    // Wait at least 750 ms for conversion
    delay(1000);

    // Iterate all DS18B20 devices and read temperature values
    for (int i=0; i<ds18b20_device_count; i++) {
        float temperatureC = ds18b20_sensor.getTempC(ds18b20_addresses[i]);
        ds18b20_temperature[i] = temperatureC;
    }

    #endif

}

// DHTxx: Humidity and temperature
void read_humidity_temperature() {

    #if SENSOR_DHTxx

    Serial.println(F("Read humidity and temperature (DHTxx)"));

    // Iterate all DHTxx devices
    for (int i=0; i<dht_device_count; i++) {

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
                Serial.println("DHT checksum error. Device #" + i);
                break;

            case DHTLIB_ERROR_TIMEOUT:
                Serial.println("DHT timeout error. Device #" + i);
                break;

            default:
                Serial.println("DHT unknown error. Device #" + i);
                break;
        }
    }

    // FOR UNO + DHT33 400ms SEEMS TO BE MINIMUM DELAY BETWEEN SENSOR READS,
    // ADJUST TO YOUR NEEDS
    // we do other time consuming things after reading DHT33, so we can skip this
    //  delay(1000);

    #endif

}


void read_weight() {

    #if SENSOR_HX711

    Serial.println(F("Read weight (HX711)"));

    hx711_sensor.power_up();
    weight = hx711_sensor.get_units(5);

    // Debugging
    Serial.println(weight);

    // Aftermath
    hx711_sensor.power_down();              // put the ADC in sleep mode
    yield();

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
    Serial.print("Battery ADC Level: ");
    Serial.println(adc_level);

    // Convert battery level to percentage
    battery_level = map(adc_level, 535, 759, 0, 100);
    Serial.print("Battery level: ");
    Serial.print(battery_level);
    Serial.println("%");

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
void transmit_readings() {

    // Build JSON object containing sensor readings
    StaticJsonBuffer<512> jsonBuffer;


    // Create telemetry payload by manually mapping sensor readings to telemetry field names.
    // Note: For more advanced use cases, please have a look at the TerkinData C++ library
    //       https://hiveeyes.org/docs/arduino/TerkinData/README.html
    JsonObject& json_data = jsonBuffer.createObject();

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

    #if SENSOR_DUMMY
    json_data["temperature"]                 = 42.42f;
    json_data["humidity"]                    = 84.84f;
    json_data["weight"]                      = 33.33f;
    #endif

    // Debugging
    json_data.printTo(Serial);


    // Serialize data
    int json_length = json_data.measureLength();
    char payload[json_length+1];
    json_data.printTo(payload, sizeof(payload));


    // Publish data
    if (mqtt_publisher.publish(payload)) {
        Serial.println(F("MQTT publish succeeded"));
    } else {
        Serial.println(F("MQTT publish failed"));
    }

}


// Setup WiFi
void wifi_setup() {

    // Add/remove entries as needed
    wifi_multi.addAP(WIFI_SSID_1, WIFI_PASS_1);
    wifi_multi.addAP(WIFI_SSID_2, WIFI_PASS_2);

}

// Connect to WiFi
bool wifi_connect() {

    // Debugging
    Serial.println(); Serial.println();
    delay(10);
    Serial.println("Connecting to WiFi");

    // Try connecting to WiFi
    for (int i = 0; i < WIFI_ATTEMPTS; i++) {

        // Wait for WiFi connection
        if ((wifi_multi.run() == WL_CONNECTED)) {

            // Debug WiFi
            Serial.println();
            Serial.print(F("WiFi connected! IP address: "));
            Serial.print(wifi_client.localIP());
            Serial.println();

            return true;

        // Not connected yet
        } else {

            Serial.print(".");

            // Wait 400 ms before retry
            delay(400);
        }
    }

    // Giving up on further connection attempts
    return false;

}

// Connect to MQTT broker
void mqtt_connect() {

    if (mqtt.connected()) {
        Serial.println(F("Already connected to MQTT"));
        return;
    }

    Serial.println(F("Connecting to MQTT..."));
    int8_t ret;

    uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0) {
        Serial.println(mqtt.connectErrorString(ret));

        Serial.println("Retrying MQTT connect in 5 seconds...");
        delay(5000);

        retries--;
        if (retries == 0) {
            Serial.println(F("Could not connect to MQTT, dying!"));
        }

    }
    Serial.println(F("Connected to MQTT!"));
}

