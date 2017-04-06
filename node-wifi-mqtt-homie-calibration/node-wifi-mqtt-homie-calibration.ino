/***********************************************************************************************************
   ESP8266 Calibration code for node-wifi-mqtt-homie[-battery]
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

  Version 0.9
  
**********************************************************************************************************/

#include <Homie.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "HX711.h"
#include <RunningMedian.h>

// PINs
#define ONE_WIRE_BUS 14
#define DOUT  13
#define PD_SCK  12

RunningMedian offsetSamples = RunningMedian(10);
RunningMedian weightSamples = RunningMedian(10);

HX711 scale;
ADC_MODE(ADC_VCC);

char c;
long offset;
double kilogramDivider;
long knownWeight;

float weight;
float temperature0;
float temperature1;
float voltage;
float raw_voltage;
float vcc_adjust;

OneWire OneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&OneWire);

void getTemperatures() {
  sensors.begin();
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




void getOffset() {
  yield();
    for (int i = 0; i < 10; i++) {
      Serial.print(".");
      yield();
      scale.power_up();
      float offsetValue = scale.read_average(10);
      yield();
      offsetSamples.add(offsetValue);
      scale.power_down();
      yield();
      delay(500);
      yield();
    }
    offset = offsetSamples.getMedian();
}


void getKilogramDivider() {
    for (int i = 0; i < 10; i++) {
      Serial.print(".");
      scale.power_up();
      float weightValue = scale.read_average(10);
      yield();
      weightSamples.add(weightValue);
      scale.power_down();
      delay(500);
      yield();
    }
    kilogramDivider = weightSamples.getMedian();
    kilogramDivider = (kilogramDivider - offset) / knownWeight;
}

void getVccAdjust() {
    raw_voltage = ESP.getVcc();
    vcc_adjust = (3300 - raw_voltage) / 1000;

}


void step1() {
  Serial << endl << endl;
  Serial << " Step 1: enter the weight of a calibration weight " << endl;
  Serial << "--------------------------------------------------" << endl;
  Serial << "  You need a calibration weigt of at least 1000g  " << endl;
  Serial << "  for Step 3. Please measure it with *another*    " << endl;
  Serial << "  (hopefully) exact scale and enter it's weight   " << endl;
  Serial << "  in *gram* here and press ENTER when done...     " << endl; 
  Serial.flush();
  while(!Serial.available()) {
  knownWeight = Serial.parseInt();
  }
}

void step2() {
  Serial << endl << endl;
  Serial << "               Step 2: tare scale                " << endl;
  Serial << "-------------------------------------------------" << endl;
  Serial << "      Remove all weight from the scale and       " << endl;
  Serial << "        press 1 and ENTER to continue...         " << endl;
  Serial.flush();
  while (c!='1') {
    c = Serial.read();
    yield();
    };
  getOffset();
  c='x';
}

void step3() {
  Serial << endl << endl;
  Serial << "        Step 3: determine gain per gramm         " << endl;
  Serial << "-------------------------------------------------" << endl;
  Serial << "Now put the calibration weight of Step 1 onto the" << endl;
  Serial << "    scale press 1 and ENTER when you are done    " << endl;
  Serial.flush();
  while (c!='1') {
    c = Serial.read();
    yield();
    };
  getKilogramDivider();
}

void step4() {
  Serial << endl << endl;
  Serial << "    Step 4: determine calibration temperature"     << endl;
  Serial << "-------------------------------------------------" << endl;
  Serial << "     Make sure, that the temperature sensors     " << endl;
  Serial << "         are attached to the load cell           " << endl;
  Serial << "     or at least at the same (room) temperature. " << endl;
  Serial << "        press 1 and ENTER to continue...         " << endl;
  Serial.flush();
  while (c!='1') {
    c = Serial.read();
    yield();
  }
  getTemperatures();
  c='x';
}

void step5() {
  Serial << endl << endl;
  Serial << "        Step 5: determine volatge adjust         " << endl;
  Serial << "-------------------------------------------------" << endl;
  Serial << " The ESP's ADC is not pretty acurate in ADC_VCC  " << endl;
  Serial << " mode. To determine offset make sure, that the   " << endl;
  Serial << " input voltage is stable above 3.5 V!            " << endl;
  Serial << "        press 1 and ENTER to continue...         " << endl;
  Serial.flush();
  while (c!='1') {
    c = Serial.read();
    yield();
  }
  getVccAdjust();
  c='x';
}

void printResults() {
  Serial << endl << endl;
  Serial << "      Done! Your calculated parameters are:      " << endl;
  Serial << "-------------------------------------------------" << endl;
  Serial << "Your entered known weight was: " << knownWeight << "g" << endl;
  Serial << "weightOffset is:            " << offset << endl;
  Serial << "kilogramDivider is:         " << kilogramDivider << endl;
  Serial << "calibration temperature is: " << temperature1 << endl;
  Serial << "vccAdjust is:               " << vcc_adjust << endl;
  Serial << "The temperature calibration factor can not be    " << endl;
  Serial << "determined right here. It has to be evaluated and" << endl;
  Serial << "calibrated while be scale is productive with real" << endl;
  Serial << "temperature fluctuations. So use 0.0 for now.    " << endl;
  Serial << endl << endl;
}

void testResults() {
  Serial << "   You can test your calculated settings now!    " << endl;
  Serial << "        press 1 and ENTER to continue...         " << endl;
  Serial << "-------------------------------------------------" << endl;
  Serial.flush();
  while (c!='1') {
    c = Serial.read();
    yield();
  }
  getWeight();
  c='x';
}

void getWeight() {
  scale.set_scale(kilogramDivider);  //initialize scale
  scale.set_offset(offset);    //initialize scale
  Serial << endl << endl;
  Serial << "Reading scale, hold on" << endl;
  scale.power_up();
  for (int i = 0; i < 4; i++) {
    float weightRaw = scale.get_units(10);
    yield();
    Serial << "✔ Raw measurements: " << weightRaw << "g" << endl;
    weightSamples.add(weightRaw);
    delay(100);
    yield();
  }
  scale.power_down();

  weight = weightSamples.getMedian();

  weight = weight / 1000 ;      // we want kilogram
}


void setup() {
  wdt_disable();
  wdt_enable(300000);
  WiFi.forceSleepBegin();
  yield();
  Serial.begin(115200);
  Serial << "Scale Adjustment for node-wifi-mqtt-homie / HX711" << endl;
  Serial << "-------------------------------------------------" << endl;
  scale.begin(DOUT, PD_SCK);
  scale.set_scale();
  scale.set_offset();
  scale.power_down();
  step1();
  step2();
  step3();
  step4();
  step5();
  printResults();
}


void loop() {
  testResults();
  yield();
  printResults();
}

