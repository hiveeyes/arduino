/*
   
                  Hiveeyes node-rfm69-beradio
 
   Code collects sensor data encodes them with BERadio protocol 
   and sends it through RFM69 radio module to a gateway.
    
   Software release 0.16.0

   Copyright (C) 2014-2016  Richard Pobering <einsiedlerkrebs@ginnungagap.org>
   Copyright (C) 2014-2016  Andreas Motl <andreas.motl@elmyra.de>

   <https://hiveeyes.org>   

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

   Hiveeyes node sketch for Arduino based platforms   

   This is an Arduino sketch for the Hiveeyes bee monitoring system.
   The purpose is to collect vital data of a bee hive and transmit it
   over a RFM69 radio module to a gateway, which sends the data over
   the internet or collects it in a own database.

   The sensor data could be temperature (via DS18B20 or DHT),
   humidity (DHT) or a load cell (H30A with HX711). Other sensors can 
   easily be added.

   After the sensor data is collected, it gets encapsulated in a
   BERadio character string, which will be the transmitted payload. 
   BERadio is a wrapper that makes use of EmBeEncode dictrionaries and 
   lists and adds some infrastructural metadata to it.
 
   This code is for use with BERadio at the gateway side, e.g.
   as a forwarder from serial to mqtt.

   The creation of this code is strongly influenced by other projects, so
   credits goes to 
   them: <https://hiveeyes.org/docs/beradio/README.html#credits> 

   Feel free to adapt this code to your own needs.

-------------------------------------------------------------------------   

   Futher informations can be obtained at:

   hiveeyes                     https://hiveeyes.org/
   documentation                https://hiveeyes.org/docs/system/
   repository                   https://github.com/hiveeyes/
   beradio                      https://hiveeyes.org/docs/beradio/

-------------------------------------------------------------------------   

*/

// TODO:  
//        * put all settings into "#define" header (e.g. scale)
//        * fix #define DEBUG-switch issue
//        * clean up code

// Libraries 

#include <RFM69.h>                      // https://github.com/LowPowerLab/RFM69
#include <RFM69_ATC.h>
#include <RFM69_OTA.h>
#include <SPI.h>
#include <SPIFlash.h>                   // https://github.com/LowPowerLab/SPIFlash
#include <avr/wdt.h>
#include <EmBencode.h>                  // https://github.com/hiveeyes/embencode
#include <LowPower.h>                   // https://github.com/LowPowerLab/LowPower
#include <HX711.h>                      // https://github.com/bogde/HX711
#include <DHT.h>                        // https://github.com/markruys/arduino-DHT
#include <OneWire.h>                    // https://github.com/PaulStoffregen/OneWire
#include <DallasTemperature.h>          // https://github.com/milesburton/Arduino-Temperature-Control-Library

// Defines //

// BERadio & RFM69 defines

#define NODEID      2 
#define NETWORKID   100
#define GATEWAYID   1
#define FREQUENCY   RF69_868MHZ         // Match this with the version HopeRF-RFM69 module (others: RF69_433MHZ, RF69_868MHZ)
//#define IS_RFM69HW                    //uncomment only for RFM69HW! 
#define ENCRYPTKEY "www.hiveeyes.org"   //has to be same 16 characters/bytes on all nodes, not more not less!
#define ACK_TIME    30                  // # of ms to wait for an ack
#define MAX_PAYLOAD_LENGTH 61
//#define RADIO_STATIC_POWER            // uncomment if you wont to decrease the transmit power
#define POWERLEVEL 5                    // 5 for sending without an antenna, comment out, if you have an antenna installed
#define ENABLE_ATC                      // enables automatic transmission control (ATC) http://lowpowerlab.com/blog/2015/11/11/rfm69_atc-automatic-transmission-control/
#define TARGET_RSSI -70                 // set ATC level
#define BERadio_profile "h1"            // BERadio profile, define your own if you like, see: https://hiveeyes.org/docs/beradio/development/api.html?highlight=protocol#beradio.protocol.BERadioProtocol2
#define BLINKPERIOD 500                 // Period of LED blinking

#ifdef __AVR_ATmega1284P__
    #define LED           15            // most Megas have LEDs on D15
    #define FLASH_SS      23            // and FLASH SS on D23
#else
    #define LED           13            // Pro328mini has LEDs on D9
    #define FLASH_SS      8             // and FLASH SS on D8
#endif

//    SPIflash defines

// MANUFACTURER_ID             0x1F44 for adesto(ex atmel) 4mbit flash
//                             0xEF30 for windbond 4mbit flash
//                             0xEF40 for windbond 16/64mbit flash
//							   0x0102 for Spansion S25FL032P 32-Mbit 

#define FLASH_MANUFACTURER_ID 0x0102  // set accordingly, if you have a spi-flash on board 

// common defines

// Debugging                             // uncomment if you want to have Serial verbose Serial output
/*
#define DEBUG
#define DEBUG_RADIO
#define DEBUG_SPI_FLASH
#define DEBUG_SENSORS
#define DEBUG_SERIAL
#define DEBUG_ENCODE
*/
#define SERIAL_BAUD 115200              // set your approriate serial baud rate here and accordingly in your terminal
#define SLEEP                           // uncomment, for none sleeping periods
#define SLEEP_MINUTES 15                // set you sleeping rhythm, in about minutes
 
// Sensor defines

#define ONE_WIRE_BUS 9                  // DS18B20 data pin
#define TEMPERATURE_PRECISION 9         // DS18B20 value resolution
#define DHT_PIN1 7                      // DHT pin #1
#define DHT_PIN2 6                      // DHT pin #2
#define DHT_TYPE DHT22                  // DHT type (dht22)
#define HX711_SCK A0                    // SCK pin of hx711 
#define HX711_DT A1                     // DT pin fo hx711

// device initiation //

#ifdef ENABLE_ATC
RFM69_ATC radio;
#else
RFM69 radio;
#endif
HX711 scale;
SPIFlash flash(FLASH_SS, FLASH_MANUFACTURER_ID);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

// variables //

// common
char input = 0;
int  BERfamily = 0;                     // the BERadio family switch initiation payload fragmentation purpose. Later it uses "1" for temperature, "2" for humidity and "3" for weight and ifrastructural information.
// radio
byte sendSize=0;
boolean requestACK = false;
// long lastPeriod = -1;                // use in case of a time controled radio transmit routine
int TRANSMITPERIOD = 300;               //transmit a packet to gateway so often (in ms)

// Sensor variables
float temp0 = 0, temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0;
float temp5 = 0, temp6 = 0, temp7 = 0, temp8 = 0;
float hum0 = 0, hum1 = 0;
float wght0 = 0, wght1 = 0;
int rACK = 0;

// Payload
typedef struct {
    char  buff[MAX_PAYLOAD_LENGTH];
    int lenght;
} Payload;
Payload theData;

// setup function

void setup(){
    pinMode(LED, OUTPUT);               // setup onboard LED
    Serial.begin(SERIAL_BAUD);          // setup serial

// radio setup
    #ifdef DEBUG
    Serial.println();
    Serial.println("Start Hiveeyes node...");
    Serial.println();
    Serial.println("setting up radio");
    #endif
    radio.initialize(FREQUENCY,NODEID,NETWORKID);
    radio.encrypt(ENCRYPTKEY);          //OPTIONAL
    #ifdef IS_RFM69HW
    radio.setHighPower();               //only for RFM69HW!
    #endif
    #ifdef RADIO_STATIC_POWER
    radio.setPowerLevel(POWERLEVEL);
    #endif

    #ifdef ENABLE_ATC 
    radio.enableAutoPower(TARGET_RSSI);
    Serial.println("RFM69_ATC Enabled (Auto Transmission Control)");
    #endif
    radio.sleep();
    #ifdef DEBUG_RADIO                   
    char buff[50];
    sprintf(buff, "Transmitting at %d Mhz...", FREQUENCY==RF69_433MHZ ? 433 : FREQUENCY==RF69_868MHZ ? 868 : 915);
    Serial.println(buff);
    #endif


// SPI Flash setup
  
    #ifdef DEBUG
    Serial.println("\nsetting up SPI Flash");
    #endif

    #ifdef DEBUG_SPI_FLASH
    if (flash.initialize()){
        Serial.print("SPI Flash Init OK ... UniqueID (MAC): ");
        flash.readUniqueId();
        for (byte i=0;i<8;i++){
            Serial.print(flash.UNIQUEID[i], HEX);
            Serial.print(' ');
            }
        Serial.println();
    }
    else
    Serial.println("SPI Flash Init FAIL! (is chip present?)");
    #endif

// Sensor setup
    #ifdef DEBUG
    Serial.println("\nsetting up Sensors");
    #endif
    ds18b20.begin();

    #ifdef DEBUG_SENSORS
    Serial.println("OneWire set");
    #endif
    scale.begin(HX711_DT, HX711_SCK);
    scale.set_offset(8361975);          // the offset of the scale, is raw output without any weight, get this first and then do set.scale
    scale.set_scale(21901.f);           // this is the difference between the raw data of a known weight and an emprty scale 
    #ifdef DEBUG_SENSORS
    Serial.println("scale set");
    #endif
}

void loop(){

    #ifdef DEBUG
    Serial.println();
    Serial.println("\n main loop begins here:");
    #endif
    // This part is optional, useful for some debugging.
    // Handle serial input (to allow basic DEBUGGING of FLASH chip)
    // ie: display first 256 bytes in FLASH, erase chip, write bytes at first 10 positions, etc
    #ifdef DEBUG_SERIAL
    if (Serial.available() > 0) {
        input = Serial.read();
        if (input == 'd') //d=dump first page
        {
            Serial.println("Flash content:");
            uint16_t counter = 0;
            Serial.print("0-256: ");
            while(counter<=256){
            Serial.print(flash.readByte(counter++), HEX);
            Serial.print('.');
            }
         while(flash.busy());
         Serial.println();
    }
    else if (input == 'e')
    {
        Serial.print("Erasing Flash chip ... ");
        flash.chipErase();
        while(flash.busy());
        Serial.println("DONE");
        }
    else if (input == 'i')
    {
        Serial.print("DeviceID: ");
        word jedecid = flash.readDeviceId();
        Serial.println(jedecid, HEX);
        }
    else if (input == 'r')
    {
        Serial.print("Rebooting");
        resetUsingWatchdog(true);
        }
    else if (input == 'R')
    {
        Serial.print("RFM69 registers:");
        radio.readAllRegs();
        }
    else if (input >= 48 && input <= 57) //0-9
    {
        Serial.print("\nWriteByte("); Serial.print(input); Serial.print(")");
        flash.writeByte(input-48, millis()%2 ? 0xaa : 0xbb);
        }
    }
    #endif

// get sensor data 

    requestOneWire();
    requestDHT(1);
    requestDHT(2);
    requestScale();

// beradio encode and send

    encodeSomeData(1);
    sendData();
    memset(&theData.buff[0], 0, sizeof(theData.buff));
    encodeSomeData(2);
    sendData();
    memset(&theData.buff[0], 0, sizeof(theData.buff));
    encodeSomeData(3);
    sendData();
    memset(&theData.buff[0], 0, sizeof(theData.buff));

    #ifdef DEBUG_RADIO 
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
    #endif
    #ifdef DEBUG
    Serial.println("main loop ends here");
    #endif
    delay(100); 
    #ifdef SLEEP
    Sleep(SLEEP_MINUTES);
    #endif

}

// get OneWire data //

void requestOneWire() {
    #ifdef DEBUG_SENSORS
    Serial.println();
    Serial.println("requesting OneWire devices");
    #endif
    ds18b20.requestTemperatures();
    temp0 = ds18b20.getTempCByIndex(0);
    temp1 = ds18b20.getTempCByIndex(1);
    temp2 = ds18b20.getTempCByIndex(2);
    temp3 = ds18b20.getTempCByIndex(3);
    #ifdef DEBUG_SENSORS
    Serial.println("Readings:");
    Serial.print(temp0, 3);
    Serial.print("\t");
    Serial.print(temp1, 3);
    Serial.print("\t");
    Serial.print(temp2, 3);
    Serial.print("\t");
    Serial.print(temp3, 3);
    Serial.print("\t");
    Serial.println();
    #endif
}

// get weight data //

void requestScale(){
    #ifdef DEBUG_SENSORS
    Serial.println();
    Serial.println("\nrequesting HX711 scale");
    #endif
    scale.power_up();
    wght0 = scale.read_average(3);              // get the raw data of the scale
    wght1 = scale.get_units(3);                 // get the scaled data
    
    scale.power_down();
    #ifdef DEBUG_SENSORS
    Serial.println("Readings:");
    Serial.println(wght1, 3);
    Serial.println(wght1);
    #endif
}

//  embencode encoding 

void encodeSomeData (int BERfamily) {           // this function encodes the values to a valid BERadio character string with embencode
    EmBencode encoder;
    theData.lenght = 0;
    encoder.startDict();                        // start the dictionary as marking point of the payload string
	  encoder.push("_");                          // "_" marks the BERadio profile
	  encoder.push(BERadio_profile);              
	  encoder.push("#");                          // "#" marks the Node/hive in your setup
	  encoder.push(NODEID);
	  if (BERfamily == 1) {                       // beradio family switches to send a list of temp, or humidity or weight with infrastructural
	  	 encoder.push("t");                       // "t" marks the value source (temp) of the following list
		   encoder.startList();                     // starts a list
		       encoder.push(temp0 * 10000);         // embencode only send intergers. Float are multiplied by 10^x and divided by the BERadioprotocoll,  see: https://hiveeyes.org/docs/beradio/
		       encoder.push(temp1 * 10000);
		       encoder.push(temp2 * 10000);
		       encoder.push(temp3 * 10000);
           if (isfinite(temp6)){
		           encoder.push(temp6 * 10000);
           }  
	     encoder.endList();                       // ends a list
	  }
	 else if (BERfamily == 2) {
	     encoder.push("h");                       // "h" marks the value source (humidity)
	     encoder.startList();
           if (isfinite(hum0)){
	             encoder.push(hum0 * 100);
           }  
           if (isfinite(hum1)){
	             encoder.push(hum1 * 100);
           }  
	         encoder.endList();
	 }
	 else if (BERfamily == 3) {
	     encoder.push("w");                       // "w" marks the value source (weight)
		   encoder.startList();
		       encoder.push(wght0);                 // encodes the raw output of the hx711
		       encoder.push(wght1 * 1000);          // encodes the scaled output of hx711 and un-floats it
	     encoder.endList();
       encoder.push("r");                       // "r" marks the value source (RSSI)
		   encoder.push(radio.RSSI);
	     encoder.push("a");                       // "a" marks the value source (rACK)
		   encoder.push(rACK);
   }
   encoder.endDict();                           // ends the dictionary
   //#ifdef DEBUG_ENCODE
   Serial.println();
   Serial.println(theData.buff);
   //#endif
}

// embencode vs. PAYLOAD control 

void EmBencode::PushChar (char ch) {          
    if (theData.lenght >= MAX_PAYLOAD_LENGTH){
        Serial.println("theData.lenght limit reached, aborting");
        return;
    }
    theData.buff[theData.lenght] = ch;
    theData.lenght +=1;
}

//    Wireless Updates via radio

void wirelessUpdate(){
    // Check for existing RF data, potentially for a new sketch wireless upload  
    // For this to work this check has to be done often enough to be
    // picked up when a GATEWAY is trying hard to reach this node for a new sketch wireless upload
    if (radio.receiveDone()){
        CheckForWirelessHEX(radio, flash, true);
        #ifdef DEBUG_RADIO
        Serial.println("checking wireless Update");
        Serial.println("Got [");
        Serial.print(radio.SENDERID);
        Serial.print(':');
        Serial.print(radio.DATALEN);
        for (byte i = 0; i < radio.DATALEN; i++)
        Serial.print((char)radio.DATA[i], HEX);
        Serial.println();
        //  CheckForWirelessHEX(radio, flash, true);
        Serial.println();
        #endif
    }
    #ifdef DEBUG_RADIO
    else Serial.print('.');
    radio.sleep();
    #endif
}
void sendData(){
    if (radio.sendWithRetry(GATEWAYID, (const void*)(&theData.buff), sizeof(theData))){
        radio.sleep();
        rACK = 1;
        //#ifdef DEBUG_RADIO
        Serial.println(" sending done");
        //#endif
    }
    //#ifdef DEBUG_RADIO
    else Serial.print(" not send");
    //#endif
    radio.sleep();
    rACK = 0;
}

//   RFM69 receiving

void receivePackages(){
   //check for any received packets
   if (radio.receiveDone()){
       #ifdef DEBUG_RADIO
       Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
       for (byte i = 0; i < radio.DATALEN; i++)
           Serial.print((char)radio.DATA[i]);
           Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
       #endif
       if (radio.ACKRequested()){
           radio.sendACK();
           #ifdef DEBUG_RADIO
           Serial.print(" - ACK sent");
           #endif
           }
       Blink(LED,3);
       #ifdef DEBUG_RADIO
       Serial.println();
       #endif
       }
   radio.sleep();
}

//   LED Blink

void Blink(byte PIN, int DELAY_MS){
   pinMode(PIN, OUTPUT);
   digitalWrite(PIN,HIGH);
   delay(DELAY_MS);
   digitalWrite(PIN,LOW);
}

//   Sleeping 

void Sleep(int minutes ){
   int var = 0;
   float cycles = 0;
   cycles = minutes * 7.5;
   while(var < cycles){
       #ifdef DEBUG
       Serial.print("starting sleep cycle");
       delay(100);
       #endif
       // do something for SLEEP_TIMES
       var++;
       // put the processor to sleep for 8 seconds
       LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
       #ifdef DEBUG
       Serial.print("stopping sleep cycle");
       #endif
   }
}

//   Getting DHT Data

void requestDHT(int nr){
   DHT dht;
   int var = 1;
   if (nr == 1){
       dht.setup(DHT_PIN1);
       delay(2000);
       hum0 = dht.getHumidity();
       temp5 = dht.getTemperature();
       if (isnan(hum0) || isnan(temp5)) {
           while(var <= 5){
               var++;
               delay(2000);
               hum0 = dht.getHumidity();
               temp5 = dht.getTemperature();
               #ifdef DEBUG_SENSORS
               Serial.println("\nFailed to read from DHT sensor!");
               #endif
           }
       }
       #ifdef DEBUG_SENSORS
       Serial.print("Humidity: ");
       Serial.print(hum0);
       Serial.print(" %\t");
       Serial.print("Temperature: ");
       Serial.print(temp5);
       Serial.println();
       #endif
   } 
   else 
   if (nr == 2){
       dht.setup(DHT_PIN2);
       delay(2000);
       hum1 = dht.getHumidity();
       temp6 = dht.getTemperature();
       if (isnan(hum1) || isnan(temp6)) {
           while(var <= 5){
               var++;
               delay(2000);
               hum1 = dht.getHumidity();
               temp6 = dht.getTemperature();
               #ifdef DEBUG_SENSORS
               Serial.println("\nFailed to read from DHT sensor!");
               #endif
            }
       }
       #ifdef DEBUG_SENSORS
       Serial.print("Humidity: ");
       Serial.print(hum1);
       Serial.print(" %\t");
       Serial.print("Temperature: ");
       Serial.print(temp6);
       Serial.println();
       #endif
   } 
}
