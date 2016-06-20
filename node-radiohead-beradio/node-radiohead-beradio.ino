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

#include <avr/wdt.h>
#include <WirelessHEX69.h>              // https://github.com/einsiedlerkrebs/WirelessProgramming 


// forward declarations //



// device initiation //

#if ENABLE_ATC
    #include <RFM69.h>                      // https://github.com/LowPowerLab/RFM69
    #include <RFM69_ATC.h>                  //https://github.com/LowPowerLab/RFM69
    RFM69_ATC radio;
#else
    #include <RFM69.h>                      // https://github.com/LowPowerLab/RFM69
    RFM69 radio;
#endif


#if HE_TEMPERATURE
    #include <OneWire.h>                    // https://github.com/PaulStoffregen/OneWire
    #include <DallasTemperature.h>          // https://github.com/milesburton/Arduino-Temperature-Control-Library
    void requestOneWire();
    OneWire oneWire(DS18B20_BUS);
    DallasTemperature sensors(&oneWire);
    float temp0 = 0, temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0;
#endif
#if HE_HUMIDITY
    #include <DHT.h>
    DHT dht;
    void requestDHT(int nr);
    float hum0 = 0, hum1 = 0;
    float temp5 = 0, temp6 = 0, temp7 = 0, temp8 = 0;
#endif 
#if HE_SCALE
    #include <HX711.h>                      // https://github.com/bogde/HX711
    void requestScale();
    float wght0 = 0, wght1 = 0;
#endif
#if HE_BERadio
    #include <EmBencode.h>                  // https://github.com/jcw/embencode
    void encodeSomeData(int BERfamily);
    int  BERfamily = 0;                     // the HE_BERadio family switch initiation payload fragmentation purpose. Later it uses "1" for temperature, "2" for humidity and "3" for weight and ifrastructural information.

#endif
#if HE_RFM69 
    void sendData();
    byte sendSize=0;
    boolean requestACK = false;
    int rACK = 0;
#endif
#if HE_SLEEP
    #include <LowPower.h>                   // https://github.com/LowPowerLab/LowPower
    void Sleep(int minutes);
#endif

    void Blink(byte, int);

#if HE_SCALE
    #include <HX711.h>
    HX711 scale(HX711_DT, HX711_SCK);
#endif

#if SPI_FLASH
    #include <SPI.h>                  
    #include <SPIFlash.h>                   // https://github.com/LowPowerLab/SPIFlash
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
    #if DEBUG
        Serial.println();
        Serial.println("Start Hiveeyes node...");
        Serial.println();
        Serial.println("setting up radio");
    #endif

    #if HE_RFM69 
        radio.initialize(RFM69_FREQUENCY,RFM69_NODE_ID,RFM69_NETWORK_ID);
        radio.encrypt(RFM69_ENCRYPTKEY);          //OPTIONAL
        char buff[50];
        #if IS_RFM69HW
            radio.setHighPower();               //only for RFM69HW!
        #endif
        #if RADIO_STATIC_POWER
            radio.setPowerLevel(RFM69_POWERLEVEL);
        #endif

        #if ENABLE_ATC 
            radio.enableAutoPower(RFM69_TARGET_RSSI);
            #if DEBUG_RADIO
                Serial.println("RFM69_ATC Enabled (Auto Transmission Control)");
            #endif
        #endif
        radio.sleep();
        #if DEBUG_RADIO                   
            sprintf(buff, "Transmitting at %d Mhz...", RFM69_FREQUENCY==RF69_433MHZ ? 433 : RFM69_FREQUENCY==RF69_868MHZ ? 868 : 915);
            Serial.println(buff);
        #endif
    #endif


    // SPI Flash setup
  
    #if SPI_FLASH
        #if DEBUG_SPI_FLASH
            Serial.println("\nsetting up SPI Flash");
        #endif
        if (flash.initialize()){
            #if DEBUG_SPI_FLASH
                Serial.print("SPI Flash Init OK ... UniqueID (MAC): ");
            #endif
            flash.readUniqueId();
            #if DEBUG_SPI_FLASH
                for (byte i=0;i<8;i++){
                    Serial.print(flash.UNIQUEID[i], HEX);
                    Serial.print(' ');
                    }
                Serial.println();
            #endif 
            }
        else
        #if DEBUG_SPI_FLASH
        Serial.println("SPI Flash Init FAIL! (is chip present?)");
        #endif
    #endif
    // Sensor setup
    #if DEBUG_SENSORS
        Serial.println("\nsetting up Sensors");
    #endif
    #ifdef HE_TEMPERATUR
        sensors.begin();
    #endif
    #if DEBUG_SENSORS
        Serial.println("OneWire set");
    #endif
    #if HE_SCALE
        scale.set_offset(HX711_OFFSET);          // the offset of the scale, is raw output without any weight, get this first and then do set.scale  
        scale.set_scale(HX711_KNOWN_WEIGHT);           // this is the difference between the raw data of a known weight and an emprty scale 
        #if DEBUG_SENSORS
            Serial.println("scale set");
        #endif
    #endif
}
void loop(){

    #if DEBUG
        Serial.println();
        Serial.println("\n main loop begins here:");
    #endif
    // This part is optional, useful for some debugging.
    // Handle serial input (to allow basic DEBUGGING of FLASH chip)
    // ie: display first 256 bytes in FLASH, erase chip, write bytes at first 10 positions, etc
    #if DEBUG_SERIAL
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

    #if HE_TEMPERATURE
        requestOneWire();
    #endif

    #if HE_HUMIDITY
        requestDHT(1);
        requestDHT(2);
    #endif

    #if HE_SCALE
        requestScale();
    #endif
    // beradio encode and send
    #if HE_BERadio
        #if HE_TEMPERATUR
            encodeSomeData(1);
        #endif
    #endif
    #if HE_RADIO
        sendData();
        memset(&theData.buff[0], 0, sizeof(theData.buff));
    #endif
    #if HE_BERadio
        #if HE_HUMIDITY
            encodeSomeData(2);
        #endif
    #endif
    #if HE_RADIO
        sendData();
        memset(&theData.buff[0], 0, sizeof(theData.buff));
    #endif
    #if HE_BERadio
        #if HE_SCALE
            encodeSomeData(3);
        #endif
    #endif
    #if HE_RADIO
        sendData();
        memset(&theData.buff[0], 0, sizeof(theData.buff));
    #endif

    #if DEBUG_RADIO 
        Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");
    #endif
    #if DEBUG
        Serial.println("main loop ends here");
    #endif
    delay(100); 
    #if HE_SLEEP
        Sleep(SLEEP_MINUTES);
    #endif

}

// get OneWire data //

void requestOneWire() {
    #if HE_TEMPERATURE
        #if DEBUG_SENSORS
            Serial.println();
            Serial.println("requesting OneWire devices");
        #endif
            sensors.requestTemperatures();
            temp0 = sensors.getTempCByIndex(0);
            temp1 = sensors.getTempCByIndex(1);
            temp2 = sensors.getTempCByIndex(2);
            temp3 = sensors.getTempCByIndex(3);
        #if DEBUG_SENSORS
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
    #endif
}

// get weight data //

void requestScale(){
    #if HE_SCALE
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
    #endif
}

//  embencode encoding 

void encodeSomeData (int BERfamily) {           // this function encodes the values to a valid HE_BERadio character string with embencode
    #if HE_BERADIO
        EmBencode encoder;
        theData.lenght = 0;
        encoder.startDict();                        // start the dictionary as marking point of the payload string
    	  encoder.push("_");                          // "_" marks the HE_BERadio profile
    	  encoder.push(BERadio_profile);              
    	  encoder.push("#");                          // "#" marks the Node/hive in your setup
    	  encoder.push(HE_NODE_ID);
        #if HE_TEMPERATUR
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
        #endif
        #if HE_HUMIDITY
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
        #endif
        #if HE_SCALE
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
            #endif
        encoder.endDict();                           // ends the dictionary
        //#ifdef DEBUG_ENCODE
        Serial.println();
        Serial.println(theData.buff);
        //#endif
    #endif
}

// embencode vs. PAYLOAD control 

#if HE_BERadio
    void EmBencode::PushChar (char ch) {          
        if (theData.lenght >= RFM69_MAX_PAYLOAD_LENGTH){
            Serial.println("theData.lenght limit reached, aborting");
            return;
        }
        theData.buff[theData.lenght] = ch;
        theData.lenght +=1;
    }
#endif
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
*/
void sendData(){
    if (radio.sendWithRetry(RFM69_GATEWAY_ID, (const void*)(&theData.buff), sizeof(theData))){
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


//   Sleeping 

void Sleep(int minutes ){
    #if HE_SLEEP
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
    #endif
}

//   Getting DHT Data

void requestDHT(int nr){
    #if HE_HUMIDITY
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
    #endif
}
