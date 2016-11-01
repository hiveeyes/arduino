/*

                  Hiveeyes node-wifi-mqtt

   Collect beehive sensor data and transmit via WiFi to a MQTT broker.

   Copyright (C) 2014-2016  Clemens Gruber


   Changes
   -------
   2016-05-18 Initial version
   2016-10-31 Beta release


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

   This is a Arduino sketch for the Hiveeyes bee monitoring system.
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

// Libraries
// ESP8266

#include <ESP8266WiFi.h>                // https://github.com/esp8266/Arduino

// WiFi parameters
#define WLAN_SSID  "change-to-your-ssid"
#define WLAN_PASS  "change-to-your-pw"

// Adafruit MQTT
#include "Adafruit_MQTT.h"              // https://github.com/adafruit/Adafruit_MQTT_Library
#include "Adafruit_MQTT_Client.h"       // https://github.com/adafruit/Adafruit_MQTT_Library

// Adafruit IO
#define AIO_SERVER      "swarm.hiveeyes.org"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    ""
#define AIO_KEY         ""

// DS18B20
#include <OneWire.h>            // oneWire for DS18B20
#include <DallasTemperature.h>  // DS18B20 itself

// number of temperature devices on bus
const int temperatureNumDevices = 1;
// order on physical array of temperature devices
// the order is normally defined by the device id hardcoded in
// the device, you can physically arrange the DS18B20 in case you
// know the ID and use here {1,2,3,4 ... or you can tryp out what
// sensor is on wich position and adusit it herer accordingly
const int temperatureOrderDevices[temperatureNumDevices] = {0};
// resolution for all devices (9, 10, 11 or 12 bits)
const int temperaturePrecision = 12;
// pin for the temperature sensors
const int temperaturePin = 5;

OneWire oneWire(temperaturePin);                       // oneWire instance to communicate with any OneWire devices (not just DS18B20)
DallasTemperature temperatureSensors(&oneWire);        // pass oneWire reference to DallasTemperature

uint8_t deviceAddressArray[temperatureNumDevices][8];  // arrays for device addresses
char gradC[4]={' ','°','C','\0'};                      // degree sign


// DHTxx sensor
#include <dht.h>

// DHTxx pin(s) / humidity and temperature
// how much pins have DHTxx devices connected?
const int humidityNumDevices = 1;
// pins have DHTxx device, pin 4
const int humidityPins[humidityNumDevices+1] = {4};

dht DHT;  // create DHT object

// HX711
#include "HX711.h"

// HX711.DOUT  - pin #14
// HX711.PD_SCK - pin #12
HX711 scale(14,12);   // parameter "gain" is ommited; the default value 128 is used by the library


// we need some variable as char to pass it to payload easily
char temperatureArrayChar[temperatureNumDevices][6];  // should handle +/-xx.x and null terminator
char temperatureChar[humidityNumDevices][6];  // should handle +/-xx.x and null terminator
char humidityChar[humidityNumDevices][6];  // should handle xxx.x and null terminator
char weightChar[9];  // should handle +-xxx.xxx and null terminator

// Functions
void connect();

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;

// Store the MQTT server, client ID, username, and password in flash memory.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;

// Set a unique MQTT client ID using the AIO key + the date and time the sketch
// was compiled (so this should be unique across multiple devices for a user,
// alternatively you can manually set this to a GUID or other random value).
const char MQTT_CLIENTID[] PROGMEM  = AIO_KEY __DATE__ __TIME__;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Setup feeds for temperature & humidity
const char WEIGHT_FEED[] PROGMEM = AIO_USERNAME "hiveeyes/kh/dach/huzzah/measure/weight";
Adafruit_MQTT_Publish weight = Adafruit_MQTT_Publish(&mqtt, WEIGHT_FEED);

const char TEMPERATURE_FEED[] PROGMEM = AIO_USERNAME "hiveeyes/kh/dach/huzzah/measure/temperature";
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, TEMPERATURE_FEED);

const char HUMIDITY_FEED[] PROGMEM = AIO_USERNAME "hiveeyes/kh/dach/huzzah/measure/humidity";
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, HUMIDITY_FEED);

const char TEMPERATUREARRAY_FEED[] PROGMEM = AIO_USERNAME "hiveeyes/kh/dach/huzzah/measure/temperaturearray";
Adafruit_MQTT_Publish temperaturearray = Adafruit_MQTT_Publish(&mqtt, TEMPERATUREARRAY_FEED);


/*************************** Sketch Code ************************************/
void setup() {
  Serial.begin(9600);

  // scale
  scale.set_scale(-25144.f);  // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.set_offset(30775);
//  scale.tare();             // reset the scale to 0
  yield();

  // Connect to WiFi access point.
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


  // temperature array
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


  // connect to adafruit io
  connect();
}


// temperature array / DS18B20
void getTemperature() {
  // request temperature on all devices on the bus
  temperatureSensors.setWaitForConversion(false);  // makes it async
  // initiatie temperature retrieval
  temperatureSensors.requestTemperatures();

  // wait at least 750 ms for conversion
  delay(1000);

  // loop through each device, print out temperature
  for(int i=0; i<temperatureNumDevices; i++) {
    // print temperature
    float temperatureC = temperatureSensors.getTempC(deviceAddressArray[i]);
    dtostrf(temperatureC, 5, 1, temperatureArrayChar[i]);  // write to char array
  }
}

// humidity and temperature, DHTxx
void getHumidityTemperature() {
  // read humidity and temperature data
  // loop through all devices
  for(int i=0; i<humidityNumDevices; i++) {
    // read device
    int chk = DHT.read33(humidityPins[i]);

    switch (chk) {
      // this is the normal case, all is working smoothly
      case DHTLIB_OK:
        //Serial.print("OK,\t");
        dtostrf(DHT.temperature, 5, 1, temperatureChar[i]);  // write to char array
        dtostrf(DHT.humidity, 5, 1, humidityChar[i]);  // write to char array
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
}


void getWeight() {
  scale.power_up();
  float weight = scale.get_units(5);
  dtostrf(weight, 8, 3, weightChar);  // write to char array
  Serial.println(weight);
  Serial.println(weightChar);
  scale.power_down();              // put the ADC in sleep mode
  yield();
}


void loop() {
  // ping adafruit io a few times to make sure we remain connected
  if(! mqtt.ping(3)) {
    // reconnect to adafruit io
    if(! mqtt.connected())
      connect();
  }

  // Grab the current state of the sensor
//  int humidity_data = (int)dht.readHumidity();
//  int temperature_data = (int)dht.readTemperature();

  // output humidity and temperature, DHTxx
  getWeight();
  getTemperature();
  getHumidityTemperature();

  // Publish data
  if (! weight.publish(weightChar))
    Serial.println(F("Failed to publish weight"));
  else {
    Serial.print(F("Weight:     "));
    Serial.print(weightChar);
    Serial.print(F(" kg"));
    Serial.println(F("  -  published!"));
  };

  // note: only for a single DS18B20 on the bus!!
  // must be rewritten for more DS18B20
  if (! temperaturearray.publish(temperatureArrayChar[0]))
    Serial.println(F("Failed to publish temperatureArray[0]"));
  else {
    Serial.print(F("temperatureArray: "));
    Serial.print(temperatureArrayChar[0]);
    Serial.print(F(" "));
    Serial.print(gradC);
    Serial.println(F("  -  published!"));
  };

  if (! temperature.publish(temperatureChar[0]))
    Serial.println(F("Failed to publish temperature"));
  else {
    Serial.print(F("Temperature: "));
    Serial.print(temperatureChar[0]);
    Serial.print(F(" "));
    Serial.print(gradC);
    Serial.println(F("  -  published!"));
  };

  if (! humidity.publish(humidityChar[0]))
    Serial.println(F("Failed to publish humidity"));
  else {
    Serial.print(F("Humidity:    "));
    Serial.print(humidityChar[0]);
    Serial.print(F(" %"));
    Serial.println(F("  -  published!"));
  };


  // Repeat every 30 seconds
  delay(30000);
}


// connect to adafruit io via MQTT
void connect() {
  Serial.print(F("Connecting to Adafruit IO... "));
  int8_t ret;

  while ((ret = mqtt.connect()) != 0) {
    switch (ret) {
      case 1: Serial.println(F("Wrong protocol")); break;
      case 2: Serial.println(F("ID rejected")); break;
      case 3: Serial.println(F("Server unavail")); break;
      case 4: Serial.println(F("Bad user/pass")); break;
      case 5: Serial.println(F("Not authed")); break;
      case 6: Serial.println(F("Failed to subscribe")); break;
      default: Serial.println(F("Connection failed")); break;
    }

    if(ret >= 0)
      mqtt.disconnect();

    Serial.println(F("Retrying connection..."));
    delay(5000);
  }
  Serial.println(F("Adafruit IO Connected!"));
}
