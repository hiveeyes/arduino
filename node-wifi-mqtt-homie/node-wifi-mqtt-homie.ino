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

#define FW_NAME "node-wifi-mqtt-homie"
#define FW_VERSION "0.9.5"
const int DEFAULT_SEND_INTERVAL = 60;

//Workaround for https://github.com/bblanchon/ArduinoJson/issues/566
#define ARDUINOJSON_USE_DOUBLE 0
//function to round to two decimals
float round2two(float tmp) {
  return ((int)(tmp * 100)) / 100.00;
}

// PINs
#define ONE_WIRE_BUS 14
#define DOUT  13
#define PD_SCK  12

/* Use sketch BeeScale-Calibration.ino to determine these calibration values.
   Set them here or use HomieSetting via config.json or WebApp/MQTT
*/
const float DEFAULT_WEIGHT_OFFSET = 244017.00; // Load cell zero offset.
const float DEFAULT_KILOGRAM_DIVIDER = 22.27;  // Load cell value per kilogram.
const float DEFAULT_CALIBRATION_TEMPERATURE = 20.0; // Temperature at which the scale has been calibrated for Temperature compensation
const float DEFAULT_CALIBRATION_FACTOR_GRAM_DEGREE = 0.0; // Calibration factor in gram per degree
unsigned long lastSent = 0;

RunningMedian WeightSamples = RunningMedian(4);

float weight;
float temperature0;
float temperature1;
float voltage;
float raw_voltage;
float vcc_adjust;
int sendInterval;


ADC_MODE(ADC_VCC);

// Setup a OneWire instance to communicate with any OneWire devices
OneWire OneWire(ONE_WIRE_BUS);
// Pass our OneWire reference to Dallas Temperature.
DallasTemperature sensors(&OneWire);


HomieNode temperatureNode0("temperature0", "temperature");
HomieNode temperatureNode1("temperature1", "temperature");
HomieNode weightNode("weight", "weight");
HomieNode batteryNode("battery", "battery");
HomieNode jsonNode("data","__json__");

HomieSetting<long> sendIntervalSetting("sendInterval", "Interval for measurements in seconds (max. 3600)");
HomieSetting<long> weightOffsetSetting("weightOffset", "Offset value to zero. Use BeeScale-Calibration.ino to determine.");
HomieSetting<double> kilogramDividerSetting("kilogramDivider", "Scale value per kilogram. Use BeeScale-Calibration.ino to determine.");
HomieSetting<double> calibrationTemperatureSetting("calibrationTemperature", "Outside Temperature at which the scale has been calibrated");
HomieSetting<double> calibrationFactorSetting("calibrationFactor", "Calibration Factor in gram per degree. 0.0 to disable adjustment");
HomieSetting<double> vccAdjustSetting("vccAdjust", "Calibration value for input voltage. See sketch for details.");

void setupHandler() {
  temperatureNode0.setProperty("unit").send("C");
  temperatureNode1.setProperty("unit").send("C");
  weightNode.setProperty("unit").send("kg");
  batteryNode.setProperty("unit").send("V");
}

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
  scale.set_scale(kilogramDividerSetting.get());  //initialize scale
  scale.set_offset(weightOffsetSetting.get());    //initialize scale
  Homie.getLogger() << endl << endl;
  Homie.getLogger() << "Reading scale, hold on" << endl;
  scale.power_up();
  for (int i = 0; i < 3; i++) {
    float WeightRaw = scale.get_units(3);
    yield();
    Homie.getLogger() << "✔ Raw measurements: " << WeightRaw << "g" << endl;
    WeightSamples.add(WeightRaw);
    delay(500);
    yield();
  }
  scale.power_down();

  weight = WeightSamples.getMedian();

  //temperature compensation
  Homie.getLogger() << "✔ uncompensated median Weight: " << weight << "g" << endl;
  float calibrationTemperature = calibrationTemperatureSetting.get();
  float calibrationFactor = calibrationFactorSetting.get();
  if (temperature1 < calibrationTemperature) weight = weight+(fabs(temperature1 - calibrationTemperature)*calibrationFactor);
  if (temperature1 > calibrationTemperature) weight = weight-(fabs(temperature1 - calibrationTemperature)*calibrationFactor);
  Homie.getLogger() << "✔ compensated median Weight: " << weight << "g" << endl;

  weight = weight / 1000 ;      // we want kilogram
}

void getVolt() {
    raw_voltage = ESP.getVcc();
    vcc_adjust = vccAdjustSetting.get();
    Homie.getLogger() << "Voltage adjust value is: " << vcc_adjust << "V" << endl;
    voltage = raw_voltage / 1000 + vcc_adjust;
}

void loopHandler() {
  if (millis() - lastSent >= sendIntervalSetting.get() * 1000UL || lastSent == 0) {
    getTemperatures();
    Homie.getLogger() << "Temperature0: " << temperature0 << " °C" << endl;
    temperatureNode0.setProperty("degrees").setRetained(false).send(String(temperature0));

    Homie.getLogger() << "Temperature1: " << temperature1 << " °C" << endl;
    temperatureNode1.setProperty("degrees").setRetained(false).send(String(temperature1));

    getWeight();
    Homie.getLogger() << "Weight: " << weight << " kg" << endl;
    weightNode.setProperty("kilogram").setRetained(false).send(String(weight));

    getVolt();
    Homie.getLogger() << "Raw Input Voltage: " << raw_voltage << " V" << endl;
    Homie.getLogger() << "Input Voltage: " << voltage << " V" << endl;
    batteryNode.setProperty("volt").setRetained(true).send(String(voltage));

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["Weight"] = round2two(weight);
    root["Temp1"] = round2two(temperature0);
    root["Temp2"] = round2two(temperature1);
    String values;
    root.printTo(values);
    Homie.getLogger() << "Json data:" << values << endl;
    jsonNode.setProperty("__json__").setRetained(false).send(values);

    //Homie.getLogger() << "Heap size: " << ESP.getFreeHeap() << " bytes, ";

    lastSent = millis();
  }
}

void setup() {
  Serial.begin(115200);

  Homie_setFirmware(FW_NAME, FW_VERSION);
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);
  Homie.disableLedFeedback(); // LED pin would break serial on ESP-07

  sendIntervalSetting.setDefaultValue(DEFAULT_SEND_INTERVAL);
  /*
  * Can't set more than one value to default, see issue #323
  * https://github.com/marvinroger/homie-esp8266/issues/323
  */
  weightOffsetSetting.setDefaultValue(DEFAULT_WEIGHT_OFFSET);
  kilogramDividerSetting.setDefaultValue(DEFAULT_KILOGRAM_DIVIDER);
  calibrationTemperatureSetting.setDefaultValue(DEFAULT_CALIBRATION_TEMPERATURE);
  calibrationFactorSetting.setDefaultValue(DEFAULT_CALIBRATION_FACTOR_GRAM_DEGREE);

  scale.begin(DOUT, PD_SCK);

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
