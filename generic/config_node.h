// Specific configuration settings for overriding all settings from config.h
// Will be in included at top *and* bottom of config.h

// Role
#define IS_NODE                 true

// Medium
#define HE_RADIO                true
#define HE_RH69                 true
#define RH69_RESCEIVER_ID       3              // ID of next RESCEIVER (gateway | transceiver)

// Debug
#define HE_DEBUG                  true               // turn on debug output and choose below
#define DEBUG_SENSORS             true              // set to 1 for sensor debug
#define BOOTSTRAP_LOOP_COUNT      15             // How often to loop fast when booting

// Details
#define HE_SLEEP                  true              // enable sleeping
#define HE_SCALE                  true
#define HE_HUMIDITY               true
#define HE_TEMPERATURE            true
#define HE_RFM69_OTA              false
#define HE_BERadio                true

#define HE_RFM69                  false              // LowPowerLab RFM69 library
#define HE_RH69                   true              // RadioHead RH_RF69 driver
#define HE_RH95                   false              // RadioHead RH_RF95 driver
#define HE_RHTCP                  false              // RadioHead RH_TCP driver
#define HE_FLASH                  false              // Enable SPI-flash

// Scale


#define HX711_OFFSET          -25018
#define HX711_KNOWN_WEIGHT     21623