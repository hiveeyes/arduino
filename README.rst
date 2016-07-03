.. include:: resources.rst

######
README
######

*****
Setup
*****

Intro
=====
This is the Hiveeyes Arduino repository. All Arduino based code (for ATmega328_ and ESP8266_) of the project can be found here.
To allow different flavours, the directory naming convention is ``<purpose>-<transport>-<protocol>``.

* ``purpose``   could be either node or gateway
* ``transport`` could be the physical transport mechanism, e.g. like RFM69-radio, LoRa-radio, GSM or WiFi 
* ``protocol``  aimes for the protocol the following communication instance would be connected with

The repository is designed for use it with Ino_, a command line toolkit for working with Arduino hardware.
Ino_ is an alternative to the Arduino IDE, which let you use your favourite editor and compile from the commandline.
To start over you simply install Ino_ on your system and recursively clone this repository.


Clone the repository
====================
::

    $ git clone --recursive https://github.com/hiveeyes/arduino

Otherwise, if you want to use the Arduino IDE you have to install all necessary libraries manually,
see: https://www.arduino.cc/en/Guide/Libraries. The sources of the libraries can be found in the header of each sketch (.ino).
In this case you clone the repository, in the normal manner::

    $ git clone https://github.com/hiveeyes/arduino

Or simply copy the desired sketch into your Arduino IDE.




*******
Operate
*******

Arduino-Makefile
================

Archlinux packages
------------------
* aur/arduino 1:1.6.8
* community/avr-libc

Compile
-------
::

    cd node-gprs-any
    make --file Makefile-OSX.mk


Inotool
=======

Compile
-------

Build a sketch from the commandline::

    cd node-rfm69-beradio
    make compile

After a successful build, find the firmware at ``node-rfm69-beradio/.build/uno/firmware.hex``.

Upload
------
To build, upload and print the serial output at once, just run::

    cd generic
    make upload && make monitor


Run on Unix
-----------
::

    cd generic

    # Build arch=x86_64, not Arduino
    ./sim/build generic.ino

    # Receive data on localhost:4000
    socat tcp-listen:4000,reuseaddr -

    # Run on local workstation, will send data to localhost:4000
    ./sim/generic



Libraries
=========
The libraries for this arduino repository are organized in a central place ``libraries/``.
To clone the repository with the all the libraries do::

    $ git clone --recursive https://github.com/hiveeyes/arduino

In order to update newly added libraries in submodules do::

    $ git submodule update --recursive --init



****
Hack
****

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

