/*
   
                  Hiveeyes node-rfm69-beradio
 
   Code collects sensor data encodes them with HE_BERadio protocol 
   and sends it through HE_RFM69radio module to a gateway.
    
   Software release 0.5.3

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
   over a HE_RFM69radio module to a gateway, which sends the data over
   the internet or collects it in a own database.

   The sensor data could be temperature (via DS18B20 or DHT),
   humidity (DHT) or a load cell (H30A with HX711). Other sensors can 
   easily be added.

   After the sensor data is collected, it gets encapsulated in a
   HE_BERadio character string, which will be the transmitted payload. 
   HE_BERadio is a wrapper that makes use of EmBeEncode dictrionaries and 
   lists and adds some infrastructural metadata to it.
 
   This code is for use with HE_BERadio at the gateway side, e.g.
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

#include "config.h"
// Defines //
//#include "config.h"

// Libraries 
#ifdef HE_RFM69
    #include <RFM69.h>                      // https://github.com/LowPowerLab/RFM69
#endif

#include <HX711.h>
#include <DHT.h>
#include <RFM69_ATC.h>                  //https://github.com/LowPowerLab/RFM69
#include <SPI.h>                  
#include <SPIFlash.h>                   // https://github.com/LowPowerLab/SPIFlash
#include <avr/wdt.h>
#include <WirelessHEX69.h>              // https://github.com/einsiedlerkrebs/WirelessProgramming 
#include <EmBencode.h>                  // https://github.com/jcw/embencode
#include <LowPower.h>                   // https://github.com/LowPowerLab/LowPower
#include <HX711.h>                      // https://github.com/bogde/HX711
#include <OneWire.h>                    // https://github.com/PaulStoffregen/OneWire
#include <DallasTemperature.h>          // https://github.com/milesburton/Arduino-Temperature-Control-Library


// forward declarations //



// device initiation //

#ifdef ENABLE_ATC
    RFM69_ATC radio;
#else
    RFM69 radio;
#endif


#ifdef HE_TEMPERATURE
    void requestOneWire();
    OneWire oneWire(DS18B20_BUS);
    DallasTemperature sensors(&oneWire);
    float temp0 = 0, temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0;
#endif
#ifdef HE_HUMIDITY
    DHT dht;
    void requestDHT(int nr);
    float hum0 = 0, hum1 = 0;
    float temp5 = 0, temp6 = 0, temp7 = 0, temp8 = 0;
#endif 
#ifdef HE_SCALE
    void requestScale();
    float wght0 = 0, wght1 = 0;
#endif
#ifdef HE_BERadio
    void encodeSomeData(int BERfamily);
    int  BERfamily = 0;                     // the HE_BERadio family switch initiation payload fragmentation purpose. Later it uses "1" for temperature, "2" for humidity and "3" for weight and ifrastructural information.

#endif
#ifdef HE_RFM69 
    void sendData();
    byte sendSize=0;
    boolean requestACK = false;
    int rACK = 0;
#endif
#ifdef HE_SLEEP
    void Sleep(int minutes);
#endif

    void Blink(byte, int);

#ifdef HE_SCALE
    HX711 scale(HX711_DT, HX711_SCK);
#endif

#ifdef SPI_FLASH
    SPIFlash flash(FLASH_SS, FLASH_MANUFACTURER_ID);
#endif

// variables //

// common
char input = 0;
// radio
// long lastPeriod = -1;                // use in case of a time controled radio transmit routine
int TRANSMITPERIOD = 300;               //transmit a packet to gateway so often (in ms)

// Sensor variables

// Payload
typedef struct {
    char  buff[RFM69_MAX_PAYLOAD_LENGTH];
    int lenght;
} Payload;
Payload theData;

// setup function

void setup(){
    pinMode(LED, OUTPUT);               // setup onboard LED
    Serial.begin(SERIAL_BAUD);          // setup serial

    //dht.begin();

    // radio setup
    #ifdef DEBUG
        Serial.println();
        Serial.println("Start Hiveeyes node...");
        Serial.println();
        Serial.println("setting up radio");
    #endif

    #ifdef HE_RFM69 
        radio.initialize(RFM69_FREQUENCY,RFM69_NODE_ID,RFM69_NETWORK_ID);
        radio.encrypt(RFM69_ENCRYPTKEY);          //OPTIONAL
        char buff[50];
        #ifdef IS_RFM69HW
            radio.setHighPower();               //only for RFM69HW!
        #endif
        #ifdef RADIO_STATIC_POWER
            radio.setPowerLevel(RFM69_POWERLEVEL);
        #endif

        #ifdef ENABLE_ATC 
            radio.enableAutoPower(RFM69_TARGET_RSSI);
            #ifdef DEBUG_RADIO
                Serial.println("RFM69_ATC Enabled (Auto Transmission Control)");
            #endif
        #endif
        radio.sleep();
        #ifdef DEBUG_RADIO                   
            sprintf(buff, "Transmitting at %d Mhz...", RFM69_FREQUENCY==RF69_433MHZ ? 433 : RFM69_FREQUENCY==RF69_868MHZ ? 868 : 915);
            Serial.println(buff);
        #endif
    #endif


    // SPI Flash setup
  
    #ifdef SPI_FLASH
        #ifdef DEBUG_SPI_FLASH
            Serial.println("\nsetting up SPI Flash");
        #endif
        if (flash.initialize()){
            #ifdef DEBUG_SPI_FLASH
                Serial.print("SPI Flash Init OK ... UniqueID (MAC): ");
            #endif
            flash.readUniqueId();
            #ifdef DEBUG_SPI_FLASH
                for (byte i=0;i<8;i++){
                    Serial.print(flash.UNIQUEID[i], HEX);
                    Serial.print(' ');
                    }
                Serial.println();
            #endif 
            }
        else
        #ifdef DEBUG_SPI_FLASH
        Serial.println("SPI Flash Init FAIL! (is chip present?)");
        #endif
    #endif
    // Sensor setup
    #ifdef DEBUG_SENSORS
        Serial.println("\nsetting up Sensors");
    #endif
    #ifdef HE_TEMPERATUR
        sensors.begin();
    #endif
    #ifdef DEBUG_SENSORS
        Serial.println("OneWire set");
    #endif
    #ifdef HE_SCALE
        scale.set_offset(HX711_OFFSET);          // the offset of the scale, is raw output without any weight, get this first and then do set.scale  
        scale.set_scale(HX711_KNOWN_WEIGHT);           // this is the difference between the raw data of a known weight and an emprty scale 
        #ifdef DEBUG_SENSORS
            Serial.println("scale set");
        #endif
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
            Serial.print("HE_RFM69registers:");
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

    #ifdef HE_TEMPERATURE
        requestOneWire();
    #endif
    #ifdef HE_HUMIDITY
        requestDHT(1);
        requestDHT(2);
    #endif
    #ifdef HE_SCALE
        requestScale();
    #endif
    // beradio encode and send
    #ifdef HE_BERadio
        encodeSomeData(1);
    #endif
    #ifdef RADIO
        sendData();
        memset(&theData.buff[0], 0, sizeof(theData.buff));
    #endif
    #ifdef HE_BERadio
        encodeSomeData(2);
    #endif
    #ifdef RADIO
        sendData();
        memset(&theData.buff[0], 0, sizeof(theData.buff));
    #endif
    #ifdef HE_BERadio
        encodeSomeData(3);
    #endif
    #ifdef RADIO
        sendData();
        memset(&theData.buff[0], 0, sizeof(theData.buff));
    #endif

    #ifdef DEBUG_RADIO 
        Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
    #endif
    #ifdef DEBUG
        Serial.println("main loop ends here");
    #endif
    delay(100); 
    #ifdef HE_SLEEP
        Sleep(SLEEP_MINUTES);
    #endif

}

// get OneWire data //

void requestOneWire() {
    #ifdef DEBUG_SENSORS
    Serial.println();
    Serial.println("requesting OneWire devices");
    #endif
    sensors.requestTemperatures();
    temp0 = sensors.getTempCByIndex(0);
    temp1 = sensors.getTempCByIndex(1);
    temp2 = sensors.getTempCByIndex(2);
    temp3 = sensors.getTempCByIndex(3);
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

void encodeSomeData (int BERfamily) {           // this function encodes the values to a valid HE_BERadio character string with embencode
    EmBencode encoder;
    theData.lenght = 0;
    encoder.startDict();                        // start the dictionary as marking point of the payload string
	  encoder.push("_");                          // "_" marks the HE_BERadio profile
	  encoder.push(BERadio_profile);              
	  encoder.push("#");                          // "#" marks the Node/hive in your setup
	  encoder.push(HE_NODE_ID);
	  if (BERfamily == 1) {                       // beradio family switches to send a list of temp, or humidity or weight with infrastructural
	  	 encoder.push("t");                       // "t" marks the value source (temp) of the following list
		   encoder.startList();                     // starts a list
		       encoder.push(temp0 * 10000);         // embencode only send intergers. Float are multiplied by 10^x and divided by the HE_BERadioprotocoll,  see: https://hiveeyes.org/docs/beradio/
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
    if (theData.lenght >= RFM69_MAX_PAYLOAD_LENGTH){
        Serial.println("theData.lenght limit reached, aborting");
        return;
    }
    theData.buff[theData.lenght] = ch;
    theData.lenght +=1;
}

//    Wireless Updates via radio

/*
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

//   HE_RFM69 receiving

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

*/
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
   //DHT dht;
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
