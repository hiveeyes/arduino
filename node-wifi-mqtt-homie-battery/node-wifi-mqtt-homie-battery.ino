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

HX711 scale;

const int DEFAULT_SLEEP_TIME = 60;

/* Use sketch BeeScale-Calibration.ino to determine these calibration values.
   Set them here or use HomieSetting via config.json or WebApp/MQTT
*/
const float DEFAULT_WEIGHT_OFFSET = 244017.00; // Load cell zero offset. 
const float DEFAULT_KILOGRAM_DIVIDER = 22.27;  // Load cell value per kilogram.

HomieSetting<long> sleepTimeSetting("sleepTime", "SleepTime in seconds (max. 3600s!), default is 60s");
HomieSetting<double> weightOffsetSetting("weightOffset", "Offset value to zero. Use BeeScale-Calibration.ino to determine.");
HomieSetting<double> kilogramDividerSetting("kilogramDivider", "Scale value per kilogram. Use BeeScale-Calibration.ino to determine.");


ADC_MODE(ADC_VCC);

// deep sleep infrastructure
// Include API-Headers
extern "C" {
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "cont.h"
}

// state definitions for wake/sleep cycles

#define STATE_COLDSTART 0
#define STATE_SLEEP_WAKE 1
#define STATE_ALARM 2
#define STATE_CONNECT_WIFI 4

#define BATT_WARNING_VOLTAGE 3.2          // Voltage for Low-Bat warning
//#define WIFI_CONNECT_TIMEOUT_S 20         // max time for wifi connect to router, if exceeded reset //FIXME

// RTC-MEM Adresses
#define RTC_BASE 65
#define RTC_STATE 66

// global variables
byte buf[10] __attribute__((aligned(4)));
byte state;   // state variable
uint32_t sleepCount;
uint32_t time1, time2;

// Temporary buffer
uint32_t b = 0;
int i;

//end deep sleep infrastructure

RunningMedian WeightSamples = RunningMedian(4);

float weight;
float temperature0;
float temperature1;
float voltage;

HomieNode weightNode("weight", "weight");
HomieNode temperatureNode0("temperature0", "temperature");
HomieNode temperatureNode1("temperature1", "temperature");
HomieNode batteryNode("battery", "battery");
HomieNode jsonNode("data","__json__");

void setupHandler() {
  weightNode.setProperty("unit").send("kg");
  temperatureNode0.setProperty("unit").send("C");
  temperatureNode1.setProperty("unit").send("C");
  batteryNode.setProperty("unit").send("V");
}

#define ONE_WIRE_BUS 14
// Setup a OneWire instance to communicate with any OneWire devices
OneWire OneWire(ONE_WIRE_BUS);
// Pass our OneWire reference to Dallas Temperature.
DallasTemperature sensors(&OneWire);


void getTemperatures() {
  sensors.begin();
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  
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
  scale.begin(13, 12);
  scale.set_scale(kilogramDividerSetting.get());  //initialize scale
  scale.set_offset(weightOffsetSetting.get());    //initialize scale
  Serial.println("Getting weight, hold on");
  scale.power_up();
  for (int i = 0; i < 4; i++) {
    Serial.print("*");
    float WeightRaw = scale.get_units(10);
    yield();
    WeightSamples.add(WeightRaw);
    delay(500);                 
    yield();
  }
  scale.power_down();
  Serial.println("");
  
  weight = WeightSamples.getMedian();
  weight = weight / 1000 ;      // we want kilogram
}

void loopHandler() {
    //getWeight();
    Serial << "Weight: " << weight << " kg" << endl;
    weightNode.setProperty("kilogram").setRetained(false).send(String(weight));

    //getTemperatures();
    Serial << "Temperature0: " << temperature0 << " °C" << endl;
    temperatureNode0.setProperty("degrees").setRetained(false).send(String(temperature0));

    Serial << "Temperature1: " << temperature1 << " °C" << endl;
    temperatureNode1.setProperty("degrees").setRetained(false).send(String(temperature1));

    voltage = ESP.getVcc() / 1000 + 0.3; // ESP07 reads 0.3V too low
    Serial << "Input Voltage: " << voltage << " V" << endl;
    batteryNode.setProperty("volt").setRetained(true).send(String(voltage));

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["Weight"] = weight;
    root["Temp1"] = temperature0;
    root["Temp2"] = temperature1;
    root["VCC"] = voltage; 
    String values;
    root.printTo(values);
    Serial << "Json data:" << values << endl;
    jsonNode.setProperty("__json__").setRetained(false).send(values);


    Homie.prepareToSleep();
    delay(500);
    Serial << "Ready to sleep" << endl;
    buf[0] = STATE_SLEEP_WAKE;
    system_rtc_mem_write(RTC_STATE, buf, 1); // set state for next wakeUp
    //FIXME
    int SLEEP_TIME = sleepTimeSetting.get();
    Serial << "Sleeping for " << SLEEP_TIME << " seconds" << endl;
    ESP.deepSleep(SLEEP_TIME*1000000, WAKE_RF_DISABLED);
    yield();
}

void setup() {

  WiFi.forceSleepBegin();  // send wifi directly to sleep to reduce power consumption
  yield();
  system_rtc_mem_read(RTC_BASE, buf, 2); // read 2 bytes from RTC-MEMORY to get our state
  
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  if ((buf[0] != 0x55) || (buf[1] != 0xaa))  // cold start, magic number is not present in rtc
  { state = STATE_COLDSTART;
    buf[0] = 0x55; buf[1] = 0xaa;             //set and write the magic number
    system_rtc_mem_write(RTC_BASE, buf, 2);   //set and write the magic number
  }
  else // reset was due to sleep-wake cycle
  { system_rtc_mem_read(RTC_STATE, buf, 1);
    state = buf[0];
  }

    // now the restart cause is clear, handle the different states
  Serial.printf("State: %d\r\n", state);

    switch (state)
    { case STATE_COLDSTART:   // first run after power on - initializes
      // prepare to activate wifi
      buf[0] = STATE_CONNECT_WIFI;  // one more sleep required to to wake with wifi on
      WiFi.forceSleepWake();
      WiFi.mode(WIFI_STA);
      system_rtc_mem_write(RTC_STATE, buf, 1); // set state for next wakeUp
      ESP.deepSleep(10, WAKE_RFCAL);
      yield();
      break;

        case STATE_SLEEP_WAKE:
      // prepare to activate wifi
      buf[0] = STATE_CONNECT_WIFI;  // one more sleep required to to wake with wifi on
      WiFi.forceSleepWake();
      WiFi.mode(WIFI_STA);
      system_rtc_mem_write(RTC_STATE, buf, 1); // set state for next wakeUp
      ESP.deepSleep(10, WAKE_RFCAL);
      yield();
      break;      
  
    case STATE_CONNECT_WIFI:
      sleepTimeSetting.setDefaultValue(DEFAULT_SLEEP_TIME);
      weightOffsetSetting.setDefaultValue(DEFAULT_WEIGHT_OFFSET);
      kilogramDividerSetting.setDefaultValue(DEFAULT_KILOGRAM_DIVIDER);
      
      voltage = ESP.getVcc() / 1000;
      getTemperatures();
      getWeight();
      
      WiFi.forceSleepWake();
      delay(500);
      wifi_set_sleep_type(MODEM_SLEEP_T);
        
      Homie_setFirmware("node-wifi-mqtt-homie-battery", "1.0.5");
      Homie.setSetupFunction(setupHandler).setLoopFunction(loopHandler);

      weightNode.advertise("unit");
      weightNode.advertise("kilogram");

      temperatureNode0.advertise("unit");
      temperatureNode0.advertise("degrees");

      temperatureNode1.advertise("unit");
      temperatureNode1.advertise("degrees");

      batteryNode.advertise("unit");
      batteryNode.advertise("volt");


      Homie.disableLedFeedback(); // collides with Serial on ESP07
      Homie.setup();
      break;
    }
}

void loop() {
  Homie.loop();
}
