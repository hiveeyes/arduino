/*
  Open Hive | Scale Adjustment
  ----------------------------
            | Scale ADC HX711

  Copyright (C) 2016-2017 by Clemens Gruber <clemens@hiveeyes.org>
  Copyright (C) 2017      by Andreas Motl <andreas@hiveeyes.org>

  2016-07-01 Clemens Gruber        | Initial version
  2017-04-02 Andreas Motl          | ESP8266 compatibility


  GNU GPL v3 License
  ------------------
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  The license can be viewed at
  http://www.gnu.org/licenses/gpl-3.0.txt


  pins hard coded
  ---------------
  D15 D14  Dout, SCK
*/

// -------------------------+------
// variables you can modify | START
// -------------------------+------

// ** load cell characteristic
// ---------------------------
// define here individual values for the used load cell
// this is not type specific! even load cells of the same type /
// model have individual characteristics
//
// - ZeroOffset is the raw sensor value for "0 kg"
//   write down the sensor value of the scale with no load and
//   adjust it here
// - KgDivider is the raw sensor value for a 1 kg weight load
//   add a load with known weight in kg to the scale, note the
//   sesor value, calculate the value for a 1 kg load and adjust
//   it here
long loadCellZeroOffset = 12022;
long loadCellKgDivider  = -21987;

// Testwaage 38 cm - Prototyp 1
// null  12022
// 1 kg  -21987

// wait between samples
// 3 sec is a good delay so that load cell did not warm up
// too much and external random influences like wind has time
// to go so that the next sample is more valid
const int waitTimeLoadSamples = 3;

// ** median statistics (for weight)
// ---------------------------------
// number of readings for median samples
// 5 is a good compromise between time cost and stable output
int weightSamplesNumber = 10;  // take at least 10 readings for adjusting

// -------------------------+----
// variables you can modify | END
// -------------------------+----

// libraries
// load cell
#include <HX711.h>      // https://github.com/bogde/HX711
HX711 loadCell;         // Create HX711 object; The parameter "gain" is ommited, a default value of "128" is used by the library.
int adjustScale = true;  // flag for adjust vs. operation mode
long weightSensorValue;
float weightKg;

// median statistics to eliminate outlier
#include <RunningMedian.h>
RunningMedian weightSamples = RunningMedian(weightSamplesNumber);  // create RunningMedian object


// Forward declarations
void getWeight();
void outputStatistic(int decimal);
void serial_poll();
int  serial_read_byte();
long serial_read_number();


void setup() {
  // serial communication
  Serial.begin(9600);

  Serial.println("Scale Adjustment for Open Hive / HX711");
  Serial.println("--------------------------------------");
  Serial.println();

  // load cell / HX711 pin definition: Dout 15, SCK 14
  loadCell.begin(15, 14);

  // switch off HX711 / load cell
  loadCell.power_down();


  // adjust scale
  // ------------

  // Step 1: tare scale
  Serial.println(">> Step 1: tare scale");
  Serial.println("   ------------------");
  Serial.println("   Remove all weight from the scale!");
  Serial.println("   (If done, input any character to continue ...)");
  Serial.println();
  Serial.flush();

  // Wait for keypress
  serial_read_byte();

  // get Weight n times and calculate median
  Serial.println("Get raw values for tare: ");
  getWeight();
  Serial.println();
  outputStatistic(0);
  // set the median as zero offset value
  loadCellZeroOffset = weightSamples.getMedian();

  // Step 2: weight lower limit
  Serial.println(">> Step 2: weight lower limit");
  Serial.println("   --------------------------");
  Serial.println("   Load at least 1 kg to the scale so that the raw values are > 0!");
  Serial.println("   (If done, input any character to continue ...)");
  Serial.println();
  Serial.flush();

  // Wait for keypress
  serial_read_byte();

  // get Weight n times and calculate median
  Serial.println("Get raw values for lower limit: ");
  getWeight();
  Serial.println();
  outputStatistic(0);
  long lowerValue = weightSamples.getMedian();

  // Step 3: weight upper limit
  Serial.println(">> Step 3: weight upper limit");
  Serial.println("   --------------------------");
  Serial.println("   Put a know load on the scale! ...");
  Serial.println("   ... and input weight in gram ...");
  Serial.println();
  Serial.flush();

  // Read numeric value from serial port
  long kgValue = serial_read_number();

  // get Weight n times and calculate median
  Serial.print("Get raw values for upper limit \"");
  Serial.print(kgValue);
  Serial.print(" g\":");
  Serial.println();
  getWeight();
  Serial.println();
  outputStatistic(0);
  long upperValue = weightSamples.getMedian();

  // calculate loadCellKgDivider
  loadCellKgDivider = (upperValue - lowerValue) / ((float) kgValue / 1000.0f);

  // output calculated parameter
  Serial.println(">> Done! Your calculated parameters:");
  Serial.println("   ---------------------------------");
  Serial.println();
  Serial.print("   loadCellZeroOffset: ");
  Serial.print(loadCellZeroOffset);
  Serial.println();
  Serial.print("   loadCellKgDivider:  ");
  Serial.print(loadCellKgDivider);
  Serial.println();
  Serial.println("   ---------------------------------");
  Serial.println();

  Serial.println("You can test your calculated settings now!");
  Serial.println("(Input any character to continue ...)");
  Serial.flush();

  // Wait for keypress
  serial_read_byte();

  Serial.println();
}


void loop() {
  // test settings
  // ------------
  adjustScale = false;

  // re-set number of weight sample
// does not work
//  weightSamplesNumber = 5;
//  RunningMedian weightSamples = RunningMedian(weightSamplesNumber);  // create RunningMedian object

  getWeight();
  Serial.println();
  outputStatistic(3);
}


// functions
// ---------

// read raw data
void getWeight() {
  // clear running median samples
  weightSamples.clear();

  // read x times weight and take median
  // do this till running median sample is full
  do {

    Serial.flush();

    // Wait some time between samples to reduce influences from heating and wind
    delay(waitTimeLoadSamples * 1000);

    // power HX711 / load cell
    loadCell.power_up();

    delay(2);  // wait for stabilizing

    // read raw data input of ADS1231
    weightSensorValue = loadCell.read();

    // switch off HX711 / load cell
    loadCell.power_down();

    // calculate weight in kg
    weightKg = ((float)weightSensorValue - (float)loadCellZeroOffset) / (float)loadCellKgDivider;

    // add single value to runnig median sample
    if (adjustScale == true) {
      // use raw values for median statistic
      weightSamples.add(weightSensorValue);
      // debug raw values
      Serial.println(weightSensorValue);
    }
    else
    {
      // use calculated kg values for median statistic
      weightSamples.add(weightKg);
      Serial.print(".");
    }
  } while (weightSamples.getCount() < weightSamples.getSize());
}

// print statistics for raw values (decimal 0) and kg values (decimal 3)
void outputStatistic(int decimal) {
  // debug
  Serial.println("Low \tAvg \tMed \tHigh \tDiff");
  Serial.print(weightSamples.getLowest(),decimal);
  Serial.print("\t");
  Serial.print(weightSamples.getAverage(),decimal);
  Serial.print("\t");
  Serial.print(weightSamples.getMedian(),decimal);
  Serial.print("\t");
  Serial.print(weightSamples.getHighest(),decimal);
  Serial.print("\t");
  Serial.println(weightSamples.getLowest()-weightSamples.getHighest(),decimal);
  Serial.println();
}


// Poll serial port until anything is received
void serial_poll() {
  while (!Serial.available()) {
    // "yield" is not implemented as noop in older Arduino Core releases
    // See also: https://stackoverflow.com/questions/34497758/what-is-the-secret-of-the-arduino-yieldfunction/34498165#34498165
    #if !(ARDUINO_VERSION <= 106) || ARDUINO >= 10605 || defined(ESP8266)
    yield();
    #endif
  };
}

// Read byte from serial port
int serial_read_byte() {
  serial_poll();
  return Serial.read();
}

// Read numeric value from serial port
long serial_read_number() {
  serial_poll();
  return Serial.parseInt();
}
