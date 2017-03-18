/***********************************************************************************************************
   ESP8266 Beescale
   (c)2014 - 2017 Alexander Wilms
   https://www.imker-nettetal.de

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


  Credits: Marvin Roger for the awesome Homie Framework: https://github.com/marvinroger/homie-esp8266

  Used libraries:
  HX711:		            https://github.com/bogde/HX711
  RunningMedian:	      https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian
  Homie-esp8266 v2.0.0:	https://github.com/marvinroger/homie-esp8266
  Bounce2:	          	https://github.com/thomasfredericks/Bounce2
  ESPAsyncTCP:	      	https://github.com/me-no-dev/ESPAsyncTCP
  async-mqtt-client:    https://github.com/marvinroger/async-mqtt-client

**********************************************************************************************************/

#include <Homie.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "HX711.h"
#include <RunningMedian.h>


HX711 scale;

const int DEFAULT_SEND_INTERVAL = 60;

/* Use sketch BeeScale-Calibration.ino to determine these calibration values.
   Set them here or use HomieSetting via config.json or WebApp/MQTT
*/
const float DEFAULT_WEIGHT_OFFSET = 244017.00; // Load cell zero offset. 
const float DEFAULT_KILOGRAM_DIVIDER = 22.27;  // Load cell value per kilogram.
unsigned long lastSent = 0;

RunningMedian WeightSamples = RunningMedian(4);
float weight;

ADC_MODE(ADC_VCC);
float volt;

HomieNode temperatureNode0("temperature0", "temperature");
HomieNode temperatureNode1("temperature1", "temperature");
HomieNode weightNode("weight", "weight");
HomieNode batteryNode("battery", "battery");
HomieNode jsonNode("data","__json__");

HomieSetting<long> sendIntervalSetting("sendInterval", "Interval for measurements in seconds (max. 3600)");
HomieSetting<double> weightOffsetSetting("weightOffset", "Offset value to zero. Use BeeScale-Calibration.ino to determine.");
HomieSetting<double> kilogramDividerSetting("kilogramDivider", "Scale value per kilogram. Use BeeScale-Calibration.ino to determine.");

void setupHandler() {
  temperatureNode0.setProperty("unit").send("C");
  temperatureNode1.setProperty("unit").send("C");
  weightNode.setProperty("unit").send("kg");
  batteryNode.setProperty("unit").send("V");
}

#define ONE_WIRE_BUS 14
// Setup a OneWire instance to communicate with any OneWire devices
OneWire OneWire(ONE_WIRE_BUS);
// Pass our OneWire reference to Dallas Temperature.
DallasTemperature sensors(&OneWire);
float temperature0;
float temperature1;

void getTemperatures() {
  sensors.begin();
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  delay(100);
  temperature0 = sensors.getTempCByIndex(0);
  if (temperature0 == -127) {                  //ignore wrong reading
    delay(100);
  temperature0 = sensors.getTempCByIndex(0);
  }
  temperature1 = sensors.getTempCByIndex(1);
  if (temperature1 == -127) {                  //ignore wrong reading
    delay(100);
  }
  temperature1 = sensors.getTempCByIndex(1);
}

void getWeight() {
  Serial.println("Getting weight, hold on for 4 seconds");
    
  for (int i = 0; i < 4; i++) {
    Serial.print("*");
    scale.power_up();
    float WeightRaw = scale.get_units(10);
    yield();
    scale.power_down();
    WeightSamples.add(WeightRaw);
    delay(1000);                 
    yield();
  }
  Serial.println("");
  weight = WeightSamples.getMedian() / 1000;
}

void loopHandler() {
  if (millis() - lastSent >= sendIntervalSetting.get() * 1000UL || lastSent == 0) {
    getTemperatures();
    Serial << "Temperature0: " << temperature0 << " °C" << endl;
    temperatureNode0.setProperty("degrees").setRetained(false).send(String(temperature0));

    Serial << "Temperature1: " << temperature1 << " °C" << endl;
    temperatureNode1.setProperty("degrees").setRetained(false).send(String(temperature1));

    getWeight();
    Serial << "Weight: " << weight << " kg" << endl;
    weightNode.setProperty("kilogram").setRetained(false).send(String(weight));

    volt = ESP.getVcc() / 1000 + 0.3; // ESP07 reads 0.3V too low
    Serial << "Input Voltage: " << volt << " V" << endl;
    batteryNode.setProperty("volt").setRetained(true).send(String(volt));

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["Weight"] = weight;
    root["Temp1"] = temperature0;
    root["Temp2"] = temperature1;
    String values;
    root.printTo(values);
    Serial << "Json data:" << values << endl;
    jsonNode.setProperty("__json__").setRetained(false).send(values);
    
    lastSent = millis();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Homie_setFirmware("node-wifi-mqtt-homie", "1.0.5");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  Homie.disableLedFeedback(); // LED pin would break serial on ESP-07

  
  sendIntervalSetting.setDefaultValue(DEFAULT_SEND_INTERVAL);
  weightOffsetSetting.setDefaultValue(DEFAULT_WEIGHT_OFFSET);
  kilogramDividerSetting.setDefaultValue(DEFAULT_KILOGRAM_DIVIDER);

  scale.begin(13, 12);
  scale.set_scale(kilogramDividerSetting.get());
  scale.set_offset(weightOffsetSetting.get());    

  temperatureNode0.advertise("unit");
  temperatureNode0.advertise("degrees");

  temperatureNode1.advertise("unit");
  temperatureNode1.advertise("degrees");

  weightNode.advertise("unit");
  weightNode.advertise("kilogram");

  batteryNode.advertise("unit");
  batteryNode.advertise("volt");
  
  Homie.setup();
}

void loop() {
  Homie.loop();
}
