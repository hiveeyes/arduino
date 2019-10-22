### DISCLAIMER
### This is an example Makefile and it MUST be configured to suit your needs.
### For detailled explanations about all the avalaible options,
### please refer to https://github.com/sudar/Arduino-Makefile/blob/master/arduino-mk-vars.md

### PROJECT_DIR
### This is the path to where you have created/cloned your project
PROJECT_DIR       = ..
# PROJECT_DIR       = $(HOME)/dev/arduino/bare-arduino-project

### AVR_TOOLS_DIR
### Path to the AVR tools directory such as avr-gcc, avr-g++, etc.
AVR_TOOLS_DIR     = /usr/local
#AVR_TOOLS_DIR     = /Applications/Arduino.app/Contents/Resources/Java/hardware/tools/avr

### AVR_GCC_VERSION
### Check if the version is equal or higher than 4.9
AVR_GCC_VERSION  := $(shell expr `$(AVR_TOOLS_DIR)/bin/avr-gcc -dumpversion | cut -f1` \>= 4.9)

### ARDMK_DIR
### Path to the Arduino-Makefile directory.
ARDMK_DIR         = $(PROJECT_DIR)/tools/Arduino-Makefile

### ARDUINO_DIR
### Path to the Arduino application and ressources directory.
### For Arduino 1.0.x
# ARDUINO_DIR       = /Applications/Arduino.app/Contents/Resources/Java
### For Arduino 1.6.x and 1.8.x
ARDUINO_DIR       = /Applications/Arduino.app/Contents/Java

### USER_LIB_PATH
### Path to where the your project's libraries are stored.
USER_LIB_PATH     :=  $(realpath $(PROJECT_DIR)/libraries)

### BOARD_TAG & BOARD_SUB
### For Arduino 1.0.x
### Only BOARD_TAG is needed. It must be set to the board you are currently using. (i.e uno, mega2560, etc.)
# BOARD_TAG         = mega2560
# BOARD_TAG         = pro328

### For Arduino 1.6.x and 1.8.x
### Both BOARD_TAG and BOARD_SUB are needed. They must be set to the board you are currently using. (i.e BOARD_TAG = uno, mega, etc. & BOARD_SUB = atmega2560, etc.)
### Note: for the Arduino Uno, only BOARD_TAG is mandatory and BOARD_SUB can be equal to anything
BOARD_TAG         = uno
BOARD_SUB         = atmega328p

### MONITOR_BAUDRATE
### It must be set to Serial baudrate value you are using.
MONITOR_BAUDRATE  = 115200

MONITOR_CMD       = picocom

### AVRDUDE
### Path to avrdude directory.
AVRDUDE           = /opt/local/bin/avrdude
AVRDUDE_CONF      = /opt/local/etc/avrdude.conf

### CFLAGS_STD
CFLAGS_STD        = -std=gnu11

### CXXFLAGS_STD
CXXFLAGS_STD      = -std=gnu++11

### CPPFLAGS
### Flags you might want to set for debugging purpose. Comment to stop.
CXXFLAGS         = -pedantic -Wall -Wextra

### If avr-gcc -v is higher than 4.9, activate coloring of the output
ifeq "$(AVR_GCC_VERSION)" "1"
    CXXFLAGS += -fdiagnostics-color
endif

### MONITOR_PORT
### The port your board is connected to. Using an '*' tries all the ports and finds the right one.
MONITOR_PORT      = /dev/tty.usbserial-A703X2SY

### OBJDIR
### This is were you put the binaries you just compile using 'make'
CURRENT_DIR       = $(shell basename $(CURDIR))
OBJDIR            = $(PROJECT_DIR)/bin/$(CURRENT_DIR)/$(BOARD_TAG)

### Setup dispatcher: Delivers configuration settings for specific hardware setups.
### Find a blueprint and further documentation in User.mk.
ifneq "$(MAKEFILE_USER)" ""
    include $(MAKEFILE_USER)
endif

### path to Arduino.mk, inside the ARDMK_DIR, don't touch.
include $(ARDMK_DIR)/Arduino.mk

### FWBUILDER_DIR
### Include helpers from Firmware-Builder directory.
FWBUILDER_DIR     = $(PROJECT_DIR)/tools/Firmware-Builder
include $(FWBUILDER_DIR)/Helper.mk

