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
#BOARD_TAG         = pro328

### BOARD_TAG for Arduino IDE 1.6.x
#BOARD_TAG         = uno
#BOARD_SUB         = atmega328p

### MONITOR_PORT
### The port your board is connected to. Using an '*' tries all the ports and finds the right one.
#MONITOR_PORT       = /dev/ttyUSB0

### MONITOR_BAUDRATE: Use appropriate Serial baudrate
#MONITOR_BAUDRATE  = 115200

### Program to access UART TTY
MONITOR_CMD       = picocom

endif


#############################################################
### "Gateway" device: Board and programming configuration ###
#############################################################
ifeq "$(HE_ROLE)" "gateway"

# RasPIO Hiveeyes, 2016-12-15
# Upload using "make -f Makefile-Linux.mk ispload"
BOARD_TAG         = gert328
MCU = atmega328p
F_CPU = 12000000L
ISP_PROG = gpio
FORCE_MONITOR_PORT = true

endif
