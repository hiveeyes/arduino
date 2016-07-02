// vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
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

#if DEBUG_MEMORY
    #include <MemoryFree.h>
#endif

// device initiation //

#if ENABLE_ATC
    #include <RFM69.h>                      // https://github.com/LowPowerLab/RFM69
    #include <RFM69_ATC.h>                  //https://github.com/LowPowerLab/RFM69
    RFM69_ATC radio;
#else
    #include <RFM69.h>                      // https://github.com/LowPowerLab/RFM69
    RFM69 radio;
#endif

#if HE_CONTAINERS
    #include <new.cpp>
    #include <StandardCplusplus.h>
    #include <func_exception.cpp>
    #include <BERadio.h>
#endif

#if HE_TEMPERATURE
    #include <OneWire.h>                    // https://github.com/PaulStoffregen/OneWire
    #include <DallasTemperature.h>          // https://github.com/milesburton/Arduino-Temperature-Control-Library
    void readTemperature();
    OneWire oneWire(DS18B20_BUS);
    DallasTemperature sensors(&oneWire);
    #ifndef HE_CONTAINERS
        float temp0, temp1, temp2, temp3, temp4;
    #endif
#endif
#if HE_HUMIDITY
    #include <DHT.h>
    DHT dht1;
    DHT dht2; 
    void readHumidity();
    #ifndef HE_CONTAINERS
        float hum0 = 0, hum1 = 0;
        float temp5 = 0, temp6 = 0, temp7 = 0, temp8 = 0;
    #endif
#endif 
#if HE_SCALE
    #include <HX711.h>                      // https://github.com/bogde/HX711
    HX711 scale(HX711_DT, HX711_SCK);
    void readScale();
    #ifndef HE_CONTAINERS
        float wght0 = 0, wght1 = 0;
    #endif
#endif
#if HE_BERadio
    #include <EmBencode.h>                  // https://github.com/jcw/embencode
    #include <BERadio.h>
    BERadioMessage message(HE_HIVE_ID, "h1", 61);
#endif


#if CONT_HUM
    FloatList humL;
#endif
#if CONT_WGHT
    FloatList wghtL;
#endif
#if CONT_TEMP
    FloatList tempL;
#endif

#if HE_RFM69 
    void sendData();
    byte sendSize=0;
    boolean requestACK = false;
    int rACK = 0;
    // Payload
    typedef struct {
        char  buff[RFM69_MAX_MESSAGE_LENGTH];
        int lenght;
    } Payload;
    Payload theData;
#endif
#if HE_SLEEP
    #include <LowPower.h>                   // https://github.com/LowPowerLab/LowPower
    void Sleep(int minutes);
#endif

    void Blink(byte, int);



#if HE_FLASH
    #include <SPI.h>                  
    #include <SPIFlash.h>                   // https://github.com/LowPowerLab/SPIFlash
    SPIFlash flash(FLASH_SS, FLASH_MANUFACTURER_ID);
#endif

#if HE_RH95
    #include <RH_RF95.h>
    #include <RHReliableDatagram.h>
    #include <SPI.h>
    RH_RF95 rh95(RH95_SS, RH95_IRQ);
    uint8_t buf95[RH_RF95_MAX_MESSAGE_LEN];
    #if RH95_IS_TRANSCEIVER
        void transceive();
        RHReliableDatagram manager95(rh95, RH95_TRANSCEIVER_ID);
    #elif RH95_IS_GATEWAY
        RHReliableDatagram manager95(rh95, RH95_GATEWAY_ID);
    #else
        RHReliableDatagram manager95(rh95, RH95_NODE_ID);
    #endif
#endif

#if HE_RH69
    #include <RH_RF69.h>
    #include <RHReliableDatagram.h>
    #include <SPI.h>
    RH_RF69 rh69(RH69_SS, RH69_IRQ);
    uint8_t buf69[RH_RF69_MAX_MESSAGE_LEN];
    #if RH69_IS_TRANSCEIVER
        void transceive();
        RHReliableDatagram manager69(rh69, RH69_TRANSCEIVER_ID);
    #elif RH69_IS_GATEWAY
        RHReliableDatagram manager69(rh69, RH69_GATEWAY_ID);
    #else
        RHReliableDatagram manager69(rh69, RH69_NODE_ID);
    #endif
#endif

// variables //

// common
char input = 0;
// radio
// long lastPeriod = -1;                // use in case of a time controled radio transmit routine
int TRANSMITPERIOD = 300;               //transmit a packet to gateway so often (in ms)

// Sensor variables

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//                  setup function#
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void setup(){
    #if HE_DEBUG
        Serial.println(" setup begins here:\n");
        Serial.println("-----------------------\n");
    #endif
    pinMode(LED, OUTPUT);               // setup onboard LED
    Serial.begin(SERIAL_BAUD);          // setup serial
    #if DEBUG_MEMORY
        Serial.print("freeMemory()=");
        Serial.println(freeMemory());
    #endif

    //dht.begin();

    #if HE_DEBUG
        Serial.println();
        Serial.println("Start Hiveeyes node...");
        Serial.println();
        Serial.println("setting up radio");
    #endif

    // radio setup
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
    #if DEBUG_RADIO
        #if HE_RH69
            if (!manager69.init())
                Serial.println("init failed");
            else Serial.println("init rh69 done");
    
            if (!rh69.setFrequency(RH69_FREQUENCY))
                Serial.println("set Frequency failed");
            else Serial.println("rh69 frequency set");
        #elif HE_RH95
            if (!manager95.init())
                Serial.println("init failed");
            else Serial.println("init rh95 done");
    
            if (!rh95.setFrequency(RH95_FREQUENCY))
                Serial.println("set Frequency failed");
            else Serial.println("rh95 frequency set");
        #endif
    #else 
        #if HE_RH69
            manager69.init()
            rh69.setFrequency(RH69_FREQUENCY)
        #elif HE_RH95
            manager95.init()
            rh95.setFrequency(RH95_FREQUENCY)
        #endif
    #endif
 
    // SPI Flash setup
  
    #if HE_FLASH
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
        Serial.println("Onewire set\n");
    #endif
    #if HE_HUMIDITY
        dht1.setup(DHT_PIN1);
        dht2.setup(DHT_PIN2);
    #endif
    #if DEBUG_SENSORS
        Serial.println("DHT set\n");
    #endif
    #if HE_SCALE
        scale.set_offset(HX711_OFFSET);          // the offset of the scale, is raw output without any weight, get this first and then do set.scale  
        scale.set_scale(HX711_KNOWN_WEIGHT);           // this is the difference between the raw data of a known weight and an emprty scale 
        #if DEBUG_SENSORS
            Serial.println("scale set\n");
        #endif
    #endif
    #if BERadio
        message.set_mtu_size(61);
    #endif
    #if HE_DEBUG
        Serial.println(" setup ends here:\n");
        Serial.println("-----------------------\n");
    #endif
}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//                  loop function
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void loop(){
    #if DEBUG_MEMORY
        Serial.print("freeMemory()=");
        Serial.println(freeMemory());
    #endif
    #if HE_DEBUG
        Serial.println(" main loop begins here:\n");
        Serial.println("-----------------------\n");
    #endif

    // get sensor data 
    #if HE_TEMPERATURE
        readTemperature();
    #endif
    #if HE_HUMIDITY
        readHumidity();
    #endif
    #if HE_SCALE
        readScale();
    #endif
    // serialize and transmitt data
    #if HE_BERadio
    #if DEBUG_MEMORY
        Serial.print("freeMemory()=");
        Serial.println(freeMemory());
    #endif
        //BERadioMessage message(HE_HIVE_ID);
        #if DEBUG_BERadio
             Serial.println("filling floatlists\n");
             delay(200);
        #endif
        //message.add("t", tempL);
        //message.add("h", humL);
        message.add("w", wghtL);
    #if DEBUG_MEMORY
        Serial.print("freeMemory()=");
        Serial.println(freeMemory());
    #endif
        #if DEBUG_BERadio
             Serial.println("Floatlists filled\n");
             delay(200);
        #endif
        message.encode_and_transmit();
    #endif
    #if RH69_IS_TRANSCEIVER
        transceive();
    #endif
    #if HE_DEBUG
        Serial.println(" main loop ends here:\n");
        Serial.println("-----------------------\n");
    #endif
    #if HE_SLEEP
        delay(100); 
        Sleep(SLEEP_MINUTES);
    #endif

}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//                  user functions
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// get OneWire data //

#if HE_TEMPERATURE
    void readTemperature() {
        #if DEBUG_SENSORS
            Serial.println();
            Serial.println("requesting OneWire devices");
        #endif
        sensors.requestTemperatures();
        tempL.push_back(sensors.getTempCByIndex(0));
        tempL.push_back(sensors.getTempCByIndex(1));
        tempL.push_back(sensors.getTempCByIndex(2));
        tempL.push_back(sensors.getTempCByIndex(3));
        #if DEBUG_SENSORS
            for (double value: tempL){
                Serial.println(value);
            }
        #endif
    }
#endif

#if HE_HUMIDITY
    void readHumidity(){
        dht1.getHumidity();          // the first reading of DHT is the last one.
        dht1.getTemperature();          // the first reading of DHT is the last one.
        float humbuf0;
        float tempbuf0;
        humbuf0 = dht1.getHumidity();
        tempbuf0 = dht1.getTemperature();
        delay(2000);
        for(int i=0; i<=3; i++){
            if (!isnan(humbuf0) || !isnan(tempbuf0)){
                break;
            }
            delay(2000);
        }
        if (!isnan(humbuf0) || !isnan(tempbuf0)){
            humL.push_back(humbuf0);
            tempL.push_back(tempbuf0);
        }
        else{
            humL.push_back(BAD_VALUE);
            tempL.push_back(BAD_VALUE);
            #if DEBUG_SENSORS
                for (double value: tempL){
                    Serial.println(value);
                }
                for (double value: humL){
                    Serial.println(value);
                }
                Serial.println("could not get humidity\n");
            #endif
        } 
    }
#endif
// get weight data //
#if HE_SCALE
    void readScale(){
        #ifdef DEBUG_SENSORS
            Serial.println();
            Serial.println("requesting HX711 scale\n");
        #endif
        scale.power_up();
        wghtL.push_back(scale.read_average(3));              // get the raw data of the scale
        wghtL.push_back(scale.get_units(3));                 // get the scaled data
        scale.power_down();
        #ifdef DEBUG_SENSORS
            Serial.println("Readings:");
            for (double value: wghtL){
                Serial.println(value);
            }
        #endif
    }
#endif
#if HE_RFM69
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
#endif
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

#if HE_SLEEP
    void Sleep(int minutes ){
        int var = 0;
        float cycles = 0;
        cycles = minutes * 7.5;
        while(var < cycles){
            #ifdef DEBUG
                Serial.println("starting sleep cycle\n");
                delay(100);
            #endif
            var++;
            // put the processor to sleep for 8 seconds
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
            #ifdef DEBUG
                Serial.println("stopping sleep cycle\n");
            #endif
        }
    }
#endif

//   Getting DHT Data


#if RH95_IS_TRANSCEIVER && RH69_IS_TRANSCEIVER
    void transceive(){ 
        digitalWrite(9, HIGH);  //disable rf95 slave select
        if (manager69.available()){
            // Wait for a message addressed to us from the client
            uint8_t len69 = sizeof(buf69);
            uint8_t len95 = sizeof(buf95);
            uint8_t from;
            if (manager69.recvfromAck(buf69, &len69, &from)){
                Serial.print("got message from node: 0x");
                Serial.print(from, HEX);
                Serial.print(": ");
                Serial.println((char*)buf69);
                // Send a reply back to the originator node 
                uint8_t answerData[] = "And hello back to you";
                if (!manager69.sendtoWait(answerData, sizeof(answerData), from))
                    Serial.println("sendto69Wait failed");
                //transceive the message to gateway
                if (manager95.sendtoWait(buf69, sizeof(buf69), RH69_GATEWAY_ID)){
                    if (manager95.recvfromAckTimeout(buf95, &len95, 1000, &from)){
                        Serial.print("got message from gateway: 0x");
                        Serial.print(from, HEX);
                        Serial.print(": ");
                        Serial.println((char*)buf95);
                    }
                    else Serial.println("No reply from gateway");
                }
                Serial.println();
            }
        }
    }
#endif

#if RH69_IS_NODE
    void nodeSend(){
        // Send a message to manager_server
        #if RADIO_DEBUG
            if (manager69.sendtoWait(buf69, sizeof(buf69), RH69_GATEWAY_ID)){
                  // Now wait for a reply from the server
                  uint8_t len = sizeof(buf69);
                  uint8_t from;   
                  if (manager69.recvfromAckTimeout(buf69, &len, 2000, &from)){
                        Serial.print("got reply from : 0x");
                        Serial.print(from, HEX);
                        Serial.print(": ");
                        Serial.println((char*)buf69);
                  }
                  else{
                        Serial.println("No reply, is rf95_reliable_datagram_server running?");
                  }
            }
            else
            Serial.println("sendtoWait failed");
        #else
            manager69.sendtoWait(buf69, sizeof(buf69), RH69_GATEWAY_ID);
        #endif
    }
#endif

#if RH95_IS_GATEWAY
    void gatewayReceive(){
        uint8_t len = sizeof(buf95);
        uint8_t from;
        #if RADIO_DEBUG
        if (manager95.available()){
              // Wait for a message addressed to us from the client
              if (manager95.recvfromAck(buf95, &len, &from)){
                    Serial.print("got message from transmitter: 0x");
                    Serial.print(from, HEX);
                    Serial.print(": ");
                    Serial.println((char*)buf95);
              
                    // Send a reply back to the originator client
                    if (!manager95.sendtoWait(answerForwardData, sizeof(answerForwardData), from))
                      Serial.println("sendtoWait95 failed");
                  }
              Serial.println();
        }
        #else
        manager95.available();
        if (manager95.recvfromAck(buf95, &len, &from)){
            Serial.println((char*)buf95);
        }
        #endif
    }        
#endif

#if HE_BERadio
    void BERadioMessage::send(std::string payload) {
        #if DEBUG_BERadio
             Serial.println("enter message send\n");
             Serial.println("payload:\n");
             //printf((std::string*)payload); 
        #endif
        //const uint8_t[] = payload.c_str();
        //buf69[0] = (uint8_t)atoi(payload);
        //manager69.sendtoWait(buf69, sizeof(buf69), RH69_TRANSCEIVER_ID);
        //manager69.sendtoWait(payload.c_str(), payload.length(), RH69_TRANSCEIVER_ID);
        //uint8_t *buf69 = new uint8_t[payload.length() + 1];
        //strcpy(buf69, payload.c_str());
        //char payload;
          // Convert from char * to uint8_t
       
        //uint8_t payload_c = *(payload.c_str());
        #if DEBUG_BERadio
        //    Serial.println(payload.c_str());
        #endif
        //manager69.sendtoWait(&payload_c, payload.length(), RH69_TRANSCEIVER_ID);
        //delete [] &payload_c;
    }
#endif
