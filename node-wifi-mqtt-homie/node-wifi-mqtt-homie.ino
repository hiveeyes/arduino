/***********************************************************************************************************
   ESP8266 Beescale
   (c)2014 - 2016 Alexander Wilms
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
  HX711:              https://github.com/bogde/HX711
  RunningMedian:      https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian
  Homie-esp8266:      https://github.com/marvinroger/homie-esp8266
  Bounce2:            https://github.com/thomasfredericks/Bounce2
  ESPAsyncTCP:        https://github.com/me-no-dev/ESPAsyncTCP
  async-mqtt-client:  https://github.com/marvinroger/async-mqtt-client

**********************************************************************************************************/

#include <Homie.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "HX711.h"
#include <RunningMedian.h>

// Use sketch BeeScale-Calibration to get these values
const float offset = 85107.00;  // Offset load cell
const float cell_divider = 22.27; // Load cell divider

HX711 scale(13, 12);    // 13 DOUT, 12 PD_SCK


const int DEFAULT_SEND_INTERVAL = 60;
unsigned long lastSent = 0;

RunningMedian WeightSamples = RunningMedian(4);
float weight;

ADC_MODE(ADC_VCC);
float volt;

HomieNode temperatureNode0("temperature0", "temperature");
HomieNode temperatureNode1("temperature1", "temperature");
HomieNode weightNode("weight", "weight");
HomieNode batteryNode("battery", "battery");

HomieSetting<unsigned long> sendIntervalSetting("sendInterval", "Interval for measurements in seconds");

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
  scale.set_scale(cell_divider);  //initialize scale
  scale.set_offset(offset);       //initialize scale
  Serial.println("Getting weight, hold on");
    
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
    
    lastSent = millis();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Homie_setFirmware("Bienen", "1.0.11");
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

  temperatureNode0.advertise("unit");
  temperatureNode0.advertise("degrees");

  temperatureNode1.advertise("unit");
  temperatureNode1.advertise("degrees");

  weightNode.advertise("unit");
  weightNode.advertise("kilogram");

  batteryNode.advertise("unit");
  batteryNode.advertise("volt");

  Homie.disableLedFeedback(); // before Homie.setup()

  sendIntervalSetting.setDefaultValue(DEFAULT_SEND_INTERVAL);
  
  Homie.setup();
}

void loop() {
  Homie.loop();
}
