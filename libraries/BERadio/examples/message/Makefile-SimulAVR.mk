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


# ------------------------------------------
#    configure which program to compile
#    remark: can be overwritten from
#            command line
# ------------------------------------------
name     := simple_message
#more     := ../../BERadio.cpp ../../simulavr.cpp


# ------------------------------------------
#           configuration options
# ------------------------------------------
#compiler := /opt/local/bin/avr-gcc
compiler := /opt/local/bin/avr-g++
#compiler := /usr/share/arduino/hardware/tools/avr/bin/avr-gcc
# remark: -std=c++11 gives nice iterator convenience (range-based ‘for’ loops are not allowed in C++98 mode), but adds another 1860 bytes
cppflags := -x c++ -std=gnu++11 -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections -Wl,-gc-sections -mcall-prologues -Wl,--relax -pedantic -fpermissive
cppflags_avr := -mmcu=atmega328p -DF_CPU=8000000L -DARDUINO=105
#cppflags_avr := -include Arduino.h -c -mmcu=atmega328p -DF_CPU=8000000L -DARDUINO=105

define INCLUDE
		-I/opt/local/avr/include \
		-I/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/cores/arduino \
		-I/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/variants/standard \
		-I../../../StandardCplusplus \
		-I../../../Terrine \
		-I../../../BERadio \
		-I../../../EmBencode \
		-I../../../RadioHead/RadioHead \
		-I../../../MemoryFree \
		-I../..
endef

simulavr          := /Users/amo/dev/foss/open.nshare.de/simulavr/src/simulavr
simulavr_options  := --verbose --cpufrequency 8000000 --device atmega328 #-c warnread
simulavr_hook     := --terminate=exit --writetopipe=0x91,- --writetoabort=0x95 --writetoexit=0x96
#simulavr_hook     := --writetopipe=0x91,- --writetoabort=0x95 --writetoexit=0x96

avr-objcopy  := /opt/local/bin/avr-objcopy


# ------------------------------------------
#                compilation
# ------------------------------------------
source   := $(name).cpp
elf      := $(name).elf
hex      := $(name).hex

build: elf hex

hex:
	$(avr-objcopy) -O ihex -R .eeprom $(elf) $(hex)

elf:
	@echo ------------------------------------------
	@echo   Compiling \"$(name)\”
	@echo ------------------------------------------
	$(compiler) $(cppflags) $(cppflags_avr) $(cppflags_more) $(INCLUDE) -o $(elf) $(source) $(more)

clean:
	rm $(elf) $(hex)

clean-all:
	rm *.elf *.hex


# ------------------------------------------
#                simulation
# ------------------------------------------
sim:
	$(MAKE) build cppflags_more=-DSIMULAVR=true
	@echo ------------------------------------------
	@echo   Simulating \"$(name)\"
	@echo ------------------------------------------
	# http://www.nongnu.org/simulavr/intro.html
	$(simulavr) $(simulavr_options) $(simulavr_hook) --file $(elf)


sim-all:
	$(MAKE) sim name=simple_message
