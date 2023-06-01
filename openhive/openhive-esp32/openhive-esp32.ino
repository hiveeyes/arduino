/*
  Open Hive | ESP32 - TTGO T-Call SIM800C
  ---------------------------------------
  sensors   | Scale          / HX711
            | Temperature    / DS18B20
            | Humidity [...] / BME280
  monitor   | Timer          / TPL5110
            | Battery        / AXP20X (T-Call)
            | SD 
  transport | GSM  via HTTP POST or GET / SIM800 (POST todo)
            | WiFi via HTTP POST or GET / ESP32 (POST todo)
            | MQTT (todo)
  other     | captive portal (todo)
            | OTA update (todo)
            | local temporary storage (todo)

  Copyright (C) 2016-2021 by Clemens Gruber 

  2021-06 Clemens Gruber | code based on older Open Hive sketches
                         | initial version 
  
  
  GNU GPL v3 License 
  ------------------
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  Licence can be read at                               
  http://www.gnu.org/licenses/gpl-3.0.txt


  use this pins for ...
  ---------------------
  HX711         13 - Dout 
                15 - SCK
                
  DS18B20       14 - one-wire array data

  BME280 / I2C  21 - SDA
                22 - SCL 
  - BME280: 0x76 (or 0x77) I2C address (in the most cases) 
  - AXP192, 0x34 I2C address on the same bus 
  
  TPL5110        2 - timer done
  
  
  install dependent libraries
  ---------------------------
  
  - Wire  // ESP32 core 
    
  - TinyGSM
    https://github.com/vshymanskyy/TinyGSM or
    http://librarymanager/all#TinyGSM+Arduino+library
    
  - StreamDebugger
    https://github.com/vshymanskyy/StreamDebugger or 
    http://librarymanager/all#StreamDebugger

  - AXP202X
    https://github.com/lewisxhe/AXP202X_Library
    not direkt accessible via Arduino library manager
    
  - HX711.h
    https://github.com/bogde/HX711 or
    http://librarymanager/all#HX711+Avia+Semiconductor
    
  - RunningMedian.h
    https://github.com/RobTillaart/RunningMedian or
    http://librarymanager/all#RunningMedian
    
  - SparkFun BME280
    https://github.com/sparkfun/SparkFun_BME280_Arduino_Library or 
    http://librarymanager/all#SparkFun+BME280
    
  - OneWire / for DS18B20
    https://github.com/PaulStoffregen/OneWire or
    http://librarymanager/all#OneWire+temperature+sensors
    
  - DallasTemperature / for DS18B20 also
    https://github.com/milesburton/Arduino-Temperature-Control-Library or
    http://librarymanager/all#DallasTemperature+ICs

  - SdFat
    https://github.com/greiman/SdFat+FAT16/FAT32/exFAT or  // früher Seeduino Stalker 
    http://librarymanager/all#SdFat
    
  - ArduinoHttpClient
    https://github.com/arduino-libraries/ArduinoHttpClient or  
    http://librarymanager/all#ArduinoHttpClient

--

    later? 

  - IOtWebConf
    https://github.com/prampec/IotWebConf or 
    http://librarymanager/all#IotWebConf
    
  - MQTT
  - JSON 
  
*/

// -------------------------+------
// variables you can modify | START
// -------------------------+------

// ** what components are in use?
// ------------------------------

// comment all components you are not using 
#define isTtgoTCall  // TTGO T-Call SIM800C, for the SIM800_C_ version only! 
//#define isOtherEsp32
#define isScale  // HX711
//#define isTemperatureArray  // DS18B20
#define isHumidityTemperaturePressure  // BME280
#define isRTC  // define for ESP32 also! sets timestamp on server 
//#define isSD 
#define isBatteryMonitor  // via AXP20X on T-Call SIM800C
#define isGSM  // SIM800C 
//#define isWifi
#define isDebug           // general debug information via Serial 
//#define isDebugGsm        // debug information from the GSM module via TinyGSM
//#define isDebugGsmAt      // output AT commands and responses from the GSM module to the serial console
#define isDebugGsmNetLED  // red SIM800 LED for _network_ connect, deactivate to reduce current consumption
// --GsmStatusLED--       // red SIM800 LED for _status_ cannot be turned off, only physically removable
#define isDebugGreenOnboardLED  // green onboard LED for connect, deactivate to reduce current consumption


// ** RTC / timekeeping
// --------------------

// update interval for sensor reading in seconds 
// e.g. for 2 minutes (2 * 60 s): unsigned long updateInterval = 2 * 60;  // (d*h)*m*s
unsigned long long updateInterval = 4 * 60 * 60;  // (d*h*) m*s

// could be also controlled by external timer breakout!! 
// current consumption with timer 40 n(!)A vs. 600 uA with TTGO deep sleep only

// keep in mind that very low values make no sense here!  
// sensor reading takes time, scale reads load multiple to 
// eliminate outlier so be sure that the update intervall is
// below sum of sensor reading and waiting time!!

// ** upload server
// ----------------

#if defined (isGSM) || defined (isWifi) 

  // server, path, port, get vs. post, http vs. https
  // ------------------------------------------------
  
  // example.com / GET
//  const char uploadServer[] = "example.com";
  // build char along this upload scheme by hand "/path/script.php?beekeeper=[your-user]&beekeeper-authkey=[your-key]&node=[your-node-number]&dataset=";
  
  // for swarm.hiveeyes / POST
  const char uploadServer[] = "swarm.hiveeyes.org";
  const char uploadPath[]   = "/api-notls/hiveeyes/testdrive/beekeeper-name/hive-number/data";  
  
  // for testing via ptsv2.com" / POST
//  const char uploadServer[] = "ptsv2.com";
//  const char uploadPath[]   = "/t/[your-toilet]/post";

  //#define isHttpS  // uncomment this in case you will use http_s_ 
  //const int  uploadPortHttpS    = 443;  // for http_s_ connections 
  const int  uploadPort         = 80;  // for http (not "s") connections
 
  // GET or POST
//  #define isGet   // use this for HTTP GET
  #define isPost  // use this for HTTP POST
#endif


// ** GSM / GPRSbee
// ----------------

#ifdef isGSM
  // your GPRS credentials (leave empty, if missing)
  const char apn[]      = "iot.1nce.net";  // your APN
  const char gprsUser[] = "";  // user
  const char gprsPass[] = "";  // password
  const char simPIN[]   = "";  // SIM card PIN, if any

  // do we need this???
  // flag to force SSL client authentication, if needed
  //#define TINY_GSM_SSL_CLIENT_AUTHENTICATION

  // define the serial console for debug output
  // tinyGsm debug to serial and / or dump at commands 
  #ifdef isDebugGsm
    #define TINY_GSM_DEBUG Serial
  #endif
  #ifdef isDebugGsmAt
    #define DUMP_AT_COMMANDS
  #endif
  
  // define the hardware serial for communicating with the SIM800 module
  #define SerialAT Serial1
  
  // Configure TinyGSM library
  #define TINY_GSM_MODEM_SIM800    // modem type is SIM800
  #define TINY_GSM_RX_BUFFER 1024  // set RX buffer to 1 Kb
#endif


// ** WiFi
// -------

#ifdef isWifi
  // specify your SSID an PW here, specific for your local WiFi
  // multiple APs possible! 
  #define WLAN_SSID_1  "[your-1st-ssid]"
  #define WLAN_PW_1    "[your-1st-pw]"
  
  #define WLAN_SSID_2  "[your-2nd-ssid]"
  #define WLAN_PW_2    "[your-2nd-pw]"
#endif


// ** battery management
// --------------------------- 

#ifdef isBatteryMonitor 
  float voltageBatteryThreshold = 3.62;  // in case voltage is below this threshold node will go immediately in deep sleep 
                                         // to save battery and wait for battery charging via solar or USB 
  float voltageBattery = 220;            // :-) will be replaced with AXP reading
  float voltageUsb = 220;                // :-) will be replaced with AXP reading
#endif


// ** load cell characteristic
// --------------------------- 

#ifdef isScale
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
  const long loadCellZeroOffset = -18058;
  const long loadCellKgDivider  = -21324;

  // wait between samples
  // 3 sec is a good delay so that load cell did not warm up 
  // too much and external random influences like wind has time 
  // to go so that the next sample is more valid
  const int waitTimeLoadSamples = 3; 
  
  // pins HX711
  const int scalePinDout = 13;  // Dout  
  const int scalePinSck  = 15;  // SCK
#endif


// ** median statistics (for weight / load cell)
// ---------------------------------------------

#ifdef isScale 
  // number of readings for median samples
  // 5 is a good compromise between time cost and stable output
  const int weightSamplesNumber = 5; 
#endif


// ** humidity and temperature / DHTxx
// -----------------------------------

#ifdef isHumidityTemperaturePressure 
  // how many BME280 devices are connected?
  const int humidityNumDevices = 1;
  // I2C hardware adresses for BME280 devices (in hex, leading "0x"!)
  const int humidityPins[humidityNumDevices] = {0x76}; 
#endif


// ** temperature array / DS18B20 
// ------------------------------

#ifdef isTemperatureArray
  // number of temperature devices on bus 
  const int temperatureNumDevices = 5; 
  // order on physical array of temperature devices 
  // the order is normally defined by the device id hardcoded in 
  // the device, you can physically arrange the DS18B20 in case you 
  // know the ID and use here {0,1,2,3,4 ... or you can try out what 
  // sensor is on wich position and adjust it herer accordingly 
  const int temperatureOrderDevices[temperatureNumDevices] = {0,1,2,3,4};
  // resolution for all devices (9, 10, 11 or 12 bits)
  const int temperaturePrecision = 12; 
  // pin for the temperature sensors
  const int temperaturePin = 14;
#endif


// ** SD card
// ----------

#ifdef isSD 
  // data upload to server, path and file
  const char sdStoragePath[]  = "/upload/";
  const char sdStorageFile[]  = "data.txt";
  
  // data archive, path 
  const char sdStorageArchivePath[] = "/archive/";
#endif


// ** dataset CSV header 
// ---------------------

// define header für CSV dataset my hand, could be used for database description or variable naming
// do not use spaces before or after an comma 
//const char datasetCsvHeader[] = "Date/Time,Weight,Outside Temp,Outside Humid,Inside Temp,Inside Humid,H1 Temp,H2 Temp,H3 Temp,H4 Temp,H5 Temp,Voltage"; 
//const char datasetCsvHeader[] = "Datum/Zeit,Gewicht,Stock-Temperatur,Stock-Feuchtigkeit,Luftdruck,Brutraum-Temperatur 1,Brutraum-Temperatur 2,Brutraum-Temperatur 3,Brutraum-Temperatur 4,Brutraum-Temperatur 5,Batterie-Spannung"; 
const char datasetCsvHeader[] = "Datum/Zeit,Gewicht,Stock-Temperatur,Stock-Feuchtigkeit,Luftdruck,Batterie-Spannung"; 

// -------------------------+----
// variables you can modify | END
// -------------------------+----


// ** datasets CSV and key-value
// -----------------------------

char datasetCsv[255] = "";
char datasetWithVar[255] = "";

char uploadPathPlusDatasetCsv[255] = "";

// SD card
#ifdef isSD 
  // #include <SD.h>
  #include <SdFat.h>
  SdFat SD;
  const int sdPinCs = 10; 
  
  // some simplificatiion for your convenience 
  // calculate size of sdStoragePathPlusFile
  char sdStoragePathPlusFile[
    +sizeof(sdStoragePath)
    +sizeof(sdStorageFile)
  ];
#endif


// pin definition for TTGO T-Call version SIM800C_AXP192_VERSION_20200609 only!
#ifdef isTtgoTCall
  // SIM800 
  #define MODEM_PWRKEY          4
  #define MODEM_POWER_ON       25
  #define MODEM_TX             27
  #define MODEM_RX             26
  #define MODEM_DTR            32
  #define MODEM_RI             33
  
  // I2C for AXP192 power management IC
  // note: I2C address for AXP192 is 0x34U (hard coded in the AXP library)
  #define I2C_SDA              21
  #define I2C_SCL              22
  
  // other 
  // timer pin for TPL5110 (Sparkfun Nano Power Timer)
  #define timerPinDone          2  // double checkt to _not_ use a ESP32 pin with PWM signal at boot, 
                                   // see https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
  // green onboard LED
  #define LED_GPIO             12
  #define LED_ON              LOW  // caution low = on
  #define LED_OFF            HIGH
  
  
  // libraries 
  // I2C for AXP and BME280
  #include <Wire.h>

  #if defined (isGSM) || defined (isWifi) 
    // ArduinoHttpClient for GPRS and WiFi
    #include <ArduinoHttpClient.h>
  #endif
    
  // power management via AXP192 for TTGO T-Call 
  // version SIM800C_AXP192_VERSION_20200609 only! 

  // already done -- #include <Wire.h>
  #include <axp20x.h>
  
  AXP20X_Class axp;

// power management function
bool setupPowerManagement()
{
  // called in setup() already -- Wire.begin();
  
  // note: I2C address for AXP192 is 0x34U (hard coded in the AXP library)
  int ret = axp.begin(Wire, AXP192_SLAVE_ADDRESS);
  
  if (ret == AXP_FAIL) {
    Serial.println("AXP Power begin failed");
    return false;
  }

  //! Turn off unused power
  axp.setPowerOutPut(AXP192_DCDC1, AXP202_OFF);
  axp.setPowerOutPut(AXP192_LDO2,  AXP202_OFF);
  axp.setPowerOutPut(AXP192_LDO3,  AXP202_OFF);
  axp.setPowerOutPut(AXP192_DCDC2, AXP202_OFF);
  axp.setPowerOutPut(AXP192_EXTEN, AXP202_OFF);

  //! Do not turn off DC3, it is powered by esp32
  // axp.setPowerOutPut(AXP192_DCDC3, AXP202_ON);

  // LED charging indicator
  // set the charging indicator LED off to reduce current consumption
  // charging LED is the blue LED on the ESP32 module
  axp.setChgLEDMode(AXP20X_LED_OFF);

  // set the charging indicator to flash once per second
  //axp.setChgLEDMode(AXP20X_LED_BLINK_1HZ);


  // use axp192 adc get voltage info
  axp.adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
/*
  float vbus_v = axp.getVbusVoltage();
  float vbus_c = axp.getVbusCurrent();
  float batt_v = axp.getBattVoltage();
  Serial.printf("VBUS:%.2f mV %.2f mA, BATTERY: %.2f\n", vbus_v, vbus_c, batt_v);   
*/  
  // axp.getBattPercentage();   // axp192 is not support percentage, only axp202
  return true;
}
#endif

// reset reason 
#include <rom/rtc.h>

// GSM / GPRSbee
#ifdef isGSM
  #include <TinyGsmClient.h>

  #ifdef DUMP_AT_COMMANDS
    #include <StreamDebugger.h>
    StreamDebugger debugger(SerialAT, Serial);
    TinyGsm modem(debugger);
  #else
    TinyGsm modem(SerialAT);
  #endif


  // handle https vs. http connections 
  #ifdef isHttpS
    TinyGsmClientSecure client(modem);
  #else
    TinyGsmClient client(modem);  
  #endif


// modem functions
void setupModem()
{
  #ifdef MODEM_RST
    // Keep reset high
    pinMode(MODEM_RST, OUTPUT);
    digitalWrite(MODEM_RST, HIGH);
  #endif

  pinMode(MODEM_PWRKEY, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  // Turn on the Modem power first
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Pull down PWRKEY for more than 1 second according to manual requirements
  digitalWrite(MODEM_PWRKEY, HIGH);
  delay(100);
  digitalWrite(MODEM_PWRKEY, LOW);
  delay(1000);
  digitalWrite(MODEM_PWRKEY, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);

  // old GPRSbee
  // do we need switch off here also?
//    gprsbee.initAutonomoSIM800(Serial1, BEEDTR, -1, BEECTS);
//    gprsbee.off();  // make sure the GPRSbee is switched off to begin with
}

void turnOffNetlight()
{
  //Serial.println("turning off SIM800 red LED ...");
  modem.sendAT("+CNETLIGHT=0");
}
void turnOnNetlight()
{
  //Serial.println("turning on SIM800 red LED ...");
  modem.sendAT("+CNETLIGHT=1");
}

bool gsmMakeTCPConnection()
{
  // GSM startup
  /* do we need this here - after measuring - or can we call this in setup()?
     would be nice to power modem first when it is needed and waste no power before
  */
  setupModem();
  delay(1000);
  
  // initialize modem 
  #ifdef isDebug 
    Serial.println("initializing modem ...");
  #endif
  // restart takes quite some time, use modem.init() if you don't need the complete restart
  //modem.restart();
  modem.init();

  // turn on / off _network_ (status) LED to reduce current consumption
  #ifdef isDebugGsmNetLED
    turnOnNetlight();
  #else
    turnOffNetlight();
  #endif
  // the _status_ light cannot be turned off, only physically removable
  //turnOffStatuslight();

/*  
  // output modem information 
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);
*/

  // unlock SIM card with your PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }

/*
  // do we need waitForNetwork? 
  Serial.print("waiting for network ... ");
  if (!modem.waitForNetwork()) {
    Serial.println("fail");
    Serial.println();

    delay(100);
    return false;
  }
  Serial.println(" OK");

  if (modem.isNetworkConnected()) {
    //debug
    #ifdef isDebug 
      Serial.println("Connected to mobile network");
    #endif
    #ifdef isDebugGreenOnboardLED
      // When the network connection is successful, turn on the indicator
      digitalWrite(LED_GPIO, LED_ON);
    #endif
  }
*/
  // make TCP / GPRS connection 
  
  // connect to GPRS / APN
  //debug
  #ifdef isDebug 
    Serial.print(F("connecting to GPRS / APN "));
    Serial.print(apn);
    Serial.print(F("... "));
  #endif
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    #ifdef isDebug 
      Serial.println("failed");
      Serial.println();
    #endif
    
    delay(100);  // in case the GSM / GPRS connection could not enabled  ??? gsm_disconnect();
    return false;
  }
  #ifdef isDebug 
    Serial.println("ok");  
    Serial.println();
  #endif
  
  // When the network connection is successful, turn on the indicator
  #ifdef isDebugGreenOnboardLED
    // When the network connection is successful, turn on the indicator
    digitalWrite(LED_GPIO, LED_ON);
  #endif

  return true;
}
#endif


// Wifi via ESP32
#ifdef isWifi
  #include <WiFi.h>
  #include <WiFiMulti.h>

  // multiple APs possible 
  WiFiMulti wiFiMulti;

  // ESP32 as http client via ArduinoHttpClient (for GSM and WiFi)
  // already included  #include <ArduinoHttpClient.h>
  
  WiFiClient client;

  // maximal connection attempts 
  int maxConnectionAttempts = 10;

  
// WiFI functions
bool wifiMakeTCPConnection() 
{
  // try max. 10x to connect 
  #ifdef isDebug 
    Serial.print(F("connecting to Wifi ... "));
  #endif
  for (int i = 0; i < maxConnectionAttempts; i++) {
    // wait for WiFi connection
    if ((wiFiMulti.run() == WL_CONNECTED)) {
      // debug WIfi
      #ifdef isDebug 
        Serial.println(F("ok"));
        Serial.print(F("IP address: "));
        Serial.println(WiFi.localIP());
        Serial.println();
      #endif
      #ifdef isDebugGreenOnboardLED
        // When the network connection is successful, turn on the indicator
        digitalWrite(LED_GPIO, LED_ON);
      #endif

      return true; 
    }
    // not succesfully connected 
    else {
      #ifdef isDebug 
        Serial.print(".");
      #endif
      
      // wait 400 ms before retry 
      delay(400);

      // for last element only
      if (i == maxConnectionAttempts-1) {
        #ifdef isDebug 
          Serial.print(F(" failed, could not find WiFi\n"));
        #endif

        // give up
        return false;
      } 
    }
  }
}
#endif


// HttpClient parameter for GSM and Wifi
#if defined (isGSM) || defined (isWifi)
  #ifdef isHttpS
    HttpClient http(client, uploadServer, uploadPortHttpS);
  #else
    HttpClient http(client, uploadServer, uploadPort);
  #endif
#endif


// load cell
#ifdef isScale 
  #include <HX711.h>
  HX711 loadCell;  // create HX711 object
#endif

// median statistics (for weight)
#ifdef isScale 
  #include <RunningMedian.h>
  RunningMedian weightSamples = RunningMedian(weightSamplesNumber);  // create RunningMedian object
#endif

// humidity and temperature / BME280
#ifdef isHumidityTemperaturePressure 
  // already done -- #include <Wire.h>
  #include <SparkFunBME280.h>
  BME280 bme280;
  
  // in case the default pressure is not accurate adjust it (in Pa, not hPa)
  // see https://youtu.be/Wq-Kb7D8eQ4
  float bmePressureCallibFactor = 490.0; 
#endif

// temperature array / DS18B20 
#ifdef isTemperatureArray
  #include <OneWire.h>              // oneWire for DS18B20
  #include <DallasTemperature.h>    // DS18B20 itself 
  OneWire oneWire(temperaturePin);  // oneWire instance to communicate with any OneWire devices (not just DS18B20)
  DallasTemperature temperatureSensors(&oneWire);        // pass oneWire reference to DallasTemperature
  uint8_t deviceAddressArray[temperatureNumDevices][8];  // arrays for device addresses
  const char gradC[4]={' ','d','C','\0'};                // degree sign
#endif


// we need some variable as char to pass it to payload easily 
// timekeeping / RTC
#ifdef isRTC
  char timestampChar[20];  // should handle yyyy/mm/dd hh:mm:ss and null terminator 
#endif
// weight 
#ifdef isScale
  char weightMedianChar[9];  // should handle +-xxx.xxx and null terminator
#endif
// temperature and humidity / DHTxx
#ifdef isHumidityTemperaturePressure 
  char temperatureChar[humidityNumDevices][6];  // should handle +/-xx.x and null terminator
  char humidityChar[humidityNumDevices][6];  // should handle xxx.x and null terminator
  char dewpointChar[humidityNumDevices][6];  // should handle +/-xx.x and null terminator
  char pressureChar[humidityNumDevices][8];  // should handle [space]xxxx.x and null terminator
#endif
// temperature array / DS18B20
#ifdef isTemperatureArray 
  char temperatureArrayChar[temperatureNumDevices][6];  // should handle +/-xx.x and null terminator
#endif
// Lipo / battery 
#ifdef isBatteryMonitor 
  char voltageBatteryChar[6];  // should handle xx.xx and null terminator
  char voltageUsbChar[6];      // should handle xx.xx and null terminator
#endif


// setup program 
// -------------

void setup () {  
  // debug 
  Serial.begin(115200);
  #ifdef isDebug 
    // some new line after wakeup from sleep to separate garbage 
    Serial.flush();
    Serial.println();
    Serial.print(F("Open Hive | ESP32 - TTGO T-Call SIM800C"));
    Serial.println();
    Serial.print(F("---------------------------------------"));
    Serial.println();
  #endif

  // initialize green onboard LED and switch off / caution, HIGH is OFF
  // in case we call this not LED will always be on 
  pinMode(LED_GPIO, OUTPUT);
  digitalWrite(LED_GPIO, LED_OFF);

  // initialize timer pin for done
  pinMode(timerPinDone, OUTPUT);

  // T-Call needs some time to stabilize voltage in case it is switched on and feeded by the Nano Timer, e.g.
  // to read correct battery voltage. Without the delay he reads 0,00 V and switchs off wrongly.     
  // In case battery is connected directly to the T-Call - and powers the board always! - this delay is not necessary. 
  // (note: 800 ms is too less)
  delay(1000);
  
  // I2C initialization 
  // we need this for AXP and BME280 
  Wire.begin(I2C_SDA, I2C_SCL);

  // setup T-Call SIM800C power management
  #ifdef isTtgoTCall
    if (setupPowerManagement() == false) {
      #ifdef isDebug 
        Serial.println("Error setting up power management for T-Call");
      #endif
    }
  #endif

  // read battery voltage ... and go sleeping if too low
  #ifdef isBatteryMonitor 
    #ifdef isDebug 
      Serial.print("check battery level ... "); 
    #endif

    // read battery voltage
    getvoltageBattery();
    if (voltageBattery < voltageBatteryThreshold) 
    {
      #ifdef isDebug 
        Serial.print("battery voltage is: "); 
        Serial.print(voltageBatteryChar); 
        Serial.print(" V, and so under the threshold of "); 
        Serial.print(voltageBatteryThreshold, 2); 
        Serial.println(" V!"); 
      #endif

      // battery voltage is low, but is there a USB power connection?
      float voltageUsb = axp.getVbusVoltage() / 1000;  // axp.getVbusVoltage returns mV, we need V
      dtostrf(voltageUsb,5,2,voltageUsbChar);  // write to char array 

      if (voltageUsb > 4.5) 
      {
        #ifdef isDebug 
          Serial.print("... but USB voltage is: "); 
          Serial.print(voltageUsbChar); 
          Serial.println(" V, so I assume we can go on!"); 
          Serial.println(); 
        #endif
      }
      else {
        // go sleeping to save power and wait for sun to charge by solar
        #ifdef isDebug 
          Serial.println(); 
        #endif
        goSleeping();
      }
    }
  #endif 

  // reset reason not deep sleep but reset button?
  if (rtc_get_reset_reason(0) == POWERON_RESET) 
  {
    #ifdef isDebug 
      Serial.println("Reset reason POWERON_RESET!");
//      Serial.println("todo [Captive Portal], start Wifi as AP, online configuration form, ...");
      Serial.println(); 
    #endif
  }

  // SD card
  #ifdef isSD 
    // switchable power on D4 for SD
    pinMode(4, OUTPUT);

    // initialize SD card 
    // power SD  
    digitalWrite(4, LOW);  // LOW is power on 
    // see if the card is present and can be initialized
    if (!SD.begin(sdPinCs)) {
      #ifdef isDebug 
        Serial.println(F("SD card failed, or not present!"));
        Serial.println();
      #endif
    }
    #ifdef isDebug 
      Serial.println(F("SD card initialized!"));
      Serial.println();
    #endif
    
    // make upload directories on SD 
    if (!SD.mkdir(sdStoragePath)) {
      #ifdef isDebug 
        Serial.print(F("Failed to create "));
        Serial.print(sdStoragePath);
        Serial.print(F(" on SD card!"));
        Serial.println();
      #endif
    }
    #ifdef isDebug 
      Serial.print(F("Directory "));
      Serial.print(sdStoragePath);
      Serial.print(F(" on SD card created or already existing!"));
      Serial.println();
    #endif
    
    // switch off SD 
    digitalWrite(4, HIGH);  // caution, LOW is power on 

    
    // some simplificatiion for your convenience 
    // make sdStoragePathPlusFile
    snprintf(sdStoragePathPlusFile, sizeof(sdStoragePathPlusFile), "%s%s",
      sdStoragePath,
      sdStorageFile
    ); 
  #endif

  #ifdef isHumidityTemperaturePressure 
    // called in setup() already -- Wire.begin();
    
    // set I2C address for BME280, must be set before .begin() 
    bme280.setI2CAddress(0x76); 
    
    // initialize I2C communication to BME280 
    if (bme280.beginI2C() == false)
    {
      #ifdef isDebug 
        Serial.println("Error BME280 initialization, please check I2C address, wiring, ... !");
      #endif
    }
    
    // filter can be off or number of FIR coefficients to use:
    //  0, filter off
    //  1, coefficients = 2
    //  2, coefficients = 4
    //  3, coefficients = 8
    //  4, coefficients = 16
    bme280.setFilter(3); // some HW filter
  
    // OverSample can be:
    //  0, skipped
    //  1 through 5, oversampling *1, *2, *4, *8, *16 respectively
    bme280.setTempOverSample(2);
    bme280.setPressureOverSample(1);
    bme280.setHumidityOverSample(1);
  
    // use BME280 later in forced mode, so sleep for now
    bme280.setMode(MODE_SLEEP); 
  #endif

  // WiFi
  #ifdef isWifi
    wiFiMulti.addAP(WLAN_SSID_1, WLAN_PW_1);
    wiFiMulti.addAP(WLAN_SSID_2, WLAN_PW_2);
  #endif

  // some simplificatiion for your convenience 
  #if defined (isGSM) || defined (isWifi) 
    // update header
    // calculate size of uploadUrlHeader
    char uploadUrlHeader[
      +sizeof(uploadPath)
      +sizeof(datasetCsvHeader)
    ];
    // make uploadUrlHeader
    snprintf(uploadUrlHeader, sizeof(uploadUrlHeader), "%s%s",
      uploadPath,
      datasetCsvHeader
    );
  #endif
  
  #ifdef isSD
//    writeToSd(datasetCsvHeader); 
  #endif
  
  // weight 
  #ifdef isScale 
    loadCell.begin(scalePinDout, scalePinSck);
    loadCell.set_scale(loadCellKgDivider);  // this value is obtained by calibrating the scale with known weights; see the README for details
    loadCell.set_offset(loadCellZeroOffset);
    
    // weight debug
    #ifdef isDebug 
      Serial.print(F("Be patient, it could take some time ... "));
      Serial.println();
      Serial.println();
      Serial.print(F("Median sample for the load cell consists of "));
      Serial.print(weightSamples.getSize());
      Serial.print(F(" single readings."));
      Serial.println();
      Serial.print(F("We wait "));
      Serial.print(waitTimeLoadSamples);
      Serial.print(F(" seconds between each reading. So the waiting time in "));
      Serial.println();
      Serial.print(F("the current case is "));
      Serial.print(weightSamples.getSize() * waitTimeLoadSamples);
      Serial.print(F(" seconds!")); 
      Serial.println();
      Serial.println();
    #endif
  #endif
  
  // temperature array 
  #ifdef isTemperatureArray 
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
        #ifdef isDebug 
          Serial.print(F("Unable to find address for temperature array device ")); 
          Serial.print(i);
          Serial.println();
        #endif
      }
    }
  #endif
}


// functions for single devices 
// ---------------------------- 

// timestamp 
#ifdef isRTC 
  void getTimestamp() {
    // set timestamp later by server 
    memcpy(timestampChar, "timestamp-by-server", sizeof("timestamp-by-server"));
  }  
#endif

// SD card 
#ifdef isSD
  void writeToSd(char *data) {   
    // power SD
    digitalWrite(4, LOW);  // LOW is power on 
    
    // open file
    File dataFile = SD.open(sdStoragePathPlusFile, FILE_WRITE);
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(data);
      dataFile.close();
      // debug 
      #ifdef isDebug 
        Serial.println(F("Wrote dataset to SD!"));
      #endif
    }
    // if the file isn't open, pop up an error:
    else {
      // debug 
      #ifdef isDebug 
        Serial.print(F("Error opening on SD "));
        Serial.print(sdStoragePathPlusFile);
        Serial.println();
      #endif
    }
    // switch off SD 
    digitalWrite(4, HIGH);  // LOW is power on 
  }
#endif

// weight
#ifdef isScale 
  void getWeight() {
    #ifdef isDebug 
      Serial.print("read load cell sensor, round "); 
    #endif
    
    // clear running median samples 
    weightSamples.clear();
  
    // read x times weight and take median 
    // do this till running median sample is full  
    do {
      #ifdef isDebug 
        Serial.print(weightSamples.getCount() + 1); 
        // not for last element 
        if (weightSamples.getCount() < (weightSamples.getSize() - 1)) {
          Serial.print(F(" .. "));
        } 
        else{
          Serial.println(F(" done"));
        }        
      #endif

      // wait between samples
      Serial.flush();
      for (int i=0; i<waitTimeLoadSamples; i++) {
        // sleep for one second 
        delay(1000);
      }
      
      // power HX711 and load cell 
      loadCell.power_up();
      
      // read data input of ADS1231
      float weightKg = loadCell.get_units(); 
      
      // switch off ADS1231 and load cell 
      loadCell.power_down();  // put the ADC in sleep mode

      // add single value to runnig median sample
      weightSamples.add(weightKg);
    } while (weightSamples.getCount() < weightSamples.getSize());

    dtostrf(weightSamples.getMedian(), 8, 3, weightMedianChar);  // write to char array 
  }
#endif

// temperature array / DS18B20
#ifdef isTemperatureArray
  void getTemperature() {
    #ifdef isDebug 
      Serial.println("read temperature array sensor(s)"); 
    #endif
    
    // request temperature on all devices on the bus
    temperatureSensors.setWaitForConversion(false);  // makes it async
    // initiatie temperature retrieval
    temperatureSensors.requestTemperatures();

    // wait at least 750 ms for conversion 
    // to save power go to sleep for this delay 
    Serial.flush();  // flush serial before sleeping 
    // sleep for one second 
    delay(1000);


    // loop through each device, print out temperature 
    for (int i=0; i<temperatureNumDevices; i++) {
      // print temperature
      float temperatureC = temperatureSensors.getTempC(deviceAddressArray[i]);
      dtostrf(temperatureC, 5, 1, temperatureArrayChar[i]);  // write to char array 
    }
  }
#endif

// humidity and temperature / BME280
#ifdef isHumidityTemperaturePressure 
  void getHumidityTemperature() {
    #ifdef isDebug 
      Serial.println("read humidity, temperature, pressure sensor(s)"); 
    #endif
    
    // read humidity, temperature and pressure data  
    // loop through all devices 
    for (int i=0; i<humidityNumDevices; i++) {
      // wake up sensor and take reading
      bme280.setMode(MODE_FORCED); 
      while (bme280.isMeasuring()) 
      {
        // wait for sensor to finish measuring
      };
        
      // sensor is now back asleep but we get the data
//      Serial.print(" Dewpoint: ");
//      Serial.print(bme280.dewPointC(), 1);

      dtostrf(bme280.readTempC(), 5, 1, temperatureChar[i]);  // write to char array 
      dtostrf(bme280.readFloatHumidity(), 5, 1, humidityChar[i]);  // write to char array 
      dtostrf(bme280.dewPointC(), 5, 1, dewpointChar[i]);  // write to char array 
      dtostrf((bme280.readFloatPressure() + bmePressureCallibFactor) / 100, 7, 1, pressureChar[i]);  // write to char array 
    }
  }
#endif

// battery voltage 
#ifdef isBatteryMonitor 
  void getvoltageBattery() {
    #ifdef isDebug 
      Serial.println("read battery voltage"); 
      Serial.println(); 
    #endif
    voltageBattery = axp.getBattVoltage() / 1000;  // axp.getBattVoltage returns mV, we need V
    dtostrf(voltageBattery,5,2,voltageBatteryChar);  // write to char array 
  }
#endif

// handle data upload
void uploadDataToServer() 
{
  // upload data to server 
  // ... via http GET call / CSV dataset
  #ifdef isGet
    #ifdef isHttpS
      #ifdef isDebug 
        Serial.print(F("[HTTP_S_] GET ... "));
      #endif
      http.connectionKeepAlive();  // currently, this is needed for HTTPS
    #else
      #ifdef isDebug
        Serial.print(F("[HTTP] GET ... "));
      #endif
    #endif

    // make get call
    int err = http.get(uploadPathPlusDatasetCsv);
    if (err != 0) {
      #ifdef isDebug
        Serial.println(F("failed to connect"));
        Serial.println();
      #endif

      delay(100);
      return;
    }
  #endif
  //get end

  // ... or
  // via http POST call / key-value dataset
  #ifdef isPost
    #ifdef isDebug
      Serial.print(F("[HTTP] POST ... "));
    #endif
    String contentType = "application/x-www-form-urlencoded";
    //String postData = "time=2021%2D03%2D17T03%3A03%3A57Z&weight=85%2E00&temperature-outside=42%2E42&humidity-outside=84%2E84&temperature-inside=33%2E33&voltage=3%2E84";
    //String postData = "time=2021-03-17T03:03:57Z&weight=85.00&temperature-outside=42.42&humidity-outside=84.84&temperature-inside=33.33&voltage=3.84";
    String postData = datasetWithVar;
    
    // make post call
    int err = http.post(uploadPath, contentType, postData);
    if (err != 0) {
      #ifdef isDebug
        Serial.println(F("failed to connect"));
        Serial.println();
      #endif

      delay(100);
      return;
    }
  #endif
  //post end

  // output response status code  
  int status = http.responseStatusCode();
  #ifdef isDebug 
    Serial.print(F("code: "));
    Serial.println(status);
    Serial.println();
  #endif
  
  if (status < 0) {

    delay(100);
    return;
  }

/* 
 Serial.println(F("Response Headers:"));
  while (http.headerAvailable()) {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    Serial.println("    " + headerName + ": " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    Serial.print(F("Content length is: "));
    Serial.println(length);
  }
  if (http.isResponseChunked()) {
    Serial.println(F("The response is chunked"));
  }
*/
  // output response body 
  #ifdef isDebug 
    String body = http.responseBody();
    Serial.println(F("response:"));
    Serial.println(body);
    Serial.println();
  #endif
  
//    Serial.print(F("Body length is: "));
//    Serial.println(body.length());
}


// shutdown modem
#ifdef isGSM
  void shutdownModem() {
    client.stop();
    #ifdef isDebug
      Serial.println(F("server disconnected"));
    #endif
    
    modem.gprsDisconnect();
    #ifdef isDebug
      Serial.println(F("GPRS disconnected"));
    #endif
    
    // switch off green status LED 
    #ifdef isDebugGreenOnboardLED
      digitalWrite(LED_GPIO, LED_OFF);    
    #endif


    // after all modem off
    modem.poweroff();
    #ifdef isDebug
      Serial.println(F("modem poweroff"));
      Serial.println();
      Serial.flush(); 
    #endif
  }
#endif

// manage and initialize (deep) sleep and wake up
#ifdef isRTC 
  void goSleeping() {
    // calclate sleep time
    unsigned long long runningTime = millis() * 1000ULL;  // in us!
    unsigned long long sleepTime = (updateInterval * 1000*1000ULL) - runningTime;  // s (*ms*us)
    // use this in case sleep time should be without substracted running time
    //unsigned long long sleepTime = (updateInterval * 1000*1000ULL);  // s (*ms*us)

    // sometimes we need this adjustment  
    if (sleepTime > updateInterval * 1000*1000ULL){
      //sleepTime = updateInterval * 1000*1000ULL; 
      // do not unse 0, this is too short for slepping, take 500! 
      sleepTime = 500; 
    }

    #ifdef isDebug 
      Serial.print("running time: \t");
      Serial.println((float)runningTime / 1000000, 1);
      Serial.print("sleep time: \t");
      Serial.println((float)sleepTime / 1000000, 1);
      Serial.print("total: \t\t");  
      Serial.println((float)(runningTime + sleepTime) / 1000000, 1);  
      Serial.println();
      
      Serial.println(F("sleeping ... "));
      Serial.println();
      Serial.println();
      Serial.flush(); 
    #endif

    // if timer
    
    // We're done! Set pin HIGH for done 
    // TTGO needs around 35 n(!)A (datasheet) with TPL5110 Nano Power Timer
    digitalWrite(timerPinDone, LOW); 
    digitalWrite(timerPinDone, HIGH);
    delay(10);
    digitalWrite(timerPinDone, LOW); // save power in case no timer is connected

        
    // if no timer 
    
    // go sleeping 
    // TTGO needs around 600 uA! 
    esp_sleep_enable_timer_wakeup(sleepTime);  // ESP32 deep sleep function expects time in us! 
    esp_deep_sleep_start();
    // waiting for wake up 
    // tzzzzz ... tzzzzz ...

    // without deep sleep use 
    delay(sleepTime / 1000);  // delay function uses ms not us as the ESP32 deep sleep function
  }
#endif


// main program 
// ------------ 

void loop () {
  // update data
  // update timestamp
  #ifdef isRTC 
    getTimestamp();
  #endif  
  
  // update weight 
  #ifdef isScale 
    getWeight();
  #endif
  
  // update humidity, temperature, pressure, BME280
  #ifdef isHumidityTemperaturePressure 
    getHumidityTemperature();
  #endif
  
  // update temperature array, DS18B20
  #ifdef isTemperatureArray 
    getTemperature(); 
  #endif
  
  // update battery monitoring (voltage)
  #ifdef isBatteryMonitor 
    getvoltageBattery();
  #endif

/*
  // build datasetCsv
  // calculate size of datasetCsv 
  char datasetCsv[0
    #ifdef isRTC
      +sizeof(timestampChar)
    #endif
    #ifdef isScale 
      +sizeof(weightMedianChar)
    #endif
    #ifdef isHumidityTemperaturePressure 
      +((sizeof(humidityChar[0]) + sizeof(temperatureChar[0])+ sizeof(pressureChar[0])) * humidityNumDevices)
    #endif
    #ifdef isTemperatureArray 
      +sizeof(temperatureArrayChar[0]) * temperatureNumDevices
    #endif
    #ifdef isBatteryMonitor 
      +sizeof(voltageBatteryChar)
    #endif
  ] = "";

  char datasetWithVar[255] = "";
*/  
  // make dataset 
/* // we do not need this for swarm.hiveeyes
  #ifdef isRTC
    strcat(datasetCsv, timestampChar);
    
    strcat(datasetWithVar, "timestamp=");
    strcat(datasetWithVar, timestampChar);
    
    // debug 
    #ifdef isDebug 
      Serial.print("timestamp: "); 
      Serial.println(timestampChar); 
    #endif
  #endif
*/
  #ifdef isScale 
// without timestamp we do not the "," 
//    strcat(datasetCsv, ","); 
    strcat(datasetCsv, weightMedianChar); 
// without timestamp we do not the "&"
//    strcat(datasetWithVar, "&weight=");
    strcat(datasetWithVar, "weight=");
    strcat(datasetWithVar, weightMedianChar);

    // debug 
    #ifdef isDebug
      Serial.print("weight: "); 
      Serial.println(weightMedianChar); 
    #endif
  #endif
  
  // loop through all HumidityTemperaturePressure devices 
  #ifdef isHumidityTemperaturePressure 
    #ifdef isDebug
      Serial.println(F("environment-sensor(s):"));
    #endif
    for(int i=0; i<humidityNumDevices; i++) {
      strcat(datasetCsv, ","); 
      strcat(datasetCsv, temperatureChar[i]);
      strcat(datasetCsv, ","); 
      strcat(datasetCsv, humidityChar[i]);
      strcat(datasetCsv, ","); 
      strcat(datasetCsv, pressureChar[i]);

      char counterNumber[2]; 
      sprintf(counterNumber,"%d",i+1);  // convert int counter to char 
      strcat(datasetWithVar, "&temperature-"); 
      strcat(datasetWithVar, counterNumber);
      strcat(datasetWithVar, "=");
      strcat(datasetWithVar, temperatureChar[i]);
      strcat(datasetWithVar, "&humidity-"); 
      strcat(datasetWithVar, counterNumber);
      strcat(datasetWithVar, "=");
      strcat(datasetWithVar, humidityChar[i]);
      strcat(datasetWithVar, "&pressure-"); 
      strcat(datasetWithVar, counterNumber);
      strcat(datasetWithVar, "=");
      strcat(datasetWithVar, pressureChar[i]);
    
      // debug 
      #ifdef isDebug       
        Serial.print(F("  temperature-")); 
        Serial.print(i+1);  // print index 
        Serial.print(F(": ")); 
        Serial.print(temperatureChar[i]);
        Serial.print(F(", "));
        
        Serial.print(F("humidity-")); 
        Serial.print(i+1);
        Serial.print(F(": ")); 
        Serial.print(humidityChar[i]);
        Serial.print(F(", "));
        
        //Serial.print(F("dewpoint-")); 
        //Serial.print(i+1);
        //Serial.print(F(": ")); 
        //Serial.print(dewpointChar[i]);
        //Serial.print(F(", "));
        
        Serial.print(F("pressure-")); 
        Serial.print(i+1);
        Serial.print(F(": ")); 
        Serial.print(pressureChar[i]);
        //Serial.print(F(", "));
        Serial.println(); 
      #endif 
    }
  #endif
  
  // loop through all TemperatureArray devices 
  #ifdef isTemperatureArray 
    #ifdef isDebug 
      Serial.println(F("temperature array: "));
    #endif
    for(int i=0; i<temperatureNumDevices; i++) {
      strcat(datasetCsv, ",");
      strcat(datasetCsv, temperatureArrayChar[i]);

      char counterNumber[2]; 
      sprintf(counterNumber,"%d",i+1);  // convert int counter to char 
      strcat(datasetWithVar, "&temp-arr-"); 
      strcat(datasetWithVar, counterNumber);
      strcat(datasetWithVar, "=");
      strcat(datasetWithVar, temperatureArrayChar[i]);
      
      // debug 
      #ifdef isDebug
        Serial.print(F("  temp-arr-"));
        Serial.print(i+1);  // print index 
        Serial.print(F(": "));
        Serial.print(temperatureArrayChar[i]);
        // not for last element 
        if (i < temperatureNumDevices-1) {
          Serial.println();
        }
      #endif 
    }
    // debug 
    #ifdef isDebug 
      Serial.println(); 
    #endif
  #endif
  #ifdef isBatteryMonitor 
    strcat(datasetCsv, ",");
    strcat(datasetCsv, voltageBatteryChar); 

    strcat(datasetWithVar, "&battery-voltage=");
    strcat(datasetWithVar, voltageBatteryChar);
    // debug 
    #ifdef isDebug 
      Serial.print("battery voltage: "); 
      Serial.println(voltageBatteryChar); 
    #endif
  #endif
  
  // debug 
  #ifdef isDebug 
    Serial.println(); 
    Serial.println("datasetCsv: "); 
    Serial.println(datasetCsv); 
    Serial.println(); 
  #endif
  
  
  // proper URL design and make path with dataset 
  #if defined (isGSM) || defined (isWifi) 
    // replace spaces in dataset with plus char for proper URL 
    // in datasetCsv ...
    for (int i = 0; datasetCsv[i] != 0; i++) {
      if (datasetCsv[i] == ' ') datasetCsv[i] = '+';
    }

    // ... and in datasetWithVar
    for (int i = 0; datasetWithVar[i] != 0; i++) {
      if (datasetWithVar[i] == ' ') datasetWithVar[i] = '+';
    }

    // debug 
    // print datasetWithVar, spaces replace by plus
    #ifdef isDebug 
      Serial.println("datasetWithVar: "); 
      Serial.println(datasetWithVar); 
      Serial.println(); 
    #endif

    // make uploadPathPlusDatasetCsv
    snprintf(uploadPathPlusDatasetCsv, sizeof(uploadPathPlusDatasetCsv), "%s%s",
      uploadPath, 
      datasetCsv
    );
    
    // debug 
    // print path with CSV dataset 
    #ifdef isGet
      #ifdef isDebug 
        Serial.println("upload path plus datasetCsv: ");
        Serial.println(uploadPathPlusDatasetCsv);
        Serial.println();
      #endif
    #endif
  #endif


  // write CSV data to SD
  #ifdef isSD
    writeToSd(datasetCsv); 
  #endif


  // connect via GPRS modem to internet
  #ifdef isGSM
    if (gsmMakeTCPConnection() == false) {
      // give up in case GPRS could not established
      shutdownModem();
      goSleeping();
    }
  #endif
  

  // connect via wifi to internet
  #ifdef isWifi
    if (wifiMakeTCPConnection() == false) {
      // give up in case GPRS could not established
      client.stop();
      #ifdef isDebug
        Serial.println(F("server disconnected"));
        Serial.println();
      #endif
      goSleeping();
    }
  #endif

  // send data to server
  uploadDataToServer();

  // shutdown GSM modem 
  #ifdef isGSM
    shutdownModem();
  #endif
  #ifdef isWifi
    // stop http client 
    client.stop();

    // debug
    #ifdef isDebug
      Serial.println(F("server disconnected"));
      Serial.println();
    #endif
    // switch off green status LED 
    #ifdef isDebugGreenOnboardLED
      digitalWrite(LED_GPIO, LED_OFF);    
    #endif
  #endif

  // go sleeping to save power
  goSleeping();
}  
