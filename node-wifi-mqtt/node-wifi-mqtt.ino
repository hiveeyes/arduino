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
   2017-03-31 Fix JSON serialization: Transmit sensor readings as float values. Thanks, Matthias and Giuseppe!


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
#define WLAN_SSID       "change-to-your-ssid"
#define WLAN_PASS       "change-to-your-pw"

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
#define HAS_DUMMY_SENSOR   true

// The real sensors
#define HAS_HX711   false
#define HAS_DHT33   false
#define HAS_DS18B20 false


// -----
// Scale
// -----

// HX711.DOUT  - pin #14
// HX711.PD_SCK - pin #12
#define HX711_PIN_DOUT      14
#define HX711_PIN_PDSCK     12

// Define here individual values for the used load cell. This is not type specific!
// Even load cells of the same type / model have individual characteristics
//
// - ZeroOffset is the raw sensor value for "0 kg"
//   write down the sensor value of the scale with no load and
//   adjust it here
//
// - KgDivider is the raw sensor value for a 1 kg weight load
//   add a load with known weight in kg to the scale, note the
//   sesor value, calculate the value for a 1 kg load and adjust
//   it here

const long loadCellZeroOffset = 38623.0f;

// 1/2 value for single side measurement, so that 1 kg is displayed as 2 kg
//const long loadCellKgDivider  = 22053;
const long loadCellKgDivider  = 11026;



// -----------
// Temperature
// -----------

// number of temperature devices on bus
const int temperatureNumDevices = 2;
// order on physical array of temperature devices
// the order is normally defined by the device id hardcoded in
// the device, you can physically arrange the DS18B20 in case you
// know the ID and use here {1,2,3,4 ... or you can tryp out what
// sensor is on wich position and adusit it herer accordingly
const int temperatureOrderDevices[temperatureNumDevices] = {0,1};
// resolution for all devices (9, 10, 11 or 12 bits)
const int temperaturePrecision = 12;
// pin for the temperature sensors
const int temperaturePin = 5;



// =========
// Libraries
// =========

// ------------
// Connectivity
// ------------

// ESP8266: https://github.com/esp8266/Arduino
#include <ESP8266WiFi.h>

// JSON serializer
#include <ArduinoJson.h>

// Adafruit MQTT
// https://github.com/adafruit/Adafruit_MQTT_Library
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


// -------
// Sensors
// -------

#if HAS_HX711
    #include "HX711.h"
    HX711 scale;  // create HX711 object

    // Intermediate data structure for reading the sensor values
    float weight;

#endif

#if HAS_DHT33
    // DHTxx sensor
    #include <dht.h>

    dht DHT;  // create DHT object

    // DHTxx pin(s) / humidity and temperature
    // how much pins have DHTxx devices connected?
    const int humidityNumDevices = 2;
    // pins have DHTxx device, pin 4
    const int humidityPins[humidityNumDevices+1] = {2,4};

    // Intermediate data structure for reading the sensor values
    float dht_temperature[humidityNumDevices];
    float dht_humidity[humidityNumDevices];

#endif

// DS18B20
#if HAS_DS18B20
    #include <OneWire.h>            // oneWire for DS18B20
    #include <DallasTemperature.h>  // DS18B20 itself

    OneWire oneWire(temperaturePin);                       // oneWire instance to communicate with any OneWire devices (not just DS18B20)
    DallasTemperature temperatureSensors(&oneWire);        // pass oneWire reference to DallasTemperature

    uint8_t deviceAddressArray[temperatureNumDevices][8];  // arrays for device addresses

    // Intermediate data structure for reading the sensor values
    float ds18b20_temperature[temperatureNumDevices];

#endif


// ===============
// Telemetry setup
// ===============

// Functions
void mqtt_connect();

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;


// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_BROKER, MQTT_PORT, MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);

// Setup MQTT publishing handler
Adafruit_MQTT_Publish mqtt_publisher = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPIC);


// ============
// Main program
// ============

void setup() {
    Serial.begin(9600);

    // Connect to WiFi
    Serial.println(); Serial.println();
    delay(10);
    Serial.print(F("Connecting to "));
    Serial.println(WLAN_SSID);

    WiFi.begin(WLAN_SSID, WLAN_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(F("."));
    }
    Serial.println();

    Serial.println(F("WiFi connected"));
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
    Serial.println();


    // Setup scale
    #if HAS_HX711
        scale.begin(HX711_PIN_DOUT, HX711_PIN_PDSCK);

        // These values are obtained by calibrating the scale with known weights; see the README for details
        scale.set_scale(loadCellKgDivider);
        scale.set_offset(loadCellZeroOffset);

        //  scale.tare();             // reset the scale to 0
        yield();
    #endif


    // temperature array
    #if HAS_DS18B20

        temperatureSensors.begin();  // start DallasTemperature library
        temperatureSensors.setResolution(temperaturePrecision);  // set resolution of all devices

        // Assign address manually. The addresses below will need to be changed
        // to valid device addresses on your bus. Device addresses can be retrieved
        // by using either oneWire.search(deviceAddress) or individually via
        // temperatureSensors.getAddress(deviceAddress, index)
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
        for (int i=0; i<temperatureNumDevices; i++) {
            if (!temperatureSensors.getAddress(deviceAddressArray[temperatureOrderDevices[i]], i)) {
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


// temperature array / DS18B20
void getTemperature() {

    #if HAS_DS18B20

    // request temperature on all devices on the bus
    temperatureSensors.setWaitForConversion(false);  // makes it async
    // initiatie temperature retrieval
    temperatureSensors.requestTemperatures();

    // wait at least 750 ms for conversion
    delay(1000);

    // Iterate all DS18B20 devices and read temperature values
    for (int i=0; i<temperatureNumDevices; i++) {
        float temperatureC = temperatureSensors.getTempC(deviceAddressArray[i]);
        ds18b20_temperature[i] = temperatureC;
    }

    #endif

}

// humidity and temperature, DHTxx
void getHumidityTemperature() {

    #if HAS_DHT33

    // read humidity and temperature data
    // loop through all devices
    for (int i=0; i<humidityNumDevices; i++) {
        // read device
        int chk = DHT.read33(humidityPins[i]);

        switch (chk) {
            // this is the normal case, all is working smoothly
            case DHTLIB_OK:
                //Serial.print("OK,\t");
                dht_temperature[i]  = DHT.temperature;
                dht_humidity[i]     = DHT.humidity;
                break;

            // following are error cases
            case DHTLIB_ERROR_CHECKSUM:
                Serial.println("Checksum error");
                break;

            case DHTLIB_ERROR_TIMEOUT:
                Serial.println("Time out error");
                break;

            default:
                Serial.println("Unknown error");
                break;
        }
    }

    // FOR UNO + DHT33 400ms SEEMS TO BE MINIMUM DELAY BETWEEN SENSOR READS,
    // ADJUST TO YOUR NEEDS
    // we do other time consuming things after reading DHT33, so we can skip this
    //  delay(1000);

    #endif

}


void getWeight() {

    #if HAS_HX711

    scale.power_up();
    weight = scale.get_units(5);

    // Debugging
    Serial.println(weight);

    // Aftermath
    scale.power_down();              // put the ADC in sleep mode
    yield();

    #endif

}


void loop() {

    mqtt_connect();

    // Grab the current state of the sensor
    //  int humidity_data = (int)dht.readHumidity();
    //  int temperature_data = (int)dht.readTemperature();

    // note: only for a single DS18B20 on the bus!!
    // must be rewritten for more DS18B20


    // output humidity and temperature, DHTxx
    Serial.println(F("Read weight"));
    getWeight();
    Serial.println(F("Read temp"));
    getTemperature();
    Serial.println(F("Read hum temp"));
    getHumidityTemperature();


    // Build JSON object containing sensor readings
    StaticJsonBuffer<512> jsonBuffer;


    // Create telemetry payload by mapping sensor readings to telemetry field names
    // Note: For more advanced use cases, please have a look at the TerkinData C++ library
    //       https://hiveeyes.org/docs/arduino/TerkinData/README.html
    JsonObject& root = jsonBuffer.createObject();

    #if HAS_HX711
    root["weight"]                    = weight;
    #endif

    #if HAS_DS18B20
    root["broodtemperature"]          = ds18b20_temperature[0];
    root["entrytemperature"]          = ds18b20_temperature[1];
    #endif

    #if HAS_DHT33
    root["airtemperature"]            = dht_temperature[0];
    root["airhumidity"]               = dht_humidity[0];
    root["airtemperature_outside"]    = dht_temperature[1];
    root["airhumidity_outside"]       = dht_humidity[1];
    #endif

    #if HAS_DUMMY_SENSOR
    root["temperature"]               = 42.42f;
    root["humidity"]                  = 84.84f;
    root["weight"]                    = 33.33f;
    #endif

    // Debugging
    root.printTo(Serial);


    // Serialize data
    int json_length = root.measureLength();
    char payload[json_length+1];
    root.printTo(payload, sizeof(payload));


    // Publish data
    if (mqtt_publisher.publish(payload)) {
        Serial.println(F("MQTT publish succeeded"));
    } else {
        Serial.println(F("MQTT publish failed"));
    }


    // Wait for the next measurement interval
    delay(MEASUREMENT_INTERVAL);

}


// Connect to MQTT
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
