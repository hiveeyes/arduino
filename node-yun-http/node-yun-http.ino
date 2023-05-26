/***********************************************************************************************************
   mois Yun Beescale
   Collect sensor data, send it via GSM to local SD, flatfile on webserver, and Hiveeyes' Kotori/InfluxDB.
   
   (c)2014-2017 Markus Euskirchen
   https://www.euse.de/wp/blog/series/bienenwaage2/
   https://github.com/bee-mois/beescale
   
   Changes
   -------
   2014-04-04 Initial version
   2015-01-12 Added second scale
   2015-03-14 Modularized code into functions, code cleanup, thanks Alexander!
   2016-10-30 Change from Wifi to ethernet
   2016-11-15 Median instead average
   2017-02-26 Change from Ethernet-Shield to Yun-Shield
   2017-03-06 Added DHT22
   2017-03-09 Added switch, SD for local backup
   2017-03-12 Added TSL2591 digital light sensor
   
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
   
   Credits: Alexander Wilms for a first modularized/functionized loop.
            Hiveeyes Developers for pushing me forward.
  
  Used libraries:
   Bridge:               Yun Standard
   Console:              Yun Standard
   HttpClient:           Yun Standard
   FileIO:               Yun Standard
   ADS1231:		          http://forum.arduino.cc/index.php?action=dlattach;topic=131086.0;attach=67564
                         maybe better (?): https://github.com/rfjakob/barwin-arduino/tree/master/lib/ads1231
   RunningMedian:	       https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian
   SPI:                  Arduino Standard
   digitalWriteFast:     https://github.com/watterott/Arduino-Libs/tree/master/digitalWriteFast
   OneWire:              https://github.com/PaulStoffregen/OneWire
   DallasTemperature:	 https://github.com/milesburton/Arduino-Temperature-Control-Library
   DHT:	      	       https://github.com/adafruit/DHT-sensor-library
   Wire:                 Arduino Standard
   Adafruit_Sensor:      https://github.com/adafruit/Adafruit_Sensor
   Adafruit_TSL2591:     https://github.com/adafruit/Adafruit_TSL2591_Library
**********************************************************************************************************/

// define individual values for used load cell type
  //   loadCellZeroOffset: write down the sensor value of the scale with no load and adjust it
  //   loadCellKgDivider: add a load with known weight in kg to the cell, note the sensor value, calculate
  //   the value for the load and adjust it
// Vertauschte Anschlüsse der Wägezellen:
long loadCellZeroOffset = 6100000;    //die mit den NICHT-versenkten schrauben (erhöhen um den offset zu senken und vice versa)
long loadCellKgDivider = 5810000;
long loadCellZeroOffset01 = 33500000; //die mit den versenkten schrauben
long loadCellKgDivider01 = 3800000;

//for yun:
#include <Bridge.h>
#include <Console.h>
#include <HttpClient.h>
#include <FileIO.h>

#define USERAGENT "beescale"       //User Agent to be checked by php script

// initialize all needed variables
float temp1;   // Temp outside box
float temp2;   // Temp in box
float h1;      // DHT Humidity Hive1
float t1;      // DHT Temperature Hive1
float h2;      // DHT Humidity Hive2
float t2;      // DHT Temperature Hive2
float weight1;
long weightSensorValue;
float weightKg;
boolean weightKgTrusted = false;
float weight2;
long weightSensorValue01;
float weightKg01;
boolean weightKgTrusted01 = false;
static char cKg00[8];
static char cKg01[8];
String dataURL = "";
int switchPin = 11;   // switch is connected to pin 11
uint16_t lux = 0;

#include <ADS1232.h>
ADS1232 loadCell;  // create ADS1231 object
ADS1232 loadCell01;
#include <RunningMedian.h>
// RunningMedian sample size is 11
RunningMedian weightSamples = RunningMedian(11);  // create RunningMedian object
RunningMedian weightSamples01 = RunningMedian(11);

#include <SPI.h>
#include <digitalWriteFast.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 4
OneWire OneWire(ONE_WIRE_BUS);
// Pass our OneWire reference to Dallas Temperature.
DallasTemperature sensors(&OneWire);

#include "DHT.h"
#define DHT1PIN 2
#define DHT2PIN 3
#define DHT1TYPE DHT22
#define DHT2TYPE DHT22
DHT dht1(DHT1PIN, DHT1TYPE);
DHT dht2(DHT2PIN, DHT2TYPE);

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier

// Forward declarations
String getTimeStamp();

// Function to read out weight cell.
void getWeight(void) {
  // ADS1231 ready?
  if (loadCell.is_ready())
  {
    for (int i = 0; i < 10; i++) {
      delay(200);
      // read input of ADS1231:
      loadCell.read(weightSensorValue);
      loadCell01.read(weightSensorValue01);
      // calculate weight in kg
      weightKg = ((float)weightSensorValue - loadCellZeroOffset) / loadCellKgDivider;
      weightKg01 = ((float)weightSensorValue01 - loadCellZeroOffset01) / loadCellKgDivider01;
      // add data to runnig median sample
      weightSamples.add(weightKg);
      weightSamples01.add(weightKg01);
    }
//    weightKg = weightSamples.getAverage();
    weightKg = weightSamples.getMedian();     // Besser, weil durch Ausreißer nicht beeinflusst. Der Median ist der Wert in der Mitte einer nach ihrer Größe geordneten Rangreihe.
    weightKg01 = weightSamples01.getMedian();
    //convert the sensor data from float to string
    dtostrf(weightKg, 6, 3, cKg00);     // Achtung: String ist exakt 6 Zeichen lang, inkl. Punkt. Funktioniert nur für Gewichte von 10.000 bis 99.999kg!
    dtostrf(weightKg01, 6, 3, cKg01);
  }
}

void configureSensor(void)    // Configures the gain and integration time for the TSL2591
{
  tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  // tsl.setGain(TSL2591_GAIN_MED);    // 25x gain
  // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)
  //
  // 25 gain/200ms ok for winter indoor. spring sun too bright.
  //  1 gain/300ms ok for sunny spring morning
}

void simpleRead(void)
{
  // Simple data read example. Just read the infrared, fullspecrtrum diode
  // or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
  lux = tsl.getLuminosity(TSL2591_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2591_INFRARED);
}

// Function to send values via GET request.
void add_line()
{
  HttpClient client;
  // convert the readings to a String to send it:
    dataURL="http://www.euse.de/honig/beescale/add_line2.php?weight1=";
    dataURL += cKg00;
    dataURL += "&weight2=";
    dataURL += cKg01;
    dataURL += "&temp1=";
    dataURL += temp1;
    dataURL += "&temp2=";
    dataURL += temp2;
    dataURL += "&h1=";
    dataURL += h1;
    dataURL += "&t1=";
    dataURL += t1;
    dataURL += "&h2=";
    dataURL += h2;
    dataURL += "&t2=";
    dataURL += t2;
    dataURL += "&lux=";
    dataURL += lux;
  //log to console, view with ssh root@[yun-ip] 'telnet localhost 6571'
    Console.print("Messwerte "); Console.print(getTimeStamp()); Console.println(":");
    Console.print("Kiste1 kg: ");
    Console.print(weightKg, 3);
    Console.print(", Hum: ");
    Console.print(h1);
    Console.print("%, Temp: ");
    Console.println(t1);
    Console.print("Kiste2 kg: ");
    Console.print(weightKg01, 3);
    Console.print(", Hum: ");
    Console.print(h2);
    Console.print("%, Temp: ");
    Console.println(t2);
    Console.print("Temp outside: ");
    Console.print(temp1);
    Console.print(", inside: ");
    Console.print(temp2);
    Console.print(", Lux: "); Console.println(lux, DEC);
    Console.print("GET request: ");
    Console.println(dataURL);
  //send data
    client.get(dataURL);
    Console.println("sending data...");
  //if there's incoming data from the net connection send it out the console.
    while (client.available())
      {
      char c = client.read();
      Console.print(c);
      }
    Console.flush();
}

void add_line_sd()
{
  // make a string that starts with a timestamp for assembling the data to log:
  String dataString;
  dataString += getTimeStamp();
  dataString += ",";
  dataString += cKg00;
  dataString += ",";
  dataString += temp1;
  dataString += ",0,";
  dataString += lux;
  dataString += ",";
  dataString += cKg01;
  dataString += ",";
  dataString += temp2;
  dataString += ",";
  dataString += h1;
  dataString += ",";
  dataString += t1;
  dataString += ",";
  dataString += h2;
  dataString += ",";
  dataString += t2;
  File  dataFile = FileSystem.open("/mnt/sda1/arduino/www/datalog.txt", FILE_APPEND);
  // if the file is available, write to it:
  if (dataFile) {
    Console.print("Writing to Yun SD... ");
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    Console.println("OK.");
//    Console.print("OK. File Size: "); Console.println(getLogSize());
  }
  // if the file isn't open, pop up an error:
  else {
    Console.println("error opening /mnt/sda1/arduino/www/datalog.txt");
  }
}

String getTimeStamp()
{
  String result;
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.begin("date");
  time.addParameter("+%Y/%m/%d %T");
  time.run();  // run the command
  // read the output of the command
  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n') {
      result += c;
    }
  }
  return result;
}

/*
String getLogSize()
{
  String res;
  Process size;
  size.runShellCommand("du -h /mnt/sda1/arduino/www/datalog.txt | cut -f1");
  while (size.running());
  while (size.available() > 0) {
    char c = size.read();
    if (c != '\n') {
      res += c;
    }
  }
  return res;
}
*/

void setup()
{
//  delay(1000);
  Serial.begin(9600); //init serial port and set baudrate
  Bridge.begin();
  Console.begin();
  FileSystem.begin();
  // a second to initialize:
  delay(1000);
  // load cell / ADS1231 pin definition: Data 6, SCL 7, PowerDown 10
  loadCell.begin(6,7,10);
  // second load cell / ADS1231 pin definition: Data 5, SCL 8, PowerDown 9
  loadCell01.begin(5,8,9);
  sensors.begin();    // start DallasTemperature Lib
  // Switch LED: initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(switchPin, INPUT);    // Switch: Set the switch pin as input
  dht1.begin();
  dht2.begin();
  tsl.begin();
  configureSensor();    // digital light sensor
}

void loop()
{
  while (digitalRead(switchPin) == HIGH)
    {
      digitalWrite(LED_BUILTIN, HIGH);    // turn the LED on (HIGH is the voltage level)
      Console.println("paused by switch...");
      delay(5000);
    }
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off (LOW is the voltage level)
//  delay(1000);
  //read and prepare sensor data
  sensors.requestTemperatures();      // get temperatures
  temp2 = sensors.getTempCByIndex(0); // Temp in box
  temp1 = sensors.getTempCByIndex(1); // Temp outside box
  simpleRead();                       // Luminosity by TSL2591
  h1 = dht1.readHumidity();
  t1 = dht1.readTemperature();
  h2 = dht2.readHumidity();
  t2 = dht2.readTemperature();
  getWeight();
  add_line();     //transmit data
  add_line_sd();  // backup to Yun's MicroSD
  Console.println();
//  delay(21000); //run every 30 seconds (runtime without delay: 9 seconds)
  delay(111000); //run every two minutes (runtime without delay: 9 seconds)
}
