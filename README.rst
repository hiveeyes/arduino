.. include:: resources.rst

.. _README:

######
README
######

.. contents::
   :local:
   :depth: 1

----

*****
Intro
*****
This is the Hiveeyes Arduino repository. All Arduino based code (for ATmega328_ and ESP8266_) of the project can be found here.
To allow different flavours, the directory naming convention is ``<purpose>-<transport>-<protocol>``.

* ``purpose``   could be either node or gateway
* ``transport`` could be the physical transport mechanism, e.g. like RFM69-radio, LoRa-radio, GSM or WiFi
* ``protocol``  aimes for the protocol the following communication instance would be connected with

The repository is designed for use it with Ino_, a command line toolkit for working with Arduino hardware.
Ino_ is an alternative to the Arduino IDE, which let you use your favourite editor and compile from the commandline.
To start over you simply install Ino_ on your system and recursively clone this repository.

.. todo:: We don't use Ino any more, but switched to `Arduino-Makefile`_ instead.

.. todo:: List and describe current flavors.


*****
Setup
*****
.. highlight:: bash


Source code
===========
Get source code with all dependency libraries and tools::

    $ git clone --recursive https://github.com/hiveeyes/arduino

Toolchain
=========

Debian packages
---------------
::

    apt-get install arduino-core


Archlinux packages
------------------
* aur/arduino 1:1.6.8
* community/avr-libc


Details
=======

Git submodules
--------------
The libraries for this repository are organized in the central place ``libraries/``.
In order to update newly added libraries in submodules do::

    $ git submodule update --recursive --init

Arduino IDE
-----------
If you want to use the Arduino IDE, you have to install all necessary libraries manually.
The sources of the libraries can be found in the header of each sketch (.ino).
In this case, clone the repository without fetching git submodule contents::

    $ git clone https://github.com/hiveeyes/arduino

Or simply copy the desired sketch into your Arduino IDE.

.. seealso:: https://www.arduino.cc/en/Guide/Libraries


*******
Operate
*******
For operative tasks like compiling, `Arduino-Makefile`_ is in the loop.


Compile
=======
::

    cd node-gprs-any
    make --file Makefile-OSX.mk

After a successful build, find the firmware at ``bin/node-gprs-any/pro328/node-gprs-any.hex``.


Run on chip
===========
To build a firmware hex file, upload it to the MCU and
finally start a serial port monitor, do::

    cd generic

    # Prepare appropriate Makefile
    ln -s Makefile-OSX.mk Makefile

    # Compile and upload to chip
    make upload

    # Run serial port monitor
    make monitor


Run on Unix
===========
::

    cd generic

    # Build arch=x86_64, not Arduino
    ./sim/build

    # Receive data on localhost:4000 (TCP)
    socat tcp-listen:4000,reuseaddr -

    # Run "generic" program on local workstation, it
    # will send data to localhost:4000 through RH_TCP.
    ./sim/generic

::

    file ./sim/generic
    sim/generic: Mach-O 64-bit x86_64 executable


Run in SimulAVR
===============
::

    cd libraries/BERadio/examples/message

    # Edit Makefile and enable line "include Makefile-SimulAVR.mk"

    make sim-all





********
Appendix
********

Initialize a new Arduino flavour
================================
The following steps are needed to initialize a new project.

Using ``ino``::

    $ mkdir purpose-transport-protocol                    # make new project directory, following our naming convention
    $ cd purpose-transport-protocol                       # change dir
    $ ino init                                            # creates a src/ and lib/ folder. Alternativly do it manual
    $ rmdir lib/                                          # removes lib/ ;)
    $ cd ..                                               # change dir to repository root
    $ ln -sr libraries/ purpose-transport-protocol/lib    # create a relative symlink to the repositories library folder

Using Arduino-Makefile::

    $ mkdir purpose-transport-protocol                    # make new project directory, following our naming convention
    $ cp tools/Bare-Arduino-Project/Makefile-* node-gprs-any/  # install tooling


Add new libraries
=================
The libraries are organized as git submodules inside the `libraries/` folder. To add new submodules, do the following::

    $ cd arduino/
    $ git submodule add https://mightbegithub.com/author/repo-name libraries/repo-name

