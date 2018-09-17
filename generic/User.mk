#
# User-specific overrides for Makefile.mk
#
# Synopsis:
#
#   # Copy blueprint to specific instance reflecting your hardware setup
#   cp User.mk User_Acme_RadioTandem.mk
#
#   # Run appropriate Makefile
#   make -f Makefile-OSX.mk MAKEFILE_USER=User_Acme_RadioTandem.mk HE_ROLE=gateway
#


### HE_ROLE dispatcher
ifneq "$(HE_ROLE)" ""
    CXXFLAGS_STD += -DHE_ROLE=$(HE_ROLE)
    CXXFLAGS_STD += -DCUSTOM_CONFIG=config_$(HE_ROLE).h
endif



##########################################################
### "Node" device: Board and programming configuration ###
##########################################################
ifeq "$(HE_ROLE)" "node"

### BOARD_TAG & BOARD_SUB for Arduino IDE 1.0.x
BOARD_TAG         = pro

### BOARD_TAG for Arduino IDE 1.6.x
#BOARD_TAG         = uno
BOARD_SUB         = 8MHzatmega328

### MONITOR_PORT
### The port your board is connected to. Using an '*' tries all the ports and finds the right one.
MONITOR_PORT       = /dev/ttyUSB0

### MONITOR_BAUDRATE: Use appropriate Serial baudrate
MONITOR_BAUDRATE  = 115200

### Program to access UART TTY
MONITOR_CMD       = picocom

endif


##########################################################
### "weather" device: Board and programming configuration ###
##########################################################
ifeq "$(HE_ROLE)" "weather"

### BOARD_TAG & BOARD_SUB for Arduino IDE 1.0.x
BOARD_TAG         = pro

### BOARD_TAG for Arduino IDE 1.6.x
#BOARD_TAG         = uno
BOARD_SUB         = 8MHzatmega328

### MONITOR_PORT
### The port your board is connected to. Using an '*' tries all the ports and finds the right one.
MONITOR_PORT       = /dev/ttyUSB0

### MONITOR_BAUDRATE: Use appropriate Serial baudrate
MONITOR_BAUDRATE  = 115200

### Program to access UART TTY
MONITOR_CMD       = picocom

endif


#############################################################
### "Gateway" device: Board and programming configuration ###
#############################################################
ifeq "$(HE_ROLE)" "gateway"

BOARD_TAG         = uno
MONITOR_CMD       = picocom
MONITOR_PORT       = /dev/ttyUSB1
MONITOR_BAUDRATE  = 115200

endif

##################################################################
### "Gateway_7688" device: Board and programming configuration ###
##################################################################
ifeq "$(HE_ROLE)" "gateway7688"

# LinkIt Smart 7688 Duo Hiveeyes, 2017-04-02


# BOARD_TAG:
# $ 'make show_boards' shows a list
BOARD_TAG = smart7688
#F_CPU = 16000000L
#BOARD_SUB = atmega32u4
#MCU = atmega32u4
ISP_PORT          = /dev/ttyACM0

endif
