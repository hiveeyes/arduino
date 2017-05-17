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


# ArduinoSTL
# ----------
# ArduinoSTL 1.0.1 by Mike Matera
# https://github.com/mike-matera/ArduinoSTL
# https://github.com/hiveeyes/ArduinoSTL
#
# A STL and iostream implementation for Arduino based on uClibc++.
#
STL_LIB_PATH        := $(USER_LIB_PATH)/ArduinoSTL
CPPFLAGS            += -I$(STL_LIB_PATH)
LOCAL_CPP_SRCS      += $(wildcard $(STL_LIB_PATH)/src/*.cpp)


# DHTlib
# ------
# DHTlib 0.1.13 by Rob Tillart
# http://playground.arduino.cc/Main/DHTLib
# https://github.com/RobTillaart/Arduino/blob/master/libraries/DHTlib
#
# This library is made of a "dht.{h,cpp}" (small caps!). As this is prone to
# collisions in the main library folder, this gets included by manipulating
# the appropriate Arduino Makefile parameters.
#
# TODO: This is an old version, upgrade to the recent version named "libDHT"
#       https://github.com/RobTillaart/libDHT
#
DHT_LIB_PATH        := $(USER_LIB_PATH)/RobTillaart-Arduino/libraries/DHTstable
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
