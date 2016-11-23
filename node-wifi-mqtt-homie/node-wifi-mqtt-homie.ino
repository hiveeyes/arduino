/***********************************************************************************************************
   ESP8266 Beescale
   (c)2015-2016 Alexander Wilms
   alex.wilms@adminguru.org
   https://www.imker-nettetal.de

  Dieser Sketch von Alexander Wilms ist lizensiert unter einer Creative Commons
  Namensnennung - Nicht-kommerziell - Weitergabe unter gleichen
  Bedingungen 4.0 International Lizenz.
  This code by Alexander Wilms is licensed under a Creative Commons
  Attribution-NonCommercial-ShareAlike 4.0 International License.

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
//const float offset = 85107.00;  // Offset load cell
const float offset = 244017.00;  // Offset load cell
const float cell_divider = 22.09; // Load cell divider
//const float cell_divider = 22.27; // Load cell divider

// Pin Assignement here stopped working after 23.11.2016
// Reason unknown, see
// Workaround http://www.esp8266.com/viewtopic.php?f=8&t=11958
//HX711 scale(13, 12);    // 13 DOUT, 12 PD_SCK
HX711 scale;

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

HomieSetting<long> sendIntervalSetting("sendInterval", "Interval for measurements in seconds");

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
  Serial.println("Getting weight with 4 measurements, hold on");
    
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

  // Workaround http://www.esp8266.com/viewtopic.php?f=8&t=11958
  scale.begin(13, 12);
  scale.set_scale(cell_divider);  //initialize scale
  scale.set_offset(offset);       //initialize scale
  
  Homie_setFirmware("Bienen", "1.0.0");
  Homie.disableLedFeedback(); // before Homie.setup()
  Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);


  temperatureNode0.advertise("unit");
  temperatureNode0.advertise("degrees");

  temperatureNode1.advertise("unit");
  temperatureNode1.advertise("degrees");

  weightNode.advertise("unit");
  weightNode.advertise("kilogram");

  batteryNode.advertise("unit");
  batteryNode.advertise("volt");

  sendIntervalSetting.setDefaultValue(DEFAULT_SEND_INTERVAL);
  
  Homie.setup();
}

void loop() {
  Homie.loop();
}
