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

/*
 * Some notes to increase battery runtime:
 * You should remove and short out the input polarity protection diode 
 * if you are using this sketch with the PCB/Design from 
 * https://www.imker-nettetal.de/bienen-nsa/ESP8266-BeeScale1_1.fzz
 * to avoid unnecessary voltage drop. But don't mix polarity!
 * 
 * Also you should remove any unnecessary power indicator LED to save
 * a couple om mA. (e.g. on ESP-07 the red power indicator LED)
 */

#include <Homie.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "HX711.h"
#include <RunningMedian.h>

#define FW_NAME "node-wifi-mqtt-homie-battery"
#define FW_VERSION "0.9.3"
const int DEFAULT_SLEEP_TIME = 600;

// PINs
#define ONE_WIRE_BUS 14
#define DOUT  13
#define PD_SCK  12

/* Use sketch BeeScale-Calibration.ino to determine these calibration values.
   Set them here or use HomieSetting via config.json or WebApp/MQTT
*/
const long DEFAULT_WEIGHT_OFFSET = 154231; // Load cell zero offset. 
const float DEFAULT_KILOGRAM_DIVIDER = 21.76;  // Load cell value per kilogram.

/*
 * Scale should be calibrated with a regulated input of 3.3V!
 * Every change during runtime will cause wrong readings
 */
const float LOW_BATTERY = 3.1;

/*
 * *Some* ESPs tend to have a too low reading on ESP.getVcc()
 * To get the maximum battery runtime you should check what ADC is reading and compare
 * it with a multimeter between VCC and GND at the ESP. Change the following value to e.g. 0.13
 * or -0.02 in case of a too high reading. Or set it using HomieSetting via config.json or WebApp/MQTT
 */
const float DEFAULT_VCC_ADJUST = 0.13;

HomieSetting<long> sleepTimeSetting("sleepTime", "SleepTime in seconds (max. 3600s!), default is 60s");
HomieSetting<long> weightOffsetSetting("weightOffset", "Offset value to zero. Use BeeScale-Calibration.ino to determine.");
HomieSetting<double> kilogramDividerSetting("kilogramDivider", "Scale value per kilogram. Use BeeScale-Calibration.ino to determine.");
HomieSetting<double> vccAdjustSetting("vccAdjust", "Calibration value for input voltage. See sketch for details.");

HX711 scale;

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
#define STATE_CONNECT_WIFI 4

// For connection loss detection    
#define SLEEP_AFTER_X_TIMES_NO_WIFI     3
#define SLEEP_AFTER_X_TIMES_NO_MQTT     3
int wifi_disconnect_event_count = 0;
int mqtt_disconnect_event_count = 0;

#define BATT_WARNING_VOLTAGE 3.2          // Voltage for Low-Bat warning

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
float raw_voltage;
float vcc_adjust;
int SLEEP_TIME;

HomieNode weightNode("weight", "weight");
HomieNode temperatureNode0("temperature0", "temperature");
HomieNode temperatureNode1("temperature1", "temperature");
HomieNode batteryNode("battery", "volt");
HomieNode batAlarmNode("battery","alarm");
HomieNode jsonNode("data","__json__"); //Hiveeyes.org compatibility format

void setupHandler() {
  weightNode.setProperty("unit").send("kg");
  temperatureNode0.setProperty("unit").send("C");
  temperatureNode1.setProperty("unit").send("C");
  batteryNode.setProperty("unit").send("V");
  batAlarmNode.setProperty("bool");
}

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
  scale.begin(DOUT, PD_SCK);
  scale.set_scale(kilogramDividerSetting.get());  //initialize scale
  scale.set_offset(weightOffsetSetting.get());    //initialize scale
  Homie.getLogger() << endl << endl;
  Homie.getLogger() << "Reading scale, hold on" << endl;
  scale.power_up();
  for (int i = 0; i < 4; i++) {
    float WeightRaw = scale.get_units(10);
    yield();
    Homie.getLogger() << "✔ Raw measurements: " << WeightRaw << "g" << endl;
    WeightSamples.add(WeightRaw);
    delay(500);                 
    yield();
  }
  scale.power_down();
  
  weight = WeightSamples.getMedian();
  weight = weight / 1000 ;      // we want kilogram
}

void getVolt() {
    raw_voltage = ESP.getVcc();
    vcc_adjust = vccAdjustSetting.get();
    Homie.getLogger() << "Voltage adjust value is: " << vcc_adjust << "V" << endl;
    voltage = raw_voltage / 1000 + vcc_adjust;
}
        
void transmit() {
    Homie.getLogger() << "Weight: " << weight << " kg" << endl;
    weightNode.setProperty("kilogram").setRetained(false).send(String(weight));

    Homie.getLogger() << "Temperature0: " << temperature0 << " °C" << endl;
    temperatureNode0.setProperty("degrees").setRetained(false).send(String(temperature0));

    Homie.getLogger() << "Temperature1: " << temperature1 << " °C" << endl;
    temperatureNode1.setProperty("degrees").setRetained(false).send(String(temperature1));

    Homie.getLogger() << "Raw Input Voltage: " << raw_voltage << " V" << endl;
    Homie.getLogger() << "Corrected Input Voltage: " << voltage << " V" << endl;
    batteryNode.setProperty("volt").setRetained(true).send(String(voltage));

    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.createObject();
    root["Weight"] = weight;
    root["Temp1"] = temperature0;
    root["Temp2"] = temperature1;
    root["VCC"] = voltage; 
    String values;
    root.printTo(values);
    Homie.getLogger() << "Json data:" << values << endl;
    jsonNode.setProperty("__json__").setRetained(false).send(values);

    /*
     * determine low battery, sleep forever to save lithium cell.
     * we could go lower, but scale is calibrated at 3.3V so we    
     * would get wrong readings.
    */
    if (voltage <= LOW_BATTERY)
    {
      batAlarmNode.setProperty("alarm").setRetained(true).send("true");
      SLEEP_TIME = 0;
      Homie.getLogger() << F("✖✖✖ Battery critically low, sleeping forever ✖✖✖") << endl;
    }else
    {
      batAlarmNode.setProperty("alarm").setRetained(true).send("false");
      SLEEP_TIME = sleepTimeSetting.get();
      Homie.getLogger() << "✔ Sleeping for " << SLEEP_TIME << " seconds" << endl;
    }
}

void onHomieEvent(const HomieEvent& event) {
  switch(event.type) {
    case HomieEventType::WIFI_DISCONNECTED:
      Homie.getLogger() << "Wi-Fi disconnected, reason: " << (int8_t)event.wifiReason << endl;
      if ((int8_t)event.wifiReason != 8)
      {
        wifi_disconnect_event_count++;
        Homie.getLogger() << "✖✖✖ Could not connect to WIFI. Event #" << wifi_disconnect_event_count << ". ✖✖✖" << endl;
        if (wifi_disconnect_event_count >= SLEEP_AFTER_X_TIMES_NO_WIFI) 
        {
          wifi_disconnect_event_count = 0;
          Homie.getLogger() << "✖✖✖ Could not connect to WLAN. Preparing for deep sleep... ✖✖✖" << endl;
          Homie.prepareToSleep(); 
        }
      }
    break;
    case HomieEventType::MQTT_DISCONNECTED:
      Homie.getLogger() << "MQTT disconnected, reason: " << (int8_t)event.mqttReason << endl;
      if ((int8_t)event.mqttReason != 0)
      {
        mqtt_disconnect_event_count++;
        Homie.getLogger() << "✖✖✖ Could not connect to MQTT Broker. Event #" << mqtt_disconnect_event_count << ". ✖✖✖" << endl;
        if (mqtt_disconnect_event_count >= SLEEP_AFTER_X_TIMES_NO_MQTT)
        {
          mqtt_disconnect_event_count = 0;
          Homie.getLogger() << "✖✖✖ Could not connect to MQTT Broker. Preparing for deep sleep... ✖✖✖" << endl;
          Homie.prepareToSleep();
        }
      }
    break;
    case HomieEventType::MQTT_CONNECTED:
      transmit();
      Homie.getLogger() << "✔ Data transmitted, preparing for deep sleep..." << endl;
      Homie.prepareToSleep();
    break;
    case HomieEventType::MQTT_PACKET_ACKNOWLEDGED:
      Homie.getLogger() << "MQTT packet acknowledged, packetId: " << event.packetId << endl;
    break;
    case HomieEventType::READY_TO_SLEEP:
      Homie.getLogger() << "✔ Ready to sleep" << endl;
      buf[0] = STATE_SLEEP_WAKE;
      system_rtc_mem_write(RTC_STATE, buf, 1); // set state for next wakeUp
      ESP.deepSleep(SLEEP_TIME*1000000, WAKE_RF_DISABLED);
    break;
  }
}

void loopHandler() {
}

void setup() {
   
  WiFi.forceSleepBegin();  // send wifi directly to sleep to reduce power consumption
  yield();
  system_rtc_mem_read(RTC_BASE, buf, 2); // read 2 bytes from RTC-MEMORY to get our state
  
  Serial.begin(115200);
  Homie.getLogger() << endl << endl;

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
    /*
     * Can't set more than one value to default, see issue #323
     * https://github.com/marvinroger/homie-esp8266/issues/323
     */
      weightOffsetSetting.setDefaultValue(DEFAULT_WEIGHT_OFFSET);
      kilogramDividerSetting.setDefaultValue(DEFAULT_KILOGRAM_DIVIDER);
      vccAdjustSetting.setDefaultValue(DEFAULT_VCC_ADJUST);
    
      //get all measurements *BEFORE* WIFI get's active. This saves around 4s at full mA
      getTemperatures();
      getWeight();
      getVolt();
      
      WiFi.forceSleepWake();
      delay(500);
      wifi_set_sleep_type(MODEM_SLEEP_T);
        
      Homie_setFirmware(FW_NAME, FW_VERSION);
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
      Homie.onEvent(onHomieEvent);
      Homie.setup();
      break;
    }
}

void loop() {
  Homie.loop();
}
