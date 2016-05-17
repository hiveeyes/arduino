/*
   
                  Hiveeyes node-wifi-mqtt
 
   Code collects sensor data and sends via wifi to a mqtt bus. 
    
   Software release 0.5.1

   Copyright (C) 2014-2016  Clemens Gruber <mail@clemens-gruber.de>

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

   Hiveeyes node sketch for Arduino based platforms   

   This is a arduino sketch for the hiveeyes bee monitoring system.
   The purpose is to collect vital data and sends them via wifi to
   a mqtt bus at swarm.hiveeyes.org:1883.

   This code is derived from the ardafruit esp8266 sensor model.
   See below.

   The creation of this code is strongly influenced by other projects, so
   credits goes to 
   them: <https://hiveeyes.org/docs/beradio/README.html#credits> 

   Feel free to adapt this code to your own needs.

-------------------------------------------------------------------------   

   Futher informations can be obtained at:

   hiveeyes                     https://hiveeyes.org/
   documentation                https://swarm.hiveeyes.org/docs/
   repository                   https://github.com/hiveeyes/
   beradio                      https://hiveeyes.org/docs/beradio/

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

// DHTxx sensor
#include <dht.h>                        // https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib

// DHTxx pin(s) / humidity and temperature
// how much pins have DHTxx devices connected?
const int humidityNumDevices = 1;
// pins have DHTxx device, pin 6, pin 7
const int humidityPins[humidityNumDevices+1] = {4};

dht DHT;  // create DHT object
char gradC[4]={' ','°','C','\0'};  // degree sign

// HX711
#include "HX711.h"                      // https://github.com/bogde/HX711

// HX711.DOUT  - pin #14
// HX711.PD_SCK - pin #16
HX711 scale(14,12);   // parameter "gain" is ommited; the default value 128 is used by the library


// we need some variable as char to pass it to payload easily
char temperatureChar[humidityNumDevices][6];  // should handle ±xx.x and null terminator
char humidityChar[humidityNumDevices][6];  // should handle xxx.x and null terminator
int level;
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
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD);/****************************** Feeds ***************************************/

// Setup feeds for temperature & humidity
const char WEIGHT_FEED[] PROGMEM = AIO_USERNAME "hiveeyes/xx/test/esp8266-1/measure/weight";
Adafruit_MQTT_Publish weight = Adafruit_MQTT_Publish(&mqtt, WEIGHT_FEED);

const char TEMPERATURE_FEED[] PROGMEM = AIO_USERNAME "hiveeyes/xx/test/esp8266-1/measure/temperature";
Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, TEMPERATURE_FEED);

const char HUMIDITY_FEED[] PROGMEM = AIO_USERNAME "hiveeyes/xx/test/esp8266-1/measure/humidity";
Adafruit_MQTT_Publish humidity = Adafruit_MQTT_Publish(&mqtt, HUMIDITY_FEED);

const char BATTERY_FEED[] PROGMEM = AIO_USERNAME "hiveeyes/xx/test/esp8266-1/measure/battery";
Adafruit_MQTT_Publish battery = Adafruit_MQTT_Publish(&mqtt, BATTERY_FEED);
/*************************** Sketch Code ************************************/


void setup() {
  Serial.begin(9600);

  // scale
  scale.set_scale(-21383.f);  // this value is obtained by calibrating the scale with known weights; see the README for details
  delay(1000UL*5);
  scale.set_offset(40645);
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

  // connect to adafruit io
  connect();
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


void battery_level() {
  // read the battery level from the ESP8266 analog in pin.
  // analog read level is 10 bit 0-1023 (0V-1V).
  // our 1M & 220K voltage divider takes the max
  // lipo value of 4.2V and drops it to 0.758V max.
  // this means our min analog read value should be 580 (3.14V)
  // and the max analog read value should be 774 (4.2V).
  level = analogRead(A0);

  // convert battery level to percent
  level = map(level, 580, 774, 0, 100);
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
  getHumidityTemperature();
  battery_level();

  // Publish data
  if (! weight.publish(weightChar))
    Serial.println(F("Failed to publish weight"));
  else {
    Serial.print(F("Weight:     "));
    Serial.print(weightChar);
    Serial.print(F(" kg"));
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

  if (! battery.publish(level))
    Serial.println(F("Failed to publish battery"));
  else {
    Serial.print(F("Battery:      "));
    Serial.print(level);
    Serial.print(F(" %"));
    Serial.println(F("  -  published!"));
  };

  // Repeat every 10 seconds
//  delay(10000);

  // sleep
  ESP.deepSleep(1000000*60*1, WAKE_RF_DEFAULT); // Sleep for 1 sec * 60 sec * 1 min
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

