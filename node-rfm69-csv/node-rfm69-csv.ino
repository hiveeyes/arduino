/*
  Open Hive | GSM Node
  --------------------
            | RTC
            | Scale
            | Temperature
            | Humidity
            | Battery
            | Radio

  Copyright (C) 2016 by Clemens Gruber


  Changes
  -------
  2015-10 Clemens Gruber | initial version
  2016-06 Clemens Gruber | restructure


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

  Licence can be viewed at
  http://www.gnu.org/licenses/gpl-3.0.txt


  pins hard coded
  ---------------
  D2           RTC IRQ
  D6, D7       2x DHTxx [out,in]
  D8           DS18B20 one-wire array
  D3 D9        IRQ, SS RFM
  D14 D15 D16  Data, SCL, Power ADS1231
  D17          Power load cell and ADS1231
  A7           battery voltage
*/

// -------------------------+------
// variables you can modify | START
// -------------------------+------

// ** RTC
// ------
// update interval for sensor reading in seconds
const unsigned long updateInterval = 30;

// ** Radio / RFM69
// ----------------
#define NODEID     3    // must be unique for each node on the same network
#define NETWORKID  101  // the same on all nodes that talk to each other
#define GATEWAYID  1    // ID of the gateway
#define FREQUENCY  RF69_868MHZ  // Match frequency to the hardware version of the radio
#define ENCRYPTKEY "demoKey4openHive"  // exactly the same 16 characters/bytes on all nodes!
//#define IS_RFM69HW  // uncomment only for RFM69_H_W! Leave out if you have RFM69W!
#define ACK_TIME   30  // max # of ms to wait for an ack
#define ledPin     13  // Moteino MEGAs have LEDs on D15

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
const long loadCellZeroOffset = -10052992;
const long loadCellKgDivider = 5114752;
// wait between samples
// 3 sec is a good delay so that load cell did not warm up
// too much and external random influences like wind has time
// to go so that the next sample is more valid
int waitTimeLoadSamples = 1;

// ** median statistics (for weight)
// ---------------------------------
// number of readings for median samples
// 5 is a good compromise between time cost and stable output
const int weightSamplesNumber = 3;

// ** humidity and temperature / DHTxx
// -----------------------------------
// how much pins have DHTxx devices connected?
const int humidityNumDevices = 1;
// pins with DHTxx device, pin 6, pin 7
const int humidityPins[humidityNumDevices] = {6};

// ** temperature array / DS18B20
// ------------------------------
// number of temperature devices on bus
const int temperatureNumDevices = 1;
// order on physical array of temperature devices
// the order is normally defined by the device id hardcoded in
// the device, you can physically arrange the DS18B20 in case you
// know the ID and use here {0,1,2,3,4 ... or you can tryp out what
// sensor is on wich position and adusit it herer accordingly
const int temperatureOrderDevices[temperatureNumDevices] = {0};
// resolution for all devices (9, 10, 11 or 12 bits)
const int temperaturePrecision = 12;
// pin for the temperature sensors
const int temperaturePin = 8;
// -------------------------+----
// variables you can modify | END
// -------------------------+----

// libraries
// RTC
#include <Wire.h>  // I2C (for RTC)
#include <DS3231.h>  // RTC itself
DS3231 RTC;  // create RTC object for DS3231
DateTime interruptTime;  // next interrupt time

// raddio / RFM69
#include <RFM69.h>  // RFM69 itself
#include <SPI.h>  // SPI (for RTC)
boolean requestACK = false;
RFM69 radio;

// load cell
#include <ADS1231.h>
ADS1231 loadCell;  // create ADS1231 object

// median statistics
#include <RunningMedian.h>
RunningMedian weightSamples = RunningMedian(weightSamplesNumber);  // create RunningMedian object

// humidity and temperature / DHTxx
#include <dht.h>
dht DHT;  // create DHT object

// temperature array / DS18B20
#include <OneWire.h>  // oneWire for DS18B20
#include <DallasTemperature.h>  // DS18B20 itself
OneWire oneWire(temperaturePin); // oneWire instance to communicate with any OneWire devices (not just DS18B20)
DallasTemperature temperatureSensors(&oneWire);  // pass oneWire reference to DallasTemperature
//was DeviceAddress insideThermometer, outsideThermometer;
uint8_t deviceAddressArray[temperatureNumDevices][8]; // arrays for device addresses
const char gradC[4]={' ','°','C','\0'};  // degree sign

// power saving
#include <LowPower.h>


// we need some variable as char to pass it to payload easily
// timekeeping / RTC
char timestampChar[20];  // should handle yyyy/mm/dd hh:mm:ss and null terminator
// weight / ADS1231
char weightMedianChar[9];  // should handle +-xxx.xxx and null terminator
// temperature and humidity / DHTxx
char temperatureChar[humidityNumDevices][6];  // should handle +/-xx.x and null terminator
char humidityChar[humidityNumDevices][6];  // should handle xxx.x and null terminator
// temperature array / DS18B20
char temperatureArrayChar[temperatureNumDevices][6];  // should handle +/-xx.x and null terminator
// Lipo / battery
char voltageChar[6];  // should handle xx.xx and null terminator

// Forward declarations
void getTimestamp();
void getWeight();
void getHumidityTemperature();
void getTemperature();
void getVoltage();
void Blink(byte PIN, int DELAY_MS);
void wakeUp();

void setup () {
  // debug
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);  // debug led

  // RTC, IRQ
  Wire.begin();
  RTC.begin();
  pinMode(2, INPUT_PULLUP);  // initialize INT0 (D2 on the most Arduino's) to accept external interrupts

  // radio / RFM69
  radio.initialize(FREQUENCY,NODEID,NETWORKID);

  #ifdef IS_RFM69HW  // only for RFM69_H_W!
    radio.setHighPower();
  #endif
  radio.encrypt(ENCRYPTKEY);
  radio.sleep();

  // weight / ADS1231
  // pin definition: SCL 15, Data 14, Power 16
  loadCell.attach(15,14,16);
  // power pin load cell and ADS1231: pin 17
  pinMode(17, OUTPUT);

  // weight debug
  Serial.print(F("Open Hive - Bee Monitoring System"));
  Serial.println();
  Serial.print(F("---------------------------------"));
  Serial.println();
  Serial.print(F("Be patient, it could take some time ... "));
  Serial.println();
  Serial.println();
  Serial.print(F("Median sample for the load cell consists of "));
  Serial.print(weightSamples.getSize());
  Serial.print(F(" single readings."));
  Serial.println();
  Serial.print(F("We wait "));
  Serial.print(waitTimeLoadSamples);
  Serial.print(F(" seconds between each reading. So the weiting time in "));
  Serial.println();
  Serial.print(F("the current case is "));
  Serial.print(weightSamples.getSize() * waitTimeLoadSamples);
  Serial.print(F(" seconds!"));
  Serial.println();
  Serial.println();
  Serial.print(F("Date/Time, Weight, Outside Temp, Outside Humid, Inside Temp, Voltage"));
  Serial.println();

  // temperature array
  temperatureSensors.begin();  // start DallasTemperature library
  temperatureSensors.setResolution(temperaturePrecision);  // set resolution of all devices

  // Assign address manually. The addresses below will beed to be changed
  // to valid device addresses on your bus. Device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // temperatureSensors.getAddress(deviceAddress, index)
  //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
  //outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  //
  // method 1: by index
  // change index to order divices as in nature
  // (an other approach can be to order physical devices ascending to device address on cable)
  for (int i=0; i<temperatureNumDevices; i++) {
    if (!temperatureSensors.getAddress(deviceAddressArray[temperatureOrderDevices[i]], i)) {
      Serial.print(F("Unable to find address for temperature array device "));
      Serial.print(i);
      Serial.println();
    }
  }

  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices,
  // or you have already retrieved all of them. It might be a good idea to
  // check the CRC to make sure you didn't get garbage. The order is
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
  // assigns the seconds address found to outsideThermometer
  //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

  // lipo, battery
  // use internal analog reference for lipo / battery
  analogReference(INTERNAL);

  // set periodic interrupt intervals
  //
  // do this at the end of the setup routine because the initialization
  // of the sensors need some time and mess up timestamp
  //
  //RTC.enableInterrupts(EveryMinute); // options: EverySecond, EveryMinute, EveryHour
  // or specify only the first interrupt time in case EverySecond, EveryMinute, EveryHour
  // does not fit your needs
  DateTime start = RTC.now();
  // add x seconds (interval time) to start time
  interruptTime = DateTime(start.get() + updateInterval);
}


void loop () {
  // update data
  //
  // update timestamp
  getTimestamp();
  // update weight
  getWeight();
  // update humidity and temperature, DHTxx
  getHumidityTemperature();
  // update temperature array
  getTemperature();
  // update battery voltage
  getVoltage();

/*
  Serial.println(timestampChar);
  Serial.println(weightMedianChar);

  for (int i=0; i<humidityNumDevices; i++) {
    Serial.print(F("t"));
    Serial.print(i);  // print index
    Serial.print(F(":"));
    Serial.print(temperatureChar[i]);
    Serial.print(F(","));
    Serial.print(F("h"));
    Serial.print(i);  // print index
    Serial.print(F(":"));
    Serial.print(humidityChar[i]);
    Serial.print(F(","));
  }
  Serial.println();

  for (int i=0; i<temperatureNumDevices; i++) {
    Serial.print(F("ta"));
    Serial.print(i);  // print index
    Serial.print(F(":"));
    Serial.print(temperatureArrayChar[i]);
    Serial.print(F(","));
  }
  Serial.println();

  Serial.println(voltageChar);
*/

  char payload[sizeof(timestampChar)+sizeof(weightMedianChar)+((sizeof(humidityChar[0])+sizeof(temperatureChar[0]))*humidityNumDevices)+sizeof(temperatureArrayChar[0])*temperatureNumDevices+sizeof(voltageChar)];
  snprintf(payload, sizeof(payload), "%s,%s,%s,%s,%s,%s", timestampChar, weightMedianChar,temperatureChar[0],humidityChar[0],temperatureArrayChar[0],voltageChar);

//  Serial.println(payload);
//  Serial.println(sizeof(buffer));

  // send packets
  Serial.print("Sending[");
  Serial.print(sizeof(payload));
  Serial.print("]: ");
  Serial.print(payload);


  if (radio.sendWithRetry(GATEWAYID, payload, sizeof(payload)))
    Serial.print(" ok!");
  else Serial.print(" nothing...");

  radio.sleep();
  Serial.println();

  Blink(ledPin,3);


  // debug
  // blink LED to indicate sensor reading is done
  digitalWrite(ledPin, HIGH);
  Serial.flush();  // flush serial before sleeping
  LowPower.powerDown(SLEEP_60MS, ADC_OFF, BOD_OFF); // delay 60 ms
  digitalWrite(ledPin, LOW);

  // manage sleeping and wake up
  //
  // prepare next interrupt time
  RTC.clearINTStatus();  // this function call is a must to bring INT pin high after an interrupt
  RTC.enableInterrupts(interruptTime.hour(),interruptTime.minute(),interruptTime.second());  // set the interrupt (h,m,s)
  attachInterrupt(0, wakeUp, LOW);  // enable INT0, required to handle level triggered interrupts

  // go sleeping
  // debug
//  Serial.print(F("\nSleeping ... "));
  Serial.flush();  // flush serial before sleeping
  // power down with ADC and BOD disabled
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  // waiting for the next interrupt
  // tzzzzz ... tzzzzz ...

  // wakeup
  detachInterrupt(0);
  // calculate next interrupt time
  interruptTime = DateTime(interruptTime.get() + updateInterval);

  delay(10); // this delay is required to allow CPU to stabilize
//  Serial.println(F("Awake from sleep.\n"));
}


// functions for devices
// ---------------------

// timestamp
void getTimestamp() {
  DateTime currentTime = RTC.now();  // get the current date-time
  snprintf(timestampChar, sizeof(timestampChar), "%d/%02d/%02d %02d:%02d:%02d", currentTime.year(), currentTime.month(), currentTime.date(), currentTime.hour(), currentTime.minute(), currentTime.second());  // write to char array
}

// weight
void getWeight() {
  // clear running median samples
  weightSamples.clear();

  // read x times weight and take median
  // do this till running median sample is full
  do {
    // wait between samples
    Serial.flush();
    for (int i=0; i<waitTimeLoadSamples; i++) {
      // print temperature
      LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); // delay 60 ms
    }

    // power ADS1231 / load cell
    digitalWrite(17, HIGH);
    loadCell.power(HIGH);

    delay(2);  // wait for stabilizing

    // ADS1231 ready?
    while (!loadCell.check()) {
      loadCell.check();
    }

    // read raw data input of ADS1231
    long weightSensorValue = loadCell.readData();

    // switch off ADS1231 / load cell
    loadCell.power(LOW);
    digitalWrite(17, LOW);

    // calculate weight in kg
    float weightKg = ((float)weightSensorValue - (float)loadCellZeroOffset) / (float)loadCellKgDivider;

    // add single value to runnig median sample
    weightSamples.add(weightKg);

    // debug and adjusting
    // you need to uncomment this to see the raw sensor value in
    // the serial monitor for adjusting the scale and define
    // the variables loadCellZeroOffset and loadCellKgDivider
    // for your specific load cell
//    Serial.print(weightSensorValue);
//    Serial.print(" ");
//    Serial.print(weightKg, 3);
//    Serial.print(" ");
  } while (weightSamples.getCount() < weightSamples.getSize());

  // debug
//  Serial.println(" raw data");  // use this line in case you output raw data for debugging and adjusting your scale

  dtostrf(weightSamples.getMedian(), 8, 3, weightMedianChar);  // write to char array
}

// temperature array / DS18B20
void getTemperature() {
  // request temperature on all devices on the bus
  temperatureSensors.setWaitForConversion(false);  // makes it async
  // initiatie temperature retrieval
  temperatureSensors.requestTemperatures();

  // wait at least 750 ms for conversion
  // to save power go to sleep for this delay
  Serial.flush();  // flush serial before sleeping
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF); // delay 60 ms

  // loop through each device, print out temperature
  for (int i=0; i<temperatureNumDevices; i++) {
    // print temperature
    float temperatureC = temperatureSensors.getTempC(deviceAddressArray[i]);
    dtostrf(temperatureC, 5, 1, temperatureArrayChar[i]);  // write to char array
  }
}

// humidity and temperature / DHTxx
void getHumidityTemperature() {
  // read humidity and temperature data
  // loop through all devices
  for (int i=0; i<humidityNumDevices; i++) {
    // read device
    int chk = DHT.read33(humidityPins[i]);

    switch (chk) {
      // this is the normal case, all is working smoothly
      case DHTLIB_OK:
        //Serial.print("OK,\t");
        dtostrf(DHT.temperature, 5, 1, temperatureChar[i]);  // write to char array
        dtostrf(DHT.humidity, 5, 1, humidityChar[i]);  // write to char array
        break;
      // following are error cases
      case DHTLIB_ERROR_CHECKSUM:
        Serial.println(F("Checksum error"));
        break;
      case DHTLIB_ERROR_TIMEOUT:
        Serial.println(F("Time out error"));
        break;
      case DHTLIB_ERROR_CONNECT:
        Serial.println(F("Connect error"));
        break;
      case DHTLIB_ERROR_ACK_L:
        Serial.println(F("Ack Low error"));
        break;
      case DHTLIB_ERROR_ACK_H:
        Serial.println(F("Ack High error"));
        break;
      default:
        Serial.println(F("Unknown error"));
        break;
    }
  }

  // FOR UNO + DHT33 400ms SEEMS TO BE MINIMUM DELAY BETWEEN SENSOR READS,
  // ADJUST TO YOUR NEEDS
  // we do other time consuming things / sleeping after reading DHT33, so we can skip this
//  delay(1000);
}

// battery voltage
void getVoltage() {
  int batteryValue = analogRead(A7); // read battery as analog value
  float voltage = batteryValue * (1.1 / 1024) * (10 + 2) / 2;  // voltage divider
  dtostrf(voltage,5,2,voltageChar);  // write to char array
}

void Blink(byte PIN, int DELAY_MS) {
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

// interrupt service routine for external interrupt on INT0 triggered from RTC
// keep this as short as possible, possibly avoid using function calls
void wakeUp() {
   // handler for the pin interrupt
}
