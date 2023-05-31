/**

  Mois Labs Beescale Yun - Firmware source code
  Collect sensor data, send it via GSM to local SD, flat-file on webserver, and to Hiveeyes' Kotori/InfluxDB/Grafana.

  https://hiveeyes.org/docs/arduino/firmware/moislabs/beescale-yun/README.html

  (c) 2011-2023 Markus Euskirchen
  License: Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License
  Lizenz:  Creative Commons Namensnennung - Nicht-kommerziell - Weitergabe unter gleichen Bedingungen 4.0 International Lizenz

  Source code repository and documentation:
  - https://github.com/bee-mois/beescale
  - https://github.com/hiveeyes/arduino/tree/main/moislabs/beescale-yun
  - https://www.euse.de/wp/blog/series/bienenwaage2/

  Changelog:
  2014-04-04 mois  - Initial version
  2015-01-12 mois  - Added second scale
  2015-03-14 Alexander Wilms -
                     Modularized code into functions, adjusted loop order to avoid program hangs in case of
                     client/network/transmit errors. Code cleanup.
  2016-10-30 mois  - Verbindung umgestellt auf Ethernet hinter OpenWRT-Router
  2016-11-15 mois  - Zeile 116: Median statt Average
  2017-02-20 mois  - zweite Wägezelle wieder eingebaut
  2017-02-26 mois  - für Yun: Bridgeverwendung aktiviert, Kontroll-LED eingebaut, GET Request
  2017-02-27 mois  - Serverantwort auf die Console leiten
  2017-03-06 mois  - DHT22 eingebaut
  2017-03-09 mois  - Schalter; SD-Backup eingebaut
  2017-03-12 mois  - TSL2591 eingebaut; überflüssige Kommentare und Optionen gelöscht.
  2017-03-14 mois  - TSL2591 optimiert: simple read!
  2019-06-16 mois  - added code to swich on/off USB-power by a relay on pinA0/D14
  2020-10-05 mois  - added code to check t2 and correct via relay if necessary.
                   - commented out other relay stuff.
  2022-03-15 mois  - replaced DHT22 by SHT30. 
                   - therefore had to add an i2c multiplexer TCA9548A.
                   - wrapped TSL2591 stuff in multiplexer code.
                   - removed relay which had to reset one of the dht22.
*/

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
float h1;      // SHT30 Humidity Hive1, connect sensor pins to I2C bus number 1 on multiplexer
float t1;      // SHT30 Temperature Hive1, -"-
float h2;      // SHT30 Humidity Hive2, connect sensor pins to I2C bus number 2 on multiplexer
float t2;      // SHT30 Temperature Hive2, -"-
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
int switchPin = 11;      // switch is connected to pin 11
uint16_t lux;            // connect sensor pins to I2C bus number 4 on multiplexer
uint8_t bus;

#include <ADS1231.h>

ADS1231 loadCell;        // create ADS1231 object
ADS1231 loadCell01;

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

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"

Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pins to I2C bus number 4

#include "Adafruit_SHT31.h"

bool enableHeater = false;
Adafruit_SHT31 sht1 = Adafruit_SHT31();        // connect sensor pins to I2C bus number 1
Adafruit_SHT31 sht2 = Adafruit_SHT31();        // connect sensor pins to I2C bus number 2

// Function to select I2C bus on multiplexer TCA9548A
void tcaselect(uint8_t bus) {
    if (bus > 7) return;
    Wire.beginTransmission(0x70);  // TCA9548A multiplexer address
    Wire.write(1 << bus);          // send byte to select bus
    Wire.endTransmission();
}

// Function to read temp&humidity from SHT31 sensors
void readValues(Adafruit_SHT31 sht31, int bus) {
    tcaselect(bus);
    if (bus == 1) {
        h1 = sht31.readHumidity() + 0.4;    /* +x% korrektur */
        t1 = sht31.readTemperature();
    } else if (bus == 2) {
        h2 = sht31.readHumidity() - 0.4;    /* -x% korrektur */
        t2 = sht31.readTemperature();
    }
}

// Function to read out weight cell.
void getWeight(void) {
    // ADS1231 ready?
    if (loadCell.check()) {
        for (int i = 0; i < 10; i++) {
            delay(200);
            // read input of ADS1231:
            weightSensorValue = loadCell.readData();
            weightSensorValue01 = loadCell01.readData();
            // calculate weight in kg
            weightKg = ((float) weightSensorValue - loadCellZeroOffset) / loadCellKgDivider;
            weightKg01 = ((float) weightSensorValue01 - loadCellZeroOffset01) / loadCellKgDivider01;
            // add data to runnig median sample
            weightSamples.add(weightKg);
            weightSamples01.add(weightKg01);
        }
//    weightKg = weightSamples.getAverage();
        weightKg = weightSamples.getMedian();     // Besser, weil durch Ausreißer nicht beeinflusst. Der Median ist der Wert in der Mitte einer nach ihrer Größe geordneten Rangreihe.
        weightKg01 = weightSamples01.getMedian();
        //convert the sensor data from float to string
        dtostrf(weightKg, 6, 3,
                cKg00);     // Achtung: String ist exakt 6 Zeichen lang, inkl. Punkt. Funktioniert nur für Gewichte von 10.000 bis 99.999kg!
        dtostrf(weightKg01, 6, 3, cKg01);
    }
}

// Function for TSL2591 to configure gain and integration time
void configureSensor(void) {
    tsl.setGain(TSL2591_GAIN_LOW);       // 1x gain (bright light)
    // tsl.setGain(TSL2591_GAIN_MED);    // 25x gain
    // tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
    // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)
    // 25 gain/200ms ok for winter indoor. spring sun too bright.
    //  1 gain/300ms ok for sunny spring morning
}

// Function to read from TSL2591 brightness sensor
void TSLRead(void) {
    tcaselect(4);
    uint32_t lum = tsl.getFullLuminosity();
    uint16_t ir, full;
    ir = lum >> 16;
    full = lum & 0xFFFF;
    lux = tsl.calculateLux(full, ir);
//  lux = tsl.getLuminosity(TSL2591_VISIBLE);
// Serial.print("Visible: "); Serial.print(full - ir);
}

// Function to send values via GET request.
void add_line() {
    HttpClient client;
    // convert the readings to a String to send it:
    dataURL = "http://www.euse.de/honig/beescale/add_line2.php?weight1=";
//    dataURL += cKg00;
    dataURL += "17.075";    // fixwert wegen bloeder sonderzeichen
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
    Console.print("Messwerte ");
    Console.print(getTimeStamp());
    Console.println(":");
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
    Console.print(", Lux: ");
    Console.println(lux, DEC);
    Console.print("GET request: ");
    Console.println(dataURL);
    //send data
    client.get(dataURL);
    Console.println("sending data...");
    //if there's incoming data from the net connection, send it out the console.
    while (client.available()) {
        char c = client.read();
        Console.print(c);
    }
    Console.flush();
}

// Function to write backup to SD
void add_line_sd() {
    // make a string that starts with a timestamp for assembling the data to log:
    String dataString;
    dataString += getTimeStamp();
    dataString += ",";
    dataString += cKg00;
    dataString += ",";
    dataString += temp1;
    dataString += ",0,";              // ??
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
    File dataFile = FileSystem.open("/mnt/sda1/arduino/www/datalog.txt", FILE_APPEND);
    // if the file is available, write to it:
    if (dataFile) {
        Console.print("Writing to Yun SD... ");
        dataFile.println(dataString);
        dataFile.close();
        // print to the serial port too:
        Console.println("OK.");
//    Console.print("OK. File Size: "); Console.println(getLogSize());
        Console.println("");
    }
        // if the file isn't open, pop up an error:
    else {
        Console.println("error opening /mnt/sda1/arduino/www/datalog.txt");
    }
}

String getTimeStamp() {
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

void setup() {
    Console.println("00");
//  delay(1000);
    Wire.begin();
    Serial.begin(9600); //init serial port and set baudrate
    Bridge.begin();
    Console.begin();
    FileSystem.begin();
    // a second to initialize:
    delay(1000);
    // while (!Console);   // Wait for the Console port to connect
    // load cell / ADS1231 pin definition: SCL 7, Data 6, PowerDown 10
    loadCell.attach(7, 6, 10);
    // second load cell / ADS1231 pin definition: SCL 8, Data 5, PowerDown 9
    loadCell01.attach(8, 5, 9);
    sensors.begin();    // start DallasTemperature Lib
    // Switch LED: initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(switchPin, INPUT);    // Switch: Set the switch pin as input
    // Init sensors on multiplexers I2C bus
    tcaselect(1);
    sht1.begin(0x44);
    tcaselect(2);
    sht2.begin(0x44);
    tcaselect(4);
    tsl.begin(0x29);
    configureSensor();
}

void loop() {
    while (digitalRead(switchPin) == HIGH) {
        digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
        Console.println("paused by switch...");
        delay(5000);
    }
    digitalWrite(LED_BUILTIN, LOW);      // turn the LED off (LOW is the voltage level)
    //delay(1000);
    //read and prepare sensor data
    readValues(sht1, 1);                 // get SHT31 temperatures/humidity
    readValues(sht2, 2);
    sensors.requestTemperatures();       // get DS18B20 temperatures
    temp2 = sensors.getTempCByIndex(0);  // get temperature from inside sensor node box
    temp1 = sensors.getTempCByIndex(1);  // temperature outside box
    TSLRead();                           // Luminosity by TSL2591
    getWeight();                         // Load cell
    add_line();                          // transmit data
    add_line_sd();                       // backup to Yun's MicroSD
    //  delay(21000); //run every 30 seconds (runtime without delay: 9 seconds)
    delay(111000);    //run every two minutes (runtime without delay: 9 seconds)
}
