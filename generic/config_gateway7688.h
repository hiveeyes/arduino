// Specific configuration settings for overriding all settings from config.h
// Will be in included at top *and* bottom of config.h

// Role
// Details

#define HE_DEBUG                  true               // turn on debug output and choose below
#define SERIAL_BAUD               115200         // serial baud rate
#define BLINKPERIOD               500            // LED blinking period in ms
#define BOOTSTRAP_LOOP_COUNT      15             // How often to loop fast when booting
#define HE_SLEEP                  false              // enable sleeping
#define HE_SCALE                  false
#define HE_HUMIDITY               false
#define HE_TEMPERATURE            false
#define HE_DEMODATA               false
#define HE_RFM69_OTA              false
#define HE_BERadio                false
#define HE_RADIO                  true
//#define HE_CONTAINERS             false


#define IS_NODE                   false
#define IS_TRANSCEIVER            false
#define IS_GATEWAY                true


#define HE_RFM69                  false              // LowPowerLab RFM69 library
#define HE_RH69                   false              // RadioHead RH_RF69 driver
#define HE_RH95                   true              // RadioHead RH_RF95 driver
#define HE_RHTCP                  false              // RadioHead RH_TCP driver
#define HE_FLASH                  false              // Enable SPI-flash


#if HE_DEBUG                                  /**    fine grade debug settings     ***
                                                 ***              * *                 **/
    #define DEBUG_FRAME           false
    #define DEBUG_RADIO           false              // set to 1 for radio debug
    #define DEBUG_SPI_FLASH       false              // set to 1 for SPI-flash debug
    #define DEBUG_SENSORS         false              // set to 1 for sensor debug
    #define DEBUG_BERadio         false              // set to 1 for  HE_BERadio degub
    #define DEBUG_MEMORY          false

#endif                                           /**              * *                 **/

#define RH95_IRQ              3             // radio pins
#define RH95_SS               17              //
