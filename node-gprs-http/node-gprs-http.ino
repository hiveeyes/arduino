/*
  Open Hive | GSM or WiFi
  -----------------------
  sensors:  | Scale
            | Temperature
            | Humidity
  monitor:  | RTC
            | SD
            | Battery
  transp:   | GSM via HTTP
            | WiFi via HTTP (ESP8266)

  Copyright (C) 2013-2017 by Clemens Gruber


  Changes
  -------
  2015-10-01 Clemens Gruber | Baby steps
  2016-03-01 Clemens Gruber | Make sketch ready for publishing
  2016-06-20 Clemens Gruber | Modularization of upload URL
  2016-09-17 Clemens Gruber | Modularization of sensors and debugging
  2017-01-09 Clemens Gruber | Add support for HX711 and ESP8266


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
  D4, D10      Power (LOW is power on), CS - SD
  D6, D7       2x DHTxx [out,in]
  D8           DS18B20 one-wire array
  D12 D13      DTR, CTS - GSM
  D14 D15 D16  Data, SCL, Power - ADS1231
  D14 D12      Dour, SCK - HX711 (ESP8266)
  D17          Power load cell and ADS1231
  A7           battery voltage
*/

// -------------------------+------
// variables you can modify | START
// -------------------------+------

// ** what components are in use?
// ------------------------------
// comment all components you are not using
#define isScale                 true
#define isScaleADS1231          true
#define isScaleHX711            false
#define isTemperatureArray      true
#define isHumidityTemperature   true
// Define for ESP8266 also! gets time from server
#define isRTC                   true
#define isSD                    false
#define isBattery               true

#define isGSM                   true
#ifdef ARDUINO_ARCH_ESP8266
    #define isGSM               false
    #define isWifi              true
#endif

#define isEthernet              false

// comments this line in case you are using GSM as upload path
// GSM and Serial debug using the same pins and interfere
#define isDebug                 false

// ** RTC / timekeeping
// --------------------
// update interval for sensor reading in seconds
// unsigned long updateInterval = 60UL * 20;  // s*m*h*d
// ESP8266: maximun update interval is 1 hour due to variable size limitations
// (we'll fix that in a later version)
unsigned long updateInterval = 60UL * 60UL;  // s*m*h*d  // seems it take 11 sec longer than in theory

// keep in mind that very low values make no sense here!
// sensor reading takes time, scale reads load multiple to
// eliminate outlier so be sure that the update intervall is
// below sum of sensor reading and waiting time!!

// ** upload server
// ----------------
#if isGSM || isWifi || isEthernet
  // specify your node number
  const char uploadNode[]       = "1";
  // and user credentials
  const char uploadUserName[]   = "your-user-name";
  const char uploadUserId[]     = "your-id";
  // server, path and upload script
  const char uploadDomain[]     = "data.example.com";
  const char uploadFolderFile[] = "apiary/upload.php";
  const char uploadScheme[]     = "http://%s/%s?user=%s&id=%s&node=%s&dataset=";  // "%s" is the variable placeholder for snprintf command
#endif

// ** GSM / GPRSbee
// ----------------
#if isGSM
  // specify your APN here, specific for your network operator
  #define APN "internet.eplus.de"
#endif

// ** WiFi
// -------
#if isWifi
  // specify your SSID an PW here, specific for your local WiFi
  #define WLAN_SSID  "your-ssid"
  #define WLAN_PW    "your-pw"
#endif

// ** load cell characteristic
// ---------------------------
#if isScale
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
  const long loadCellZeroOffset = 38623;
//  const long loadCellKgDivider  = 22053;
  // 1/2 value for single side measurement, so that 1 kg is displayed as 2 kg
  const long loadCellKgDivider  = 11026;

  // wait between samples
  // 3 sec is a good delay so that load cell did not warm up
  // too much and external random influences like wind has time
  // to go so that the next sample is more valid
  const int waitTimeLoadSamples = 3;

  #if isScaleHX711
    const int scalePinDout = 14;  // Dout
    const int scalePinSck  = 12;  // SCK
  #endif
#endif

// ** median statistics (for weight / load cell)
// ---------------------------------------------
#if isScale
  // number of readings for median samples
  // 5 is a good compromise between time cost and stable output
  const int weightSamplesNumber = 5;
#endif

// ** humidity and temperature / DHTxx
// -----------------------------------
#if isHumidityTemperature
  // DHTxx type, set to "read22" for DHT22 or "read33" for DHT33
  // you have to use the same hardware type (DHT22 or DHT33) for all DHT devices
  #define readHumidityType read22
  // how much pins have DHTxx devices connected?
  const int humidityNumDevices = 1;
  // pins with DHTxx device, pin 6, pin 7
  const int humidityPins[humidityNumDevices] = {4};
#endif

// ** temperature array / DS18B20
// ------------------------------
#if isTemperatureArray
  // number of temperature devices on bus
  const int temperatureNumDevices = 1;
  // order on physical array of temperature devices
  // the order is normally defined by the device id hardcoded in
  // the device, you can physically arrange the DS18B20 in case you
  // know the ID and use here {0,1,2,3,4 ... or you can try out what
  // sensor is on wich position and adjust it here accordingly
  const int temperatureOrderDevices[temperatureNumDevices] = {0};
  // resolution for all devices (9, 10, 11 or 12 bits)
  const int temperaturePrecision = 12;
  // pin for the temperature sensors
  const int temperaturePin = 5;
#endif

// ** SD card
// ----------
#if isSD
  // data upload to server, path and file
  const char sdStorageUploadPath[]  = "/upload2/";
  const char sdStorageUploadFile[]  = "d-new.txt";

  // data archive, path
  const char sdStorageArchivePath[] = "/archive/";
#endif


// ------------------------------
// ** static dataset (v1): header
// ------------------------------
// define header für dataset
// do not use spaces before or after an comma
//const char datasetHeader[] = "Date/Time,Weight,Outside Temp,Outside Humid,Inside Temp,Inside Humid,H1 Temp,H2 Temp,H3 Temp,H4 Temp,H5 Temp,Voltage";
//const char datasetHeader[] = "Date/Time,Weight,Outside Temp,Outside Humid,Inside Temp,Voltage";
const char datasetHeader[] = "Datum/Zeit,Gewicht,Aussen-Temperatur,Aussen-Feuchtigkeit,Brut-Temperatur,Spannung";

// -------------------------+----
// variables you can modify | END
// -------------------------+----

// libraries
// RTC
#if isRTC
  #if isWifi
    // intentionally blank
    // no special lib for ESP8266
  #else
    #include <Wire.h>        // I2C (for RTC)
    #include <DS3231.h>      // RTC itself
    DS3231 RTC;              // create RTC object for DS3231
    DateTime interruptTime;  // next interrupt time
  #endif
#endif

// SD card
#if isSD
// #include <SD.h>
#include "SdFat.h"
  SdFat SD;
  const int sdPinCs = 10;

  // some simplificatiion for your convenience
  // calculate size of sdStorageUploadPathFile
  char sdStorageUploadPathFile[
    +sizeof(sdStorageUploadPath)
    +sizeof(sdStorageUploadFile)
  ];
#endif

// GSM / GPRSbee
#if isGSM
  #include <GPRSbee.h>
  const int BEEDTR = 12;
  const int BEECTS = 13;
#endif


// WiFi
#if isWifi
  // Wifi functions for ESP8266
  #include <ESP8266WiFi.h>
  #include <ESP8266WiFiMulti.h>
  // ESP8266 as http client
  #include <ESP8266HTTPClient.h>
  // multiples AP possible
  ESP8266WiFiMulti WiFiMulti;

  // maximal connection attempts
  int maxConnectionAttempts = 15;
#endif

// load cell
#if isScale
  #if isScaleADS1231
    #include <ADS1231.h>
    ADS1231 loadCell;  // create ADS1231 object
  #endif
  #if isScaleHX711
    #include <HX711.h>
    HX711 loadCell;  // create HX711 object
  #endif
#endif

// median statistics (for weight)
#if isScale
  #include <RunningMedian.h>
  RunningMedian weightSamples = RunningMedian(weightSamplesNumber);  // create RunningMedian object
#endif

// humidity and temperature / DHTxx
#if isHumidityTemperature
  #include <dht.h>
  dht DHT;  // create DHT object
#endif

// temperature array / DS18B20
#if isTemperatureArray
  #include <OneWire.h>              // oneWire for DS18B20
  #include <DallasTemperature.h>    // DS18B20 itself
  OneWire oneWire(temperaturePin);  // oneWire instance to communicate with any OneWire devices (not just DS18B20)
  DallasTemperature temperatureSensors(&oneWire);        // pass oneWire reference to DallasTemperature
  uint8_t deviceAddressArray[temperatureNumDevices][8];  // arrays for device addresses
  const char gradC[4]={' ','°','C','\0'};                // degree sign
#endif

// power saving
#if isWifi
  // intentionally blank
  // LowPower.h does not support ESP8266
#else
  #include <LowPower.h>
#endif

// lipo, battery
#if isBattery
  // battery pin
  #if isWifi
    const int batteryPin = A0;
    const float analogMax = 1.0;
  #else
    const int batteryPin = A7;
    const float analogMax = 1.1;
  #endif
#endif

// we need some variable as char to pass it to payload easily
// timekeeping / RTC
#if isRTC
  char timestampChar[20];  // should handle yyyy/mm/dd hh:mm:ss and null terminator
#endif
// weight
#if isScale
  char weightMedianChar[9];  // should handle +-xxx.xxx and null terminator
#endif
// temperature and humidity / DHTxx
#if isHumidityTemperature
  char temperatureChar[humidityNumDevices][6];  // should handle +/-xx.x and null terminator
  char humidityChar[humidityNumDevices][6];  // should handle xxx.x and null terminator
#endif
// temperature array / DS18B20
#if isTemperatureArray
  char temperatureArrayChar[temperatureNumDevices][6];  // should handle +/-xx.x and null terminator
#endif
// Lipo / battery
#if isBattery
  char voltageChar[6];  // should handle xx.xx and null terminator
#endif

// some simplificatiion for your convenience
// calculate size of uploadPath
#if isGSM || isWifi || isEthernet
  char uploadPath[
    34  // 34 = char in "http://%s/%s?user=%s&id=%s&node=%s&dataset=%s" without variables
    +sizeof(uploadDomain)
    +sizeof(uploadFolderFile)
    +sizeof(uploadUserName)
    +sizeof(uploadUserId)
    +sizeof(uploadNode)
  ] = "";
#endif


void setup () {
  // debug and GSM
  Serial.begin(9600);
  #if isDebug
    // new line after wakeup from sleep garbage
    Serial.flush();
    Serial.println();
    Serial.println();
    Serial.println();
  #endif
  // RTC, IRQ
  #if isRTC
    #if isWifi
      // intentionally blank
      // no special lib for ESP8266
    #else
      Wire.begin();
      RTC.begin();
      // initialize INT0 (D2 on the most Arduinos) to accept external interrupts
      pinMode(2, INPUT_PULLUP);
    #endif
  #endif

  // SD card
  #if isSD
    // switchable power on D4 for SD
    pinMode(4, OUTPUT);

    // initialize SD card
    // power SD
    digitalWrite(4, LOW);  // LOW is power on
    // see if the card is present and can be initialized
    if (!SD.begin(sdPinCs)) {
      #if isDebug
        Serial.println(F("SD card failed, or not present!"));
        Serial.println();
      #endif
    }
    #if isDebug
      Serial.println(F("SD card initialized!"));
      Serial.println();
    #endif

    // make upload directories on SD
    if (!SD.mkdir(sdStorageUploadPath)) {
      #if isDebug
        Serial.print(F("Failed to create "));
        Serial.print(sdStorageUploadPath);
        Serial.print(F(" on SD card!"));
        Serial.println();
      #endif
    }
    #if isDebug
      Serial.print(F("Directory "));
      Serial.print(sdStorageUploadPath);
      Serial.print(F(" on SD card created or already existing!"));
      Serial.println();
    #endif

    // switch off SD
    digitalWrite(4, HIGH);  // LOW is power on


    // some simplificatiion for your convenience
    // make sdStorageUploadPathFile
    snprintf(sdStorageUploadPathFile, sizeof(sdStorageUploadPathFile), "%s%s",
      sdStorageUploadPath,
      sdStorageUploadFile
    );
  #endif

  // GSM / GPRSbee
  #if isGSM
    // pwrkeyPin, vbatPin, statusPin, we use -1 for vbatPin because we have no switched battery here
    gprsbee.initAutonomoSIM800(Serial, BEEDTR, -1, BEECTS);
    // make sure the GPRSbee is switched off to begin with
    gprsbee.off();
  #endif

  // WiFi
  #if isWifi
    WiFiMulti.addAP(WLAN_SSID, WLAN_PW);
//    WiFiMulti.addAP(WLAN_SSID_2, WLAN_PW_2);
  #endif

  // some simplificatiion for your convenience
  // build uploadPath and upload header
  #if isGSM || isWifi || isEthernet
    // build uploadPath
    snprintf(uploadPath, sizeof(uploadPath), uploadScheme,
      uploadDomain,
      uploadFolderFile,
      uploadUserName,
      uploadUserId,
      uploadNode
    );

    // update header
    // calculate size of uploadUrl
    char uploadUrl[
      +sizeof(uploadPath)
      +sizeof(datasetHeader)
    ];
    // make uploadUrl
    snprintf(uploadUrl, sizeof(uploadUrl), "%s%s",
      uploadPath,
      datasetHeader
    );
  #endif
  #if isGSM
    // upload to server
    char buffer[50];  // buffer for get return
    memset(buffer, 0, sizeof(buffer));
    gprsbee.doHTTPGET(APN,uploadUrl,buffer,sizeof(buffer));
  #endif
  #if isSD
//    writeToSd(datasetHeader);
  #endif

  // weight / ADS1231
  #if isScale
    #if isScaleADS1231
      // pin definition: SCL 15, Data 14, Power 16
      loadCell.attach(15,14,16);
      // power pin load cell and ADS1231: pin 17
      pinMode(17, OUTPUT);
    #endif
    #if isScaleHX711
      loadCell.begin(scalePinDout, scalePinSck);
      loadCell.set_scale(loadCellKgDivider);  // this value is obtained by calibrating the scale with known weights; see the README for details
      loadCell.set_offset(loadCellZeroOffset);
    #endif
    // weight debug
    #if isDebug
      Serial.print(F("Open Hive | Scale"));
      Serial.println();
      Serial.print(F("-----------------"));
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
    #endif
  #endif

  // ouput dataset header
  #if isDebug
    Serial.println();
    Serial.print(datasetHeader);
    Serial.println();
  #endif

  // temperature array
  #if isTemperatureArray
    temperatureSensors.begin();  // start DallasTemperature library
    temperatureSensors.setResolution(temperaturePrecision);  // set resolution of all devices

    // Method 1: Assign address manually
    // The addresses below will beed to be changed to valid device addresses
    // on your bus. Device address can be retrieved by using either
    // oneWire.search(deviceAddress)
    // or individually via
    // temperatureSensors.getAddress(deviceAddress, index)
    //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };
    //outsideThermometer   = { 0x28, 0x3F, 0x1C, 0x31, 0x2, 0x0, 0x0, 0x2 };

    // Method 2: by index
    // change index to order divices as in nature
    // (an other approach can be to order physical devices ascending to device address on cable)
    for (int i=0; i<temperatureNumDevices; i++) {
      if (!temperatureSensors.getAddress(deviceAddressArray[temperatureOrderDevices[i]], i)) {
        #if isDebug
          Serial.print(F("Unable to find address for temperature array device "));
          Serial.print(i);
          Serial.println();
        #endif
      }
    }
  #endif

  // lipo, battery
  #if isBattery
    // power saving
    #if isWifi
      // intentionally blank
      // no analogReference for ESP8266
    #else
      // use internal analog reference for lipo / battery
      analogReference(INTERNAL);
    #endif
  #endif

  // set periodic interrupt intervals
  //
  // do this at the end of the setup routine because the initialization
  // of the sensors need some time and mess up timestamp
  //
  #if isRTC
    #if isWifi
      // intentionally blank
      // no special lib for ESP8266
    #else
      //RTC.enableInterrupts(EveryMinute); // options: EverySecond, EveryMinute, EveryHour
      // or specify only the first interrupt time in case EverySecond, EveryMinute, EveryHour
      // does not fit your needs
      DateTime start = RTC.now();
      // add x seconds (interval time) to start time
      interruptTime = DateTime(start.get() + updateInterval);
    #endif
  #endif
}


// functions for devices
// ---------------------

// timestamp
#if isRTC
  #if isWifi
    void getTimestamp() {
      // set timestamp later by server
      memcpy(timestampChar, "timestamp-by-server", sizeof("timestamp-by-server"));
      /*
      // try max. 15x to connect
      for (int i = 0; i < maxConnectionAttempts; i++) {
        // wait for WiFi connection
        if ((WiFiMulti.run() == WL_CONNECTED)) {
          // debug WIfi
          #if isDebug
            Serial.println();
            Serial.print(F("WiFi connected! IP address: "));
            Serial.print(WiFi.localIP());
            Serial.println();
          #endif

          HTTPClient http;
          // configure traged server and url
          // do NOT use "http://" in the domain!
          //http.begin("192.168.1.12", 443, "/test.html", true, "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
          http.begin(uploadDomain, 80, "/time.php");  // HTTP

          // start connection and send HTTP header
          int httpCode = http.GET();

          // httpCode will be negative on error
          if (httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            #if isDebug
              Serial.printf("[HTTP] GET... code: %d\n", httpCode);
            #endif
             // file found at server
            if(httpCode == 200) {
              String payload = http.getString();
              #if isDebug
                Serial.print("Time from server: ");
                Serial.println(payload);
              #endif
              payload.toCharArray(timestampChar, sizeof(timestampChar));  // write to char array
            }
          }
          else {
            #if isDebug
              Serial.print(F("[HTTP] GET... failed, no connection or no HTTP server\n"));
            #endif
          }
          http.end();
          break;
        }
        // not succesfully connected
        else {
          #if isDebug
            Serial.print(".");
          #endif
          // wait 400 ms before retry
          delay(400);
        }
      }
      */
    }
  #else
    void getTimestamp() {

      // Get the current datetime
      DateTime currentTime = RTC.now();

      // Write to char array
      snprintf(timestampChar, sizeof(timestampChar), "%d/%02d/%02d %02d:%02d:%02d", currentTime.year(), currentTime.month(), currentTime.date(), currentTime.hour(), currentTime.minute(), currentTime.second());  // write to char array

    }
  #endif
#endif

// SD card
#if isSD
  void writeToSd(char *data) {
    // power SD
    digitalWrite(4, LOW);  // LOW is power on

    // open file
    File dataFile = SD.open(sdStorageUploadPathFile, FILE_WRITE);

    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(data);
      dataFile.close();
      // debug
      #if isDebug
        Serial.println(F("Wrote dataset to SD!"));
      #endif
    }
    // if the file isn't open, pop up an error:
    else {
      // debug
      #if isDebug
        Serial.print(F("Error opening "));
        Serial.print(sdStorageUploadPathFile);
        Serial.println();
      #endif
    }
    // switch off SD
    digitalWrite(4, HIGH);  // LOW is power on
  }
#endif

// weight
#if isScale
  void getWeight() {
    // clear running median samples
    weightSamples.clear();

    // read x times weight and take median
    // do this till running median sample is full
    do {
      // wait between samples
      Serial.flush();
      for (int i=0; i<waitTimeLoadSamples; i++) {
        // sleep for one second
        #if isWifi
          delay(1000);
        #else
          LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
        #endif
      }

      #if isScaleADS1231
        // power ADS1231 and load cell
        digitalWrite(17, HIGH);
        loadCell.power(HIGH);

        delay(2);  // wait for stabilizing

        // ADS1231 ready?
        while (!loadCell.check()) {
          loadCell.check();
        }

        // read raw data input of ADS1231
        long weightSensorValue = loadCell.readData();

        // switch off ADS1231 and load cell
        loadCell.power(LOW);
        digitalWrite(17, LOW);

        // calculate weight in kg
        float weightKg = ((float)weightSensorValue - (float)loadCellZeroOffset) / (float)loadCellKgDivider;
      #endif
      #if isScaleHX711
        // power HX711 and load cell
        loadCell.power_up();

        // read data input of ADS1231
        float weightKg = loadCell.get_units();

        // switch off ADS1231 and load cell
        loadCell.power_down();  // put the ADC in sleep mode
      #endif

      // add single value to runnig median sample
      weightSamples.add(weightKg);
    } while (weightSamples.getCount() < weightSamples.getSize());

    // Final measurement value
    float weight = weightSamples.getMedian();

    // Write to char array
    dtostrf(weight, 8, 3, weightMedianChar);

  }
#endif

// temperature array / DS18B20
#if isTemperatureArray
  void getTemperature() {
    // request temperature on all devices on the bus
    temperatureSensors.setWaitForConversion(false);  // makes it async
    // initiatie temperature retrieval
    temperatureSensors.requestTemperatures();

    // wait at least 750 ms for conversion
    // to save power go to sleep for this delay
    Serial.flush();  // flush serial before sleeping
    // sleep for one second
    #if isWifi
      delay(1000);
    #else
      LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);
    #endif

    // loop through each device, print out temperature
    for (int i=0; i<temperatureNumDevices; i++) {

      // Read temperature for single device
      float temperatureC = temperatureSensors.getTempC(deviceAddressArray[i]);

      // Write to char array
      dtostrf(temperatureC, 5, 1, temperatureArrayChar[i]);

    }
  }
#endif

// humidity and temperature / DHTxx
#if isHumidityTemperature
  void getHumidityTemperature() {
    // read humidity and temperature data
    // loop through all devices
    for (int i=0; i<humidityNumDevices; i++) {

      // Read sensor device
      // We have to do this twice and wait at least 500 ms between readings,
      // otherwise the DHTxx will report the value from the previous reading.
      DHT.readHumidityType(humidityPins[i]);

      // To save power, go to sleep for this delay
      Serial.flush();  // flush serial before sleeping

      // Sleep for 500 ms
      #if isWifi
        delay(500);
      #else
        LowPower.powerDown(SLEEP_500MS, ADC_OFF, BOD_OFF);
      #endif

      // Read sensor the second time, this should deliver the appropriate values
      int chk = DHT.readHumidityType(humidityPins[i]);

      switch (chk) {
        // this is the normal case, all is working smoothly
        case DHTLIB_OK:
          // Serial.print("OK,\t");

          // Write to char array
          dtostrf(DHT.temperature, 5, 1, temperatureChar[i]);
          dtostrf(DHT.humidity, 5, 1, humidityChar[i]);


          break;

        // following are error cases
        case DHTLIB_ERROR_CHECKSUM:
          #if isDebug
            Serial.println(F("DHTxx Checksum error"));
          #endif
          break;
        case DHTLIB_ERROR_TIMEOUT:
          #if isDebug
            Serial.println(F("DHTxx Time out error"));
          #endif
          break;
        default:
          #if isDebug
            Serial.println(F("DHTxx Unknown error"));
          #endif
          break;
      }
    }

    // FOR UNO + DHT33 400ms SEEMS TO BE MINIMUM DELAY BETWEEN SENSOR READS,
    // ADJUST TO YOUR NEEDS
    // we do other time consuming things / sleeping after reading DHT33, so we can skip this
//  delay(1000);
  }
#endif

// battery voltage
// read the battery level from the analog pin.
//
// ** for Lipo 4.2V
// Our 1M & 220K voltage divider takes the max lipo value of
// 4.2V and drops it to 0.757V max. This means our min analog
// read value should be 580 (3.14V / 0.566V) and the max analog
// read value should be 774 (4.2V / 0.757).
//
// ** for 4x 1,5 V AA battery = 6 V
// Our 1.5M & 220K voltage divider takes the max battery value of
// 6V and drops it to 0.767V max. This means our min analog
// read value should be 411 (3.14V / 0.402V) and the max analog
// read value should be 785 (6V / 0.767V).
#if isBattery
  void getVoltage() {
    int batteryValue = analogRead(batteryPin); // read battery as analog value

    // Compute voltage based on voltage divider resistors
    float voltage = batteryValue * (analogMax / 1024) * (10 + 2) / 2;  // voltage divider, values in Mega Ohm

    // Write to char array
    dtostrf(voltage,5,2,voltageChar);  // write to char array


  }
#endif

// interrupt service routine for external interrupt on INT0 triggered from RTC
// keep this as short as possible, possibly avoid using function calls
#if isRTC
  #if isWifi
    // intentionally blank
    // no special lib for ESP8266
  #else
    void wakeUp() {
      // handler for the pin interrupt
    }
  #endif
#endif


// Main program
// ------------

void loop () {
  // update data
  //
  // update timestamp
  #if isRTC
    getTimestamp();
  #endif
  // update weight
  #if isScale
    getWeight();
  #endif
  // update humidity and temperature, DHTxx
  #if isHumidityTemperature
    getHumidityTemperature();
  #endif
  // update temperature array
  #if isTemperatureArray
    getTemperature();
  #endif
  // update battery voltage
  #if isBattery
    getVoltage();
  #endif

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

  // build dataset
  // calculate size of dataset
  char dataset[0
    #if isRTC
      +sizeof(timestampChar)
    #endif
    #if isScale
      +sizeof(weightMedianChar)
    #endif
    #if isHumidityTemperature
      +((sizeof(humidityChar[0]) + sizeof(temperatureChar[0])) * humidityNumDevices)
    #endif
    #if isTemperatureArray
      +sizeof(temperatureArrayChar[0]) * temperatureNumDevices
    #endif
    #if isBattery
      +sizeof(voltageChar)
    #endif
  ] = "";

  // make dataset
  #if isRTC
    strcat(dataset, timestampChar);
  #endif
  #if isScale
    strcat(dataset, ",");
    strcat(dataset, weightMedianChar);
  #endif
  // loop through all HumidityTemperature devices
  #if isHumidityTemperature
    for(int i=0; i<humidityNumDevices; i++) {
      strcat(dataset, ",");
      strcat(dataset, temperatureChar[i]);
      strcat(dataset, ",");
      strcat(dataset, humidityChar[i]);
    }
  #endif
  // loop through all Temperature Array devices
  #if isTemperatureArray
    for(int i=0; i<temperatureNumDevices; i++) {
      strcat(dataset, ",");
      strcat(dataset, temperatureArrayChar[i]);
    }
  #endif
  #if isBattery
    strcat(dataset, ",");
    strcat(dataset, voltageChar);
  #endif
  // debug
  #if isDebug
    Serial.println(dataset);
  #endif

  #if isGSM || isWifi || isEthernet
    // replace spaces with plus for proper URL
    for (int i = 0; dataset[i] != 0; i++) {
      if (dataset[i] == ' ') dataset[i] = '+';
    }

    // calculate size of uploadUrl
    char uploadUrl[
      +sizeof(uploadPath)
      +sizeof(dataset)
    ];
    // make uploadUrl
    snprintf(uploadUrl, sizeof(uploadUrl), "%s%s",
      uploadPath,
      dataset
    );
    // debug
    #if isDebug
      Serial.println(uploadUrl);
    #endif
  #endif

  #if isGSM
    char buffer[50];
    memset(buffer, 0, sizeof(buffer));
    gprsbee.doHTTPGET(APN, uploadUrl, buffer, sizeof(buffer));
  #endif

  #if isWifi
    // try max. 15x to connect
    for (int i = 0; i < maxConnectionAttempts; i++) {
      // wait for WiFi connection
      if ((WiFiMulti.run() == WL_CONNECTED)) {
        // debug WIfi
        #if isDebug
          Serial.println();
          Serial.print(F("WiFi connected! IP address: "));
          Serial.print(WiFi.localIP());
          Serial.println();
        #endif

        HTTPClient http;
        // configure traged server and url
        // do NOT use "http://" in the domain!
        //http.begin("192.168.1.12", 443, "/test.html", true, "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin(uploadUrl);  // HTTP

        #if isDebug
          Serial.print(uploadUrl);
          Serial.print(F("  |  "));
          Serial.print(sizeof(uploadUrl));
          Serial.println();

          Serial.print(F("[HTTP] GET...\n"));
        #endif
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if (httpCode > 0) {
          // HTTP header has been send and Server response header has been handled
          #if isDebug
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);
          #endif
           // file found at server
          if(httpCode == 200) {
            String payload = http.getString();
            #if isDebug
              Serial.print("Status: ");
              Serial.println(payload);
            #endif
          }
        }
        else {
          #if isDebug
            Serial.print(F("[HTTP] GET... failed, no connection or no HTTP server\n"));
          #endif
        }
        http.end();
        break;
      }
      // not succesfully connected
      else {
        #if isDebug
          Serial.print(".");
        #endif
        // wait 400 ms before retry
        delay(400);
      }
    }
    // go sleeping
    unsigned long runningTime = millis() * 1000UL;  // in us!
    unsigned long sleepTime = (updateInterval * 1000UL*1000UL) - runningTime;  // s (*ms*us)
//    unsigned long sleepTime = (updateInterval * 1000UL*1000UL);  // s (*ms*us)

    // sometimes we need this adjustment
    if (sleepTime > updateInterval * 1000UL*1000UL){
      sleepTime = updateInterval * 1000UL*1000UL;
    }

    #if isDebug
      Serial.println();
      Serial.println(millis());
      Serial.print("running time: ");
      Serial.println(runningTime);
      Serial.print("sleep time: ");
      Serial.println(sleepTime);
      Serial.print("total: ");
      Serial.println((runningTime + sleepTime));
    #endif

    Serial.flush();
    ESP.deepSleep(sleepTime, WAKE_RF_DEFAULT);  // us! micro(!) seconds, so sleeping for 1 minute would be: 1000us*1000ms *60*1 = 1 sec * 60 sec * 1 min
//    delay(10000);  // sleep for 10 s
  #endif

  #if isSD
    writeToSd(dataset);
  #endif

  // manage sleeping and wake up
  //
  #if isRTC
    #if isWifi
      // intentionally blank
      // no special lib for ESP8266
    #else
      // prepare next interrupt time
      RTC.clearINTStatus();  // this function call is a must to bring INT pin high after an interrupt
      RTC.enableInterrupts(interruptTime.hour(),interruptTime.minute(),interruptTime.second());  // set the interrupt (h,m,s)
      attachInterrupt(0, wakeUp, LOW);  // enable INT0, required to handle level triggered interrupts

      // go sleeping

      // debug
      #if isDebug
        Serial.print(F("\nSleeping ... "));
      #endif
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
      #if isDebug
        Serial.println(F("Awake from sleep.\n"));
      #endif
    #endif
  #endif
}
