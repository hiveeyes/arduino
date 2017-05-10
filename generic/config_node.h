// Specific configuration settings for overriding all settings from config.h
// Will be in included at top *and* bottom of config.h

// Role
#define IS_NODE                 true

// Medium
#define HE_RADIO                true
#define HE_RH69                 true

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

