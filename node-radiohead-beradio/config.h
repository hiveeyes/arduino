// defines for everything //

// set to 1 if using lowpowerlab's RFM69 lib
#define RFM69 0

// set to 1 if using RadioHeads rf69 lib
#define RH69 1     

// set to 1 if using RadioHeads rf95 ŕadio lib
#define RH95 1      


// RH69 radiohead's radio lib
#ifdef RH69
    // radio topology
    #define RH69_NODE_ID         99
    #define RH69_GATEWAY_ID       1
    #define RH69_TRANSMITTER_ID   3

    // radio Pins
    #define RH69_IRQ              10
    #define RH69_SS               2

    // radio settings
    # define RH69_FREQUENCY       868MHZ
#endif


// RH95 radiohead's radio lib
#ifdef RH95
    // radio topology
    #define RH95_NODE_ID         99
    #define RH95_GATEWAY_ID       1
    #define RH95_TRANSMITTER_ID   3

    // radio Pins
    #define RH95_IRQ              10
    #define RH95_SS               2

    // radio settings
    # define RH95_FREQUENCY       868MHZ
#endif

// RFM69 lib from lowpowerlab
#ifdef RFM69
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
#endif

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


