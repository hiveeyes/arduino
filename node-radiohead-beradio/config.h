//              "config.h"                       /**            * * * *               *** 
// all settings are made in this file, most      ***       interface settings         ***
//        relevant ones are above                ***             * * *                **/

#define DEBUG                     1              // turn on debug output and choose below
#define SERIAL_BAUD               115200         // serial baud rate
#define BLINKPERIOD               500            // LED blinking period in ms
#define SLEEP                     1              // set to 1 for sleeping
#define SLEEP_MINUTES             15             // sleeptime in about minutes
#define BERadio_profile           "h1"           // BERadio profile

#ifdef DEBUG                                     /**    fine grade debug settings     ***
                                                 ***              * *                 **/

    #define DEBUG_RADIO           0              // set to 1 for radio debug
    #define DEBUG_SPI_FLASH       0              // set to 1 for SPI-flash debug
    #define DEBUG_SENSORS         0              // set to 1 for sensor debug
    #define DEBUG_BERADIO         0              // set to 1 for BERadio degub

#endif                                           /**              * *                 **/


                                                 /**            * * * *               *** 
                                                 ***        hardware switches         ***
                                                 ***             * * *                **/

#define RFM69                     0              // set to 1 for lowpowerlab's RFM69 lib 
#define RH69                      1              // set to 1 for RadioHeads rf69 lib
#define RH95                      1              // set to 1 for RadioHeads rf95 lib
#define SPI_FLASH                 0              // set to 1 for using SPI-flash


                                                 /**            * * * *               *** 
                                                 ***    sensor pinning & settings     ***
                                                 ***             * * *                **/

#define ONE_WIRE_BUS              9              // DS18B20 data pin
#define TEMPERATURE_PRECISION     9              // DS18B20 value resolution

#define DHT_PIN1                  7              // DHT pin #1
#define DHT_PIN2                  6              // DHT pin #2
#define DHT_TYPE                  DHT22          // DHT type (dht22)

#define HX711_SCK                 A0             // SCK pin of hx711 
#define HX711_DT                  A1             // DT pin for hx711


                                                 /**            * * * *               *** 
                                                 ***    specific hardware defines     ***
                                                 ***             * * *                **/

#ifdef RH69                                      /**   RadioHead's RH69 radio lib     ***
                                                 ***              * *                 **/
    #define RH69_NODE_ID          99             //    
    #define RH69_GATEWAY_ID       1              // radio topology
    #define RH69_TRANSMITTER_ID   3              //

    #define RH69_IRQ              10             // radio pins
    #define RH69_SS               2              //

    #define RH69_FREQUENCY        868MHZ         // modem settings

#endif                                           /**              * *                 **/

#ifdef RH95                                      /**     RadioHead's RH95 radio lib   ***
                                                 ***              * *                 **/
    #define RH95_NODE_ID          99             // 
    #define RH95_GATEWAY_ID       1              // radio topology 
    #define RH95_TRANSMITTER_ID   3              //

    #define RH95_IRQ              10             // radio pins
    #define RH95_SS               2              //

    #define RH95_FREQUENCY       868MHZ          // modem settings

#endif                                           /**              * *                 **/

#ifdef RFM69                                     /**     RFM69 lib from lowpowerlab   ***
                                                 ***              * *                 **/

    #define RFM69_NODE_ID         2              //
    #define RFM69_NETWORK_ID      100            // radio topology
    #define RFM69_GATEWAY_ID      1              //

    #define RFM69_FREQUENCY       RF69_868MHZ    // modem settings
    #define IS_RFM69HW            0              // set to 1 for RFM69HW 
    #define RADIO_STATIC_POWER    0              // set to 1 for static power
    #define POWERLEVEL            5              // powerlevel range between 1-24(?)
    #define ENABLE_ATC            0              // 1 for automatic transmission control (ATC)
    #define TARGET_RSSI           -70            // RSSI 
    #define ENCRYPTKEY      "www.hiveeyes.org"   // same 16 characters/bytes everywhere
    #define ACK_TIME              30             // acknowledge timeout in ms 
    #define MAX_PAYLOAD_LENGTH    61             // Payload limitation RFM69 

#endif                                           /**              * *                 **/


#ifdef SPI_FLASH                                 /**           SPI-Flash              ***
                                                 ***              * *                 **/

    #define FLASH_MANUFACTURER_ID 0x0102         // MANUFACTURER_ID 
                                                 // 0x1F44 for adesto(ex atmel) 4mbit flash
                                                 // 0xEF30 for windbond 4mbit flash
                                                 // 0xEF40 for windbond 16/64mbit flash
                                                 // 0x0102 for Spansion S25FL032P 32-Mbit 

#endif                                           /**              * *                 **/


                                                 /**            * * * *               *** 
                                                 ***   automatic compiler directives  ***
                                                 ***             * * *                **/
#ifdef __AVR_ATmega1284P__
    #define LED                   15             // most Megas have LEDs on D15
    #define FLASH_SS              23             // and FLASH SS on D23
#else
    #define LED                   13             // Pro328mini has LEDs on D9
    #define FLASH_SS              8              // and FLASH SS on D8
#endif
