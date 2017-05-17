# makeEspArduino: A makefile for ESP8266/Arduino projects.
# https://github.com/plerup/makeEspArduino

# Announce path to your "ESP8266 Arduino Core" installation
# Can also be announced via environment variable: export ESP_ROOT=...
# ESP_ROOT=~/dev/hiveeyes/sdk/esp8266-arduino-core-2.3.0/Arduino-2.3.0

# Attempt to autodetect libraries: Does not work because it would try to compile LowPower libs and more.
#LIBS = $(shell perl -e 'use File::Find;$$d = shift;while (<>) {$$f{"$$1"} = 1 if /^\s*\#include\s+[<"]([^>"]+)/;}find(sub {print $$File::Find::dir," " if $$f{$$_}}, $$d);'  $(ESP_ROOT)/libraries $(SKETCH))
#LIBS += $(shell perl -e 'use File::Find;$$d = shift;while (<>) {$$f{"$$1"} = 1 if /^\s*\#include\s+[<"]([^>"]+)/;}find(sub {print $$File::Find::dir," " if $$f{$$_}}, $$d);'  ../libraries $(SKETCH))
#LIBS -= ../libraries/LowPower

# Manually enumerate libraries, this is currently the only deterministic way.
LIBS = \
    $(ESP_LIBS)/esp8266 \
    $(ESP_LIBS)/GDBStub \
    $(ESP_LIBS)/SD \
    $(ESP_LIBS)/SPI \
    $(ESP_LIBS)/Wire \
    $(ESP_LIBS)/ESP8266WiFi \
    $(ESP_LIBS)/ESP8266HTTPClient \
    ../libraries/ADS1231 \
    ../libraries/OneWire \
    ../libraries/DallasTemperature \
    ../libraries/RobTillaart-Arduino/libraries/DHTstable \
    ../libraries/RobTillaart-Arduino/libraries/RunningMedian

# For ESP8266 (makeEspArduino)
BUILD_EXTRA_FLAGS += -DARDUINO_ARCH_ESP8266=true

# Run makeEspArduino
include ../tools/makeEspArduino/makeEspArduino.mk
