/**
 *
 * Generic software breadboard for nodes, relays, and gateways.
 *
 * Copyright (C) 2014-2017  Richard Pobering <richard@hiveeyes.org>
 * Copyright (C) 2014-2023  Andreas Motl <andreas@hiveeyes.org>
 *
 * Configuration file: "config.h".
 *
 * All configuration adjustments regarding the behavior and operation mode
 * (node vs. relay vs. gateway), should be conducted within this file.
 *
 * Please create an issue if you think something is not working properly,
 * or if some feature is missing.
 *
 * -- https://github.com/hiveeyes/arduino/issues
 *
**/


/**
 * Feature flags.
 * Please define them by using the `build_flags` option within `platformio.ini`.
 */

/*
#define HE_DEBUG                  false              // turn on debug output and choose below
#define HE_SLEEP                  false              // enable sleeping
#define HE_SCALE                  false
#define HE_HUMIDITY               false
#define HE_TEMPERATURE            false
#define HE_DEMODATA               false
#define HE_RFM69_OTA              false
#define HE_BERadio                false
#define HE_RADIO                  false

#define IS_NODE                   false
#define IS_RELAY                  false
#define IS_GATEWAY                false

#define HE_RFM69                  false              // LowPowerLab RFM69 library
#define HE_RH69                   false              // RadioHead RH_RF69 driver
#define HE_RH95                   false              // RadioHead RH_RF95 driver
#define HE_RHTCP                  false              // RadioHead RH_TCP driver
#define HE_FLASH                  false              // Enable SPI-flash
*/



/**
 * Use a custom configuration settings header file.
 * To include a custom "config_xxx.h" (e.g. config_node.h vs. config_gateway.h) file.
 *
 * Synopsis:
 *
 *     PLATFORMIO_BUILD_FLAGS="-D CUSTOM_CONFIG=config_foo.h" pio run --environment=relay
 *
 */

// Compute path to header file.
// https://stackoverflow.com/questions/5873722/c-macro-dynamic-include
#ifdef CUSTOM_CONFIG
#define PASTER(str)   #str
    #define EVALUATOR(x)        PASTER(x)
    #define CUSTOM_CONFIG_FILE  EVALUATOR(CUSTOM_CONFIG)
#endif

// Include header file.
#ifdef CUSTOM_CONFIG_FILE
#include CUSTOM_CONFIG_FILE
#endif



/**
 * Configuration defaults.
 */

#if !defined(SERIAL_BAUD)
#define SERIAL_BAUD               115200         // serial baud rate
#endif

#if !defined(BLINKPERIOD)
#define BLINKPERIOD               500            // LED blinking period in ms
#endif

#if !defined(BOOTSTRAP_LOOP_COUNT)
#define BOOTSTRAP_LOOP_COUNT      15             // How often to loop fast when booting
#endif

#if HE_SLEEP
    #if !defined(SLEEP_MINUTES)
    #define SLEEP_MINUTES         15             // sleeptime in about minutes
    #endif
#endif


/**
 * Debug settings.
 */

#if HE_DEBUG

    #define DEBUG_FRAME           false
    #define DEBUG_RADIO           false              // set to 1 for radio debug
    #define DEBUG_SPI_FLASH       false              // set to 1 for SPI-flash debug
    #define DEBUG_SENSORS         false              // set to 1 for sensor debug
    #define DEBUG_BERadio         false              // set to 1 for  HE_BERadio degub
    #define DEBUG_MEMORY          false

#endif


/**
 * BERadio configuration.
 * https://github.com/hiveeyes/beradio
 */

#if HE_BERadio
    #define  BERadio_profile           "h1"           //  HE_BERadio profile
    #define  HE_HIVE_ID                2
    #define  BAD_VALUE             273.15
    #define  BERadio_DELAY         500
#endif


/**
 * Peripherals enablement.
 * Enable / disable your peripherals here.
 */

#ifndef HE_ARDUINO
    #define HE_ARDUINO              true
#endif
#ifdef __unix__
    #define HE_ARDUINO              false
    #define DEBUG_MEMORY            false
    #define HE_RH69                 false
    #define HE_RH95                 false
    #define HE_RHTCP                true
    #define HE_SCALE                false
    #define HE_HUMIDITY             false
    #define HE_TEMPERATURE          false
    #define HE_DEMODATA             true
#endif


/*
#if IS_RELAY
    #define IS_NODE                false
    #define IS_GATEWAY             false
#elif IS_GATEWAY
    #define IS_NODE                false
    #define IS_RELAY               false
#elif IS_NODE
    #define IS_RELAY               false
    #define IS_GATEWAY             false
#endif
*/


/**
 * Sensor configuration.
 * Configure settings for your sensor hardware here.
 */

#ifdef HE_TEMPERATURE
    #define DS18B20_BUS           9              // DS18B20 data pin
    #define TEMP_PRECISION        9              // DS18B20 value resolution
#endif

#ifdef  HE_HUMIDITY
    #define DHT_AMOUNT                1
    #define DHT_PIN1                  7              // DHT pin #1
    #define DHT_PIN2                  6              // DHT pin #2
    #define DHT_TYPE                  DHT22          // DHT type (dht22)
#endif

#ifdef HE_SCALE
    #define HX711_SCK                 A0             // SCK pin of hx711
    #define HX711_DT                  A1             // DT pin for hx711
    #define HX711_OFFSET          8361975

    #define HX711_KNOWN_WEIGHT     21901.f
#endif


/**
 * Telemetry hardware configuration.
 */

#ifdef HE_RADIO
    #define RH_ACK_TIMEOUT           200

    // RadioHead's HE_RH69radio lib
    #if HE_RH69
        #define RH69_NODE_ID          99             // Radio topology
        #define RH69_RECEIVER_ID       1             // ID of next hop (relay | gateway)
        //#define RH69_RELAY_ID          3

        #define RH69_IRQ              2             // radio pins
        #define RH69_SS               10              //

        #define RH69_FREQUENCY        868.0         // modem settings
        #define RH69_MAX_MESSAGE_LEN 61

    #endif

    // RadioHead's HE_RH95radio lib
    #if HE_RH95

        #define RH95_NODE_ID          99             // Radio topology
        #define RH95_RECEIVER_ID       1             // ID of next hop (relay | gateway)
        //#define RH95_RELAY_ID          3
        #if IS_RELAY
            #if !defined(RH95_IRQ)
                #define RH95_IRQ              3             // radio pins
            #endif
            #if !defined(RH95_SS)
                #define RH95_SS               5              //
            #endif
        #elif IS_GATEWAY
            #if !defined(RH95_IRQ)
                #define RH95_IRQ              2             // radio pins
            #endif
            #if !defined(RH95_SS)
                #define RH95_SS               10              //
            #endif
        #endif


        #define RH95_FREQUENCY        868.0         // modem settings
        #define RH95_MAX_MESSAGE_LEN 255

    #endif

    // RadioHead HE_TCP driver
    #if HE_RHTCP
        #define RHTCP_NODE_ID         99
        #define RHTCP_GATEWAY_ID      1
    #endif

    // HE_RFM69 lib from lowpowerlab
    #if HE_RFM69
        //#include <HE_RFM69.h>

        #define RFM69_NODE_ID         2              //
        #define RFM69_NETWORK_ID      100            // radio topology
        #define RFM69_GATEWAY_ID      1              //

        #define RFM69_FREQUENCY       RF69_868MHZ         // modem settings
        #define RFM69_IS_HW            0              // set to 1 for HE_RFM69HW
        #define RADIO_STATIC_POWER    0              // set to 1 for static power
        #define RFM69_POWERLEVEL            5              // powerlevel range between 1-24(?)
        //#define ENABLE_ATC                           // 1 for automatic transmission control (ATC)
        #ifdef  RFM69_ENABLE_ATC
            #define RFM69_TARGET_RSSI       -70            // RSSI
        #endif
        #define RFM69_ENCRYPTKEY      "www.hiveeyes.org"   // same 16 characters/bytes everywhere
        #define RFM69_ACK_TIME              30             // acknowledge timeout in ms
        #define RFM69_MAX_MESSAGE_LENGTH    61             // Payload limitation HE_RFM69
    #endif

#endif


/**
 * SPI-Flash configuration.
 */

#if HE_FLASH
    #define FLASH_MANUFACTURER_ID 0x0102         // MANUFACTURER_ID
                                                 // 0x1F44 for adesto(ex atmel) 4mbit flash
                                                 // 0xEF30 for windbond 4mbit flash
                                                 // 0xEF40 for windbond 16/64mbit flash
                                                 // 0x0102 for Spansion S25FL032P 32-Mbit
    #if HE_RFM69_OTA
        #include <WirelessHEX69.h>
    #endif
#endif


/**
 * Automatic compiler directives.
 */

#ifdef __AVR_ATmega1284P__
    #define LED                   15             // most Megas have LEDs on D15
    #define FLASH_SS              23             // and FLASH SS on D23
#else
    #define LED                   13             // Pro328mini has LEDs on D9
    #define FLASH_SS              8              // and FLASH SS on D8
#endif
