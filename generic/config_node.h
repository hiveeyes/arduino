// Specific configuration settings for overriding settings in `config.h`.

// Role
#define IS_NODE                 true

// General
#define HE_SLEEP                true            // Enable sleeping
#define HE_FLASH                false           // Enable SPI-flash

// Sensors
#define HE_SCALE                true
#define HE_HUMIDITY             true
#define HE_TEMPERATURE          true
#define HX711_OFFSET            8361975
#define HX711_KNOWN_WEIGHT      21901

// Telemetry
#define HE_RADIO                true
#define HE_RH69                 true
#define HE_RFM69_OTA            false
#define HE_BERadio              true

#define HE_RFM69                false           // LowPowerLab RFM69 library
#define HE_RH69                 true            // RadioHead RH_RF69 driver
#define HE_RH95                 false           // RadioHead RH_RF95 driver
#define HE_RHTCP                false           // RadioHead RH_TCP driver

// Debug
#define HE_DEBUG                true            // turn on debug output and choose below
#define DEBUG_SENSORS           true            // set to 1 for sensor debug
