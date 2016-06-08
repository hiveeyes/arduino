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

If you use Ino_, you can start right over after cloning the repository recursively::

    $ git clone --recursive https://github.com/hiveeyes/arduino

Otherwise, if you want to use the Arduino IDE you have to install all necessary libraries manually,
see: https://www.arduino.cc/en/Guide/Libraries. The sources of the libraries can be found in the header of each sketch (.ino).
In this case you clone the repository, in the normal manner::

    $ git clone https://github.com/hiveeyes/arduino

Or simply copy the needed sketch into your Arduino IDE.

===========
Install Ino
===========
::

    $ virtualenv-2.7 --no-site-packages .venv27
    $ source .venv27/bin/activate
    $ pip install ino

or visit http://inotool.org/#installation


*******
Operate
*******

Arduino-Makefile
================

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

You can also use ino tool directly to make it compile and upload your code::

    $ ino list-models
    $ ino build -m BOARD_MODEL
    $ ino upload -m BOARD_MODEL -p SERIAL_PORT

To get the serial output from the arduino, do::

    $ ino serial -p SERIAL_PORT -b BAUD_RATE

The code directory may contain a `ino.ini` file, in which the settings may be set::

     [build]
     board-model = pro328

     [upload]
     board-model = pro328
     serial-port = /dev/ttyUSB1

     [serial]
     serial-port = /dev/ttyUSB1
     baud-rate = 115200

To build, upload and print the serial output at once, just run::

    $ make

Further information about ino are found at http://inotool.org/



Libraries
=========

The libraries for this arduino repository are organized on a central place; `libraries/` and are symlinked into the flavours lib folders 
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

