# -*- coding: utf-8 -*-
# (c) 2015 Andreas Motl <andreas.motl@elmyra.de>
#
# This program is free software: you can redistribute it and/or
# modify it under the terms of the GNU General Public License.
#
# ==========================================
#  Minimal Makefile for producing firmwares
#  and running them in simulavr.
#  See README.rst
# ==========================================
#
# ------------------------------------------
#    configure which program to compile
#    remark: can be overwritten from
#            command line
# ------------------------------------------
name      := avr-hello
name      := avr-sim
name      := arduino-vector



# ------------------------------------------
#           configuration options
# ------------------------------------------
#compiler := /opt/local/bin/avr-gcc
#compiler := /opt/local/bin/avr-g++
#compiler := /usr/share/arduino/hardware/tools/avr/bin/avr-gcc
compiler := avr-gcc
cppflags := -mmcu=atmega328p -DF_CPU=8000000L -DARDUINO=105 -ffunction-sections -fdata-sections -g -Os -w -fno-exceptions

define INCLUDE_NATIVE
		-I/opt/local/avr/include \
		-I/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/cores/arduino \
		-I/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/variants/standard \
		-I/Users/amo/dev/foss/open.nshare.de/beradio/tmp/StandardCplusplus \
		-I.
endef

define INCLUDE
		-I/opt/local/avr/include \
		-I/Users/amo/.platformio/packages/framework-arduino-avr/cores/arduino \
		-I/Users/amo/.platformio/packages/framework-arduino-avr/variants/standard \
		-I/Users/amo/dev/hiveeyes/sources/arduino/libraries/TerkinData/examples/.pio/libdeps/avr328/ArduinoSTL/src \
		-I.
endef

simulavr          := /Users/amo/dev/foss/open.nshare.de/simulavr/src/simulavr
simulavr_options  := --verbose --cpufrequency 8000000 --device atmega328
simulavr_hook     := --terminate=exit --writetopipe=0x91,- --writetoabort=0x95 --writetoexit=0x96



# ------------------------------------------
#                compilation
# ------------------------------------------
source   := $(name).cpp
elf      := $(name).elf
hex      := $(name).hex

all: elf hex

hex:
	avr-objcopy -O ihex -R .eeprom $(elf) $(hex)

elf:
	@echo ------------------------------------------
	@echo   Compiling
	@echo ------------------------------------------
	$(compiler) $(cppflags) $(INCLUDE) -o $(elf) $(source)

clean:
	rm $(elf) $(hex)

clean-all:
	rm *.elf *.hex


# ------------------------------------------
#                simulation
# ------------------------------------------
sim: elf
	@echo ------------------------------------------
	@echo   Simulating
	@echo ------------------------------------------
	# http://www.nongnu.org/simulavr/intro.html
	$(simulavr) $(simulavr_options) $(simulavr_hook) --file $(elf)
