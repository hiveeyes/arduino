// vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
/*

   Generic software breadboard for nodes, transceivers and gateways.

   This firmware can satisfy different purposes:

   - A sensor node collects sensor data, encodes it with BERadio
     and sends it through a RFM69 radio module to a gateway node
     or a protocol transceiver node.

   - A protocol transceiver node receives radio signals on RFM69
     and emits them to RFM95 (LoRa). The messages are processed
     opaque, no decoding takes place here.

   - A gateway node receives RFM95 (LoRa) radio signals and emits
     the payloads to its UART interface connected to the gateway
     SoC (e.g. a RaspberryPi) running the BERadio forwarder.

   The BERadio forwarder will decode the data according
   to the BERadio specification and forward it to the
   MQTT bus serialized as JSON dictionary, all of which
   is beyond the scope of this MCU code.

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

   Project                      https://hiveeyes.org/
   Source code                  https://github.com/hiveeyes/

   Documentation
   System                       https://hiveeyes.org/docs/system/
   Arduino                      https://hiveeyes.org/docs/arduino/
   BERadio                      https://hiveeyes.org/docs/beradio/

-------------------------------------------------------------------------

*/


// --------------------
// Device configuration
// --------------------
#include "config.h"



// --------
// Platform
// --------

#ifdef ARDUINO

    // Arduino platform
    #include <Arduino.h>

    // Standard C++ for Arduino
    #include <new.cpp>
    #include <StandardCplusplus.h>
    #include <func_exception.cpp>
    #include <MemoryFree.h>

#endif

// Terrine: Application boilerplate for convenient MCU development
#include <Terrine.h>

//extern "C" Terrine terrine;
Terrine terrine;



// -----------------
// Platform features
// -----------------

#if HE_ARDUINO
    void Blink(byte, int);
#else
    #include <tools/simMain.cpp>
    #define OUTPUT 5
#endif

#if HE_SLEEP
    #include <LowPower.h>                   // https://github.com/LowPowerLab/LowPower
    void Sleep(int minutes);
#endif


// -------
// Sensors
// -------

#if HE_CONTAINERS
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

#if HE_BAROMETER
    #include <Adafruit_BMP085.h>
    #include <Wire.h>
    Adafruit_BMP085 bmp;
    void readBarometer();
#endif

// ----
// Serialisation
// ----


#if HE_BERadio
    #include <BERadio.h>
    #include <EmBencode.h>                  // https://github.com/jcw/embencode
    // Fix against "Undefined symbols for architecture x86_64"
    #ifdef __unix__
        #include <Terrine.cpp>
        #include <BERadio.cpp>
    #endif
    BERadioMessage *message = new BERadioMessage(HE_HIVE_ID, "h1", 61);
#endif



// ----
// Data
// ----

#if CONT_HUM
    FloatList *humL = new FloatList();
#endif
#if CONT_TEMP
    FloatList *tempL = new FloatList();
#endif
#if CONT_WGHT
    FloatList *wghtL = new FloatList();
#endif
#if CONT_BARO
    FloatList *altiL = new FloatList();
    FloatList *presL = new FloatList();
#endif
#if CONT_INFRA    
    //FloatList *bootsL = new FloatList();
    FloatList *loopsL = new FloatList();
    FloatList *rssiL = new FloatList();
#endif
#if CONT_INFRA    
    //FloatList *bootsL = new FloatList();
    FloatList *loopsL = new FloatList();
    FloatList *rssiL = new FloatList();
#endif


// -----
// Radio
// -----

// LowPowerLab RFM69 driver
#if HE_RFM69
    #include <RFM69.h>                      // https://github.com/LowPowerLab/RFM69
    #if ENABLE_ATC
        #include <RFM69_ATC.h>              // https://github.com/LowPowerLab/RFM69
        RFM69_ATC radio;
    #else
        RFM69 radio;
    #endif
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


// RadioHead subsystem
#if HE_RH95
    #include <RH_RF95.h>
    #include <RHReliableDatagram.h>
    #include <SPI.h>
    RH_RF95 rh95(RH95_SS, RH95_IRQ);
    uint8_t buf95[RH_RF95_MAX_MESSAGE_LEN];
    int rssi95;
    #if HE_RH95 && IS_TRANSCEIVER
        void transceive();
        RHReliableDatagram manager95(rh95, RH95_RESCEIVER_ID);
    #elif HE_RH95 && IS_GATEWAY
        void gatewayReceive95();
        RHReliableDatagram manager95(rh95, RH95_RESCEIVER_ID);
    #else
        RHReliableDatagram manager95(rh95, RH95_NODE_ID);
    #endif
#endif

#if HE_RH69
    #include <SPI.h>
    #include <RH_RF69.h>
    #include <RHReliableDatagram.h>
    RH_RF69 rh69(RH69_SS, RH69_IRQ);
    uint8_t buf69[RH69_MAX_MESSAGE_LEN];
    int rssi69;
    //int temp69;
    #if HE_RH69 && IS_TRANSCEIVER
        void transceive();
        RHReliableDatagram manager69(rh69, RH69_RESCEIVER_ID);
    #elif HE_RH69 && IS_GATEWAY
        void gatewayReceive69();
        RHReliableDatagram manager69(rh69, RH69_RESCEIVER_ID);
    #else
        RHReliableDatagram manager69(rh69, RH69_NODE_ID);
    #endif
#endif

#if HE_RHTCP
    #include <RH_TCP.h>
    #include <RHReliableDatagram.h>
    RH_TCP rh_tcp("localhost:4000");
    RHReliableDatagram managerTCP(rh_tcp, RHTCP_NODE_ID);
#endif


// -----------
// Peripherals
// -----------

#if HE_FLASH
    #include <SPI.h>
    #include <SPIFlash.h>                   // https://github.com/LowPowerLab/SPIFlash
    SPIFlash flash(FLASH_SS, FLASH_MANUFACTURER_ID);
#endif


// ----------------
// Global variables
// ----------------
// TODO: Review regularly and try to get rid of them.

// common
char input = 0;
// radio
// long lastPeriod = -1;                // use in case of a time controled radio transmit routine
int TRANSMITPERIOD = 300;               //transmit a packet to gateway so often (in ms)
#if HE_RH69 && HE_RH95
    bool is_online95 = false;
    bool is_online69 = false;
#else
    bool is_online = false;
#endif
#if DEBUG_SEND_INFRA
    uint8_t boots = 0;
    uint8_t loops = 0;
#endif
std::string separator = std::string(42, '=');



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//                  setup function#
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void setup() {


    // -------------
    // Bootstrapping
    // -------------
    SERIAL_PORT_HARDWARE.begin(SERIAL_BAUD);          // setup serial
    #if HE_DEBUG
        terrine.log(separator.c_str());
        terrine.log("SETUP");
        terrine.log(separator.c_str());
    #endif
    #if HE_ARDUINO
        pinMode(LED, OUTPUT);               // setup onboard LED
    #endif
    #if DEBUG_MEMORY
        terrine.logmem();
    #endif

    //dht.begin();

    #if HE_DEBUG
        terrine.log("booting: ... ", false);
        #if IS_NODE
            terrine.log("Hiveeyes node ");
        #elif IS_TRANSCEIVER
            terrine.log("Hiveeyes transceiver ");
        #else
            terrine.log("Hiveeyes gateway ");
        #endif
    #endif


    // ---------
    // Telemetry
    // ---------

    #if BERadio
        message.set_mtu_size(61);
    #endif


    // -----
    // Radio
    // -----

    #if HE_DEBUG
        terrine.log("# Radio");
    #endif

    // Inform user about radio device configuration
    #if DEBUG_RADIO
        terrine.log("RFM69: ", false);
        terrine.log(HE_RFM69);
        terrine.log("RH69:  ", false);
        terrine.log(HE_RH69);
        terrine.log("RH95:  ", false);
        terrine.log(HE_RH95);
        terrine.log("RHTCP: ", false);
        terrine.log(HE_RHTCP);
    #endif

    // LowPowerLab RFM69 driver
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
                SERIAL_PORT_HARDWARE.println("RFM69 ATC enabled");
            #endif
        #endif
        radio.sleep();
        #if DEBUG_RADIO
            sprintf(buff, "Transmitting at %d Mhz...", RFM69_FREQUENCY==RF69_433MHZ ? 433 : RFM69_FREQUENCY==RF69_868MHZ ? 868 : 915);
            SERIAL_PORT_HARDWARE.println(buff);
        #endif
    #endif

    // RadioHead subsystem
    bool freq_ack = false;
    #if HE_RH95 && HE_RH69
        bool freq_ack95 = false;
        digitalWrite(RH69_SS, HIGH);
        is_online95 = manager95.init();
        freq_ack95  = rh95.setFrequency(RH95_FREQUENCY);
        bool freq_ack69 = false;
        digitalWrite(RH95_SS, HIGH);
        is_online69 = manager69.init();
        freq_ack69  = rh69.setFrequency(RH69_FREQUENCY);
    #elif HE_RH69
        is_online = manager69.init();
        freq_ack  = rh69.setFrequency(RH69_FREQUENCY);
    #elif HE_RH95
        is_online = manager95.init();
        freq_ack  = rh95.setFrequency(RH95_FREQUENCY);
    #elif HE_RHTCP
        is_online = managerTCP.init();
        freq_ack  = -1;
    #endif

    #if DEBUG_RADIO
        #if HE_RH95 && HE_RH69 == 0
            terrine.log("is_online: ", false);
            terrine.log(is_online);
            terrine.log("freq_ack: ", false);
            terrine.log(freq_ack);
        #elif HE_RH95 && HE_RH69
            terrine.log("is_online95: ", false);
            terrine.log(is_online95);
            terrine.log("freq_ack95: ", false);
            terrine.log(freq_ack95);
            terrine.log("is_online95: ", false);
            terrine.log(is_online95);
            terrine.log("freq_ack69: ", false);
            terrine.log(freq_ack69);
        #endif
    #endif


    // -------
    // Sensors
    // -------
    #if DEBUG_SENSORS
        terrine.log("# Sensors");
    #endif

    #if HE_TEMPERATURE
        #if DEBUG_SENSORS
            terrine.log("Temp OW");
        #endif
        sensors.begin();
    #endif

    #if HE_HUMIDITY
        #if DEBUG_SENSORS
            terrine.log("Hum");
        #endif
        dht1.setup(DHT_PIN1);
        dht2.setup(DHT_PIN2);
    #endif
    #if HE_SCALE
        #if DEBUG_SENSORS
            terrine.log("Scale");
        #endif
        scale.set_offset(HX711_OFFSET);          // the offset of the scale, is raw output without any weight, get this first and then do set.scale
        scale.set_scale(HX711_KNOWN_WEIGHT);           // this is the difference between the raw data of a known weight and an emprty scale
    #endif

    #if HE_BAROMETER
        #if DEBUG_SENSORS
            terrine.log("Barometer");
        #endif
        bmp.begin();
// Medium
    #endif

    // ---------
    // SPI Flash
    // ---------
    #if HE_FLASH
        #if DEBUG_SPI_FLASH
            SERIAL_PORT_HARDWARE.println("SPI Flash setup");
        #endif
        if (flash.initialize()){
            #if DEBUG_SPI_FLASH
                SERIAL_PORT_HARDWARE.print("SPI Flash Init OK ... UniqueID (MAC): ");
            #endif
            flash.readUniqueId();
            #if DEBUG_SPI_FLASH
                for (byte i=0;i<8;i++){
                    SERIAL_PORT_HARDWARE.print(flash.UNIQUEID[i], HEX);
                    SERIAL_PORT_HARDWARE.print(' ');
                    }
                SERIAL_PORT_HARDWARE.println();
            #endif
            }
        else
        #if DEBUG_SPI_FLASH
        SERIAL_PORT_HARDWARE.println("SPI Flash Init FAIL! (is chip present?)");
        #endif
    #endif
    #if DEBUG_SEND_INFRA
        boots++;
    #endif


    #if HE_DEBUG
        #if DEBUG_SEND_INFRA
            terrine.log("boots ", false);
            terrine.log(boots);
        #endif
        terrine.log("# ready.");
    #endif

}
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//                  loop function
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int loop_counter = 0;

void loop() {
    #if DEBUG_FRAME
        terrine.log(separator.c_str());
        terrine.log("MAIN");
        terrine.log(separator.c_str());
    #endif
    #if DEBUG_MEMORY
        terrine.logmem();
    #endif

    // get sensor data

    #if CONT_TEMP
        tempL->clear();
    #endif
    #if CONT_HUM
        tempL->clear();
        humL->clear();
    #endif
    #if CONT_WGHT
        wghtL->clear();
    #endif
    #if CONT_BARO
        altiL->clear();
        presL->clear();
    #endif

    #if HE_TEMPERATURE
        readTemperature();
    #endif
    #if HE_HUMIDITY
        readHumidity();
    #endif
    #if HE_SCALE
        readScale();
    #endif
    #if HE_BAROMETER
        readBarometer();
    #endif
    #if HE_RH69 && IS_TRANSCEIVER
        transceive();
    #endif
    #if HE_RH95 && IS_GATEWAY
        gatewayReceive();
    #endif

    #if DEBUG_SEND_INFRA
        loops++;
        #if CONT_INFRA
            //bootsL->clear();
            loopsL->clear();
            rssiL->clear();
            //bootsL->push_back(boots);
            loopsL->push_back(loops);
            rssiL->push_back(rssi69);
            //infraL->push_back(temp69);
        #endif
    #endif
// serialize and transmit data
    #if HE_BERadio

        #if DEBUG_MEMORY
            terrine.logmem();
        #endif

        #if DEBUG_BERadio
             SERIAL_PORT_HARDWARE.println("cSb");
             delay(200);
        #endif

        #if HE_DEMODATA
            tempL->push_back(42.42);
            tempL->push_back(84.84);
            tempL->push_back(21.21);
            humL->push_back(55);
            humL->push_back(77.1);
            wghtL->push_back(63);
        #endif

        #if HE_TEMPERATURE
            message->add("t", *tempL);
        #endif
        #if HE_HUMIDITY
            message->add("h", *humL);
        #endif
        #if HE_SCALE
            message->add("w", *wghtL);
        #endif
        #if HE_BAROMETER
            message->add("p", *presL);
            message->add("a", *altiL);
        #endif
        #if DEBUG_MEMORY
            terrine.logmem();
        #endif
        #if DEBUG_SEND_INFRA
            //message->add("b", *bootsL);
            message->add("l", *loopsL);
            message->add("r", *rssiL);
        #endif

        #if DEBUG_MEMORY
            terrine.logmem();
        #endif

        #if DEBUG_BERadio
             SERIAL_PORT_HARDWARE.println("cSe");
             delay(200);
        #endif

        // Encode, fragment and transmit message
        message->transmit();

    #endif

    #if DEBUG_LED
        #if IS_NODE 
            Blink(LED, LED_TIME);
        #endif 
    #endif 

    #if HE_RH69 && IS_TRANSCEIVER
        transceive();
    #endif
    #if HE_RH69 && IS_GATEWAY
        //SERIAL_PORT_HARDWARE.println("YYY");
        gatewayReceive69();
    #endif
    #if HE_RH95 && IS_GATEWAY
        gatewayReceive95();
    #endif

    #if HE_SLEEP
        if (loop_counter <= BOOTSTRAP_LOOP_COUNT) {
            delay(1000);
            loop_counter++;
        } else {
            delay(100);
            Sleep(SLEEP_MINUTES);
        }
    #endif
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//                  user functions
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// get OneWire data //

#if HE_TEMPERATURE
    void readTemperature() {
        #if DEBUG_SENSORS
            SERIAL_PORT_HARDWARE.println("rT");
        #endif
        sensors.requestTemperatures();
        tempL->push_back(sensors.getTempCByIndex(0));
        tempL->push_back(sensors.getTempCByIndex(1));
        tempL->push_back(sensors.getTempCByIndex(2));
        tempL->push_back(sensors.getTempCByIndex(3));
        #if DEBUG_SENSORS
            SERIAL_PORT_HARDWARE.println("dT");
            for (double value: *tempL){
                SERIAL_PORT_HARDWARE.println(value);
            }
        #endif
    }
#endif

#if HE_HUMIDITY
    void readHumidity(){
        #if DEBUG_SENSORS
            SERIAL_PORT_HARDWARE.println("rHT");
        #endif
        dht1.getHumidity();          // the first reading of DHT is the last one.
        dht1.getTemperature();          // the first reading of DHT is the last one.
        float humbuf0;
        float tempbuf0;
        humbuf0 = dht1.getHumidity();
        tempbuf0 = dht1.getTemperature();

        #if DEBUG_SENSORS
            SERIAL_PORT_HARDWARE.print("...");
        #endif
        delay(2000);

        for(int i=0; i<=3; i++){
            if (!isnan(humbuf0) || !isnan(tempbuf0)){
                break;
            }
            #if DEBUG_SENSORS
                SERIAL_PORT_HARDWARE.print(".");
            #endif
            delay(2000);
        }
        SERIAL_PORT_HARDWARE.println();
        if (!isnan(humbuf0) || !isnan(tempbuf0)){
            humL->push_back(humbuf0);
            tempL->push_back(tempbuf0);
        }
        else{
            humL->push_back(BAD_VALUE);
            tempL->push_back(BAD_VALUE);
            #if DEBUG_SENSORS
                SERIAL_PORT_HARDWARE.println("rHTna");
            #endif
        }
        #if DEBUG_SENSORS
            SERIAL_PORT_HARDWARE.println("dH");
            for (double value: *humL){
                SERIAL_PORT_HARDWARE.println(value);
            }
            SERIAL_PORT_HARDWARE.println("dT");
            for (double value: *tempL){
                SERIAL_PORT_HARDWARE.println(value);
            }
        #endif
    }
#endif
// get weight data //
#if HE_SCALE
    void readScale(){
        #if DEBUG_SENSORS
            SERIAL_PORT_HARDWARE.println("rS");
        #endif
        scale.power_up();
        wghtL->push_back(scale.read_average(3));              // get the raw data of the scale
        wghtL->push_back(scale.get_units(3));                 // get the scaled data
        scale.power_down();
        #if DEBUG_SENSORS
            SERIAL_PORT_HARDWARE.println("dS");
            for (double value: *wghtL){
                SERIAL_PORT_HARDWARE.println(value);
            }
        #endif
    }
#endif

// get barometer data //

#if HE_BAROMETER
    void readBarometer(){
        #if DEBUG_SENSORS
            SERIAL_PORT_HARDWARE.println("rB");
        #endif
        presL->push_back(bmp.readPressure());
        altiL->push_back(bmp.readAltitude(BAROMETER_MSLP));
        #if DEBUG_SENSORS
            SERIAL_PORT_HARDWARE.println("dP");
            for (double value: *presL){
                SERIAL_PORT_HARDWARE.println(value);
            }
            SERIAL_PORT_HARDWARE.println("dA");
            for (double value: *altiL){
                SERIAL_PORT_HARDWARE.println(value);
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
            SERIAL_PORT_HARDWARE.println(" sending done");
            //#endif
        }
        //#ifdef DEBUG_RADIO
        else SERIAL_PORT_HARDWARE.print(" not send");
        //#endif
        radio.sleep();
        rACK = 0;
    }
#endif
//   HE_RFM69 receiving

void receivePackages(){
   //check for any received packets
   #if HE_RFM69
   if (radio.receiveDone()){
       #ifdef DEBUG_RADIO
       SERIAL_PORT_HARDWARE.print('[');SERIAL_PORT_HARDWARE.print(radio.SENDERID, DEC);SERIAL_PORT_HARDWARE.print("] ");
       for (byte i = 0; i < radio.DATALEN; i++)
           SERIAL_PORT_HARDWARE.print((char)radio.DATA[i]);
           SERIAL_PORT_HARDWARE.print("   [RX_RSSI:");SERIAL_PORT_HARDWARE.print(radio.RSSI);SERIAL_PORT_HARDWARE.print("]");
       #endif
       if (radio.ACKRequested()){
           radio.sendACK();
           #ifdef DEBUG_RADIO
           SERIAL_PORT_HARDWARE.print(" - ACK sent");
           #endif
           }
       #if HE_ARDUINO
       Blink(LED,3);
       #endif
       #ifdef DEBUG_RADIO
       SERIAL_PORT_HARDWARE.println();
       #endif
       }
   radio.sleep();
   #endif
}

#if HE_ARDUINO
    //   LED Blink
    void Blink(byte PIN, int DELAY_MS){
       pinMode(PIN, OUTPUT);
       digitalWrite(PIN,HIGH);
       delay(DELAY_MS);
       digitalWrite(PIN,LOW);
    }
#endif


//   Sleeping

#if HE_SLEEP
    void Sleep(int minutes ){
        int var = 0;
        float cycles = 0;
        cycles = minutes * 7.5;
        #if HE_DEBUG
            terrine.log("sleeping ", false);
        #endif
        while(var < cycles){
            var++;
            // put the processor to sleep for 8 seconds
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
            #if HE_DEBUG
                terrine.log("... ", false);
            #endif
        }
        #if HE_DEBUG
            terrine.log("awake", true);
        #endif
    }
#endif


/*
#if HE_RH69 && IS_NODE
    void nodeSend(){
        // Send a message to manager_server
        #if RADIO_DEBUG
            if (manager69.sendtoWait(buf69, sizeof(buf69), HE_RH69_GATEWAY_ID)){
                  // Now wait for a reply from the server
                  uint8_t len = sizeof(buf69);
                  uint8_t from;
                  if (manager69.recvfromAckTimeout(buf69, &len, 2000, &from)){
                        SERIAL_PORT_HARDWARE.print("got reply from : 0x");
                        SERIAL_PORT_HARDWARE.print(from, HEX);
                        SERIAL_PORT_HARDWARE.print(": ");
                        SERIAL_PORT_HARDWARE.println((char*)buf69);
                  }
                  else{
                        SERIAL_PORT_HARDWARE.println("No reply, is rf95_reliable_datagram_server running?");
                  }
            }
            else
            SERIAL_PORT_HARDWARE.println("sendtoWait failed");
        #else
            manager69.sendtoWait(buf69, sizeof(buf69), HE_RH69_GATEWAY_ID);
        #endif
    }
#endif
*/
#if HE_RH95 && HE_RH69 && IS_TRANSCEIVER
    void transceive(){
        digitalWrite(RH95_SS, HIGH);
        if (manager69.available()){
            // Wait for a message addressed to us from the client
            uint8_t len69 = sizeof(buf69);
            uint8_t len95 = sizeof(buf95);
            uint8_t from69;
            uint8_t from95;
            // Radio transmission
            if (is_online69 && is_online95) {
                #if HE_RH69 && HE_RH95
                    bool success69 = false;
                    //bool success69 =  manager69.recvfromAckTimeout(buf69, &len69, RH_ACK_TIMEOUT, &from);
                    success69 =  manager69.recvfromAck(buf69, &len69, &from69);
                    #if DEBUG_RADIO
                        #if DEBUG_SEND_INFRA   
                            if (loops >= 1){rssi69 = rh69.lastRssi();}
                            terrine.log("RSSI69: ", false);
                            terrine.log(rssi69);
                        #endif
                        terrine.log("SUCCESS69: ", false);
                        terrine.log(success69);
                        //terrine.log("recv_msg: ", false);
                        //terrine.log((char)buf69);
                    #endif
                    //delay(1000);
                    digitalWrite(RH69_SS, HIGH);
                    //bool success95 = manager95.sendtoWait(buf69, len69, RH95_GATEWAY_ID);
                    bool success95 = false;
                    success95 = manager95.sendtoWait(buf69, len69, RH95_GATEWAY_ID);
                    #if DEBUG_SEND_INFRA   
                        if (loops >= 1){rssi95 = rh95.lastRssi();}
                        terrine.log("RSSI95: ", false);
                        terrine.log(rssi95);
                    #endif
                    #if DEBUG_RADIO
                        terrine.log("SUCCESS95: ", false);
                        terrine.log(success95);
                        terrine.log((char*)buf69);
                    #elif HE_DEBUG
                        terrine.log((char*)buf69);
                    #endif
                    memset(&buf69[0], 0, len69);
                    bool sleep95 = rh95.sleep();
                    #if DEBUG_LED
                        Blink(LED, 100);
                    #endif
                    #if DEBUG_RADIO
                        terrine.log("sleeps95: ", false);
                        terrine.log(sleep95);
                    #endif
                    //memset(&buf95[0], 0, sizeof(len));
                #endif
                }
            else {
                terrine.log("WARN: Offline");
            }
        }
    }
#endif

#if HE_RH69 && IS_GATEWAY
    void gatewayReceive69(){
        uint8_t len = sizeof(buf69);
        uint8_t from;
        if (manager69.available()){
        //if (is_online){
            // Wait for a message addressed to us from the client
            //bool success = manager69.recvfromAckTimeout(buf69, &len, RH_ACK_TIMEOUT, &from);
            bool success = manager69.recvfromAck(buf69, &len, &from);
            //bool success = manager69.recvfromAck(buf69, &len);
                  #if DEBUG_RADIO
                      terrine.log("SUCCESS: ", false);
                      terrine.log(success);
                      terrine.log("@", false);
                      terrine.log(from);
                  #endif
                  if (success){SERIAL_PORT_HARDWARE.println((char*)buf69); SERIAL_PORT_HARDWARE.println();}
        }
        memset(&buf69[0], 0, len);
    }
#endif

#if HE_RH95 && IS_GATEWAY
    void gatewayReceive95(){
        uint8_t len = sizeof(buf95);
        uint8_t from;
        if (manager95.available()){
        //if (is_online){
            // Wait for a message addressed to us from the client
            //bool success = manager95.recvfromAckTimeout(buf95, &len, RH_ACK_TIMEOUT, &from);
            bool success = manager95.recvfromAck(buf95, &len, &from);
            //bool success = manager95.recvfromAck(buf95, &len);
                  #if DEBUG_RADIO
                      terrine.log("SUCCESS: ", false);
                      terrine.log(success);
                      terrine.log("@", false);
                      terrine.log(from);
                  #endif
                  if (success){SERIAL_PORT_HARDWARE.println((char*)buf95); SERIAL_PORT_HARDWARE.println();}
        }
        memset(&buf95[0], 0, len);
        #if DEBUG_SEND_INFRA   
            if (loops >= 1){rssi95 = rh95.lastRssi();}
            terrine.log("RSSI95: ", false);
            terrine.log(rssi95);
        #endif
        #if DEBUG_LED
            Blink(LED, 100);
        #else
            delay(100);
        #endif
    }
#endif

#if HE_BERadio
    void BERadioMessage::send(char* buffer, int length) {

        #if DEBUG_BERadio
            dprint("msg: ", false);
            dprint(buffer);
            //dprint(payload.c_str());
        #endif


        // C++ std::string
        //uint8_t *buf69 = new uint8_t[payload.length() + 1];
        //strcpy(buf69, payload.c_str());

        // C buffer
        // https://stackoverflow.com/questions/25360893/convert-char-to-uint8-t/25360996#25360996
        uint8_t *rh_buffer = new uint8_t[length + 1];
        memcpy(rh_buffer, (const char*)buffer, length);
        // Radio transmission
        if (is_online) {
            #if HE_RH69
                bool success = manager69.sendtoWait(rh_buffer, length, RH69_RESCEIVER_ID);
                terrine.log("SUCCESS: ", false);
                terrine.log(success);
                delay(BERadio_DELAY);
            #elif HE_RHTCP
                managerTCP.sendtoWait(rh_buffer, length, RHTCP_GATEWAY_ID);
                terrine.log("SUCCESS: ", false);
                terrine.log(success);
            #endif
            #if DEBUG_SEND_INFRA   
                if (loops >= 1){rssi69 = rh69.lastRssi();}
            #endif
            //temp69 = rh69.temperatureRead();
            bool sleep69 = rh69.sleep();
            #if DEBUG_RADIO
                terrine.log("sleeps69: ", false);
                terrine.log(sleep69);
                terrine.log("RSSI69: ", false);
                terrine.log(rssi69);
            #endif
        }
        else {
            terrine.log("WARN: Offline");
        }

        // Clean up
        delete rh_buffer;
    }
#endif

#if HE_BERadio
void BERadioMessage::dprint(const char *message, bool newline) {
    terrine.log(message, newline);
}

void BERadioMessage::dprint(int value) {
    terrine.log(value);
}
#endif

