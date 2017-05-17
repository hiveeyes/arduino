# ------------------------
# Add custom library paths
# ------------------------


# Intro
# =====
# Setup custom library paths **additionally** to the
# autodetection machinery of *Arduino Makefile*.
#
# Didn't grok neither https://github.com/sudar/Arduino-Makefile/pull/252
# nor https://github.com/sudar/Arduino-Makefile/issues/349, so stuck to
# technique from https://github.com/openxc/vi-firmware/blob/master/src/platform/pic32/pic32.mk


# Libraries
# =========


# YunBridgeLibrary
# ----------------
# YunBridgeLibrary 1.6.3 by Arduino
# The Bridge library simplifies communication between the ATmega32U4 and the AR9331.
#
# http://www.arduino.cc/en/Reference/YunBridgeLibrary
# https://github.com/arduino-libraries/Bridge
#
BRIDGE_LIB_PATH     := $(USER_LIB_PATH)/YunBridgeLibrary/src
CXXFLAGS            += -I$(BRIDGE_LIB_PATH)


# Watterott Arduino-Libs
# ----------------------
# digitalWriteFast - Fast pin access for AVR
#
# https://github.com/watterott/Arduino-Libs/tree/master/digitalWriteFast
#
FASTPIN_LIB_PATH    := $(USER_LIB_PATH)/Watterott-Arduino-Libs/digitalWriteFast
CPPFLAGS            += -I$(FASTPIN_LIB_PATH)


# Adafruit DHT-sensor-library
# ---------------------------
# Arduino library for DHT11, DHT22, etc. Temp & Humidity Sensors
#
# https://learn.adafruit.com/dht
# https://github.com/adafruit/DHT-sensor-library
#
DHT_LIB_PATH        := $(USER_LIB_PATH)/Adafruit_DHT
CPPFLAGS            += -I$(DHT_LIB_PATH)
LOCAL_CPP_SRCS      += $(wildcard $(DHT_LIB_PATH)/*.cpp)


# RunningMedian
# -------------
# RunningMedian 0.1.13 by Rob Tillart
# http://playground.arduino.cc/Main/RunningMedian
# https://github.com/RobTillaart/Arduino/tree/master/libraries/RunningMedian
#
# This library is nested inside a bigger repository. Let's counter this.
#
MEDIAN_LIB_PATH     := $(USER_LIB_PATH)/RobTillaart-Arduino/libraries/RunningMedian
CPPFLAGS            += -I$(MEDIAN_LIB_PATH)
LOCAL_CPP_SRCS      += $(wildcard $(MEDIAN_LIB_PATH)/*.cpp)
