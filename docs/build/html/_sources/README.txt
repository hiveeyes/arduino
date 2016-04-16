################
Hiveeyes Arduino
################

This is the hiveeyes arduino repository. All Arduino based code (including ESP8266) of the project can be found here. To allow different flavours the naming convention is ``purpose-transport-protocol``.

* ``purpose``   could be either node or gateway
* ``transport`` could be the physical transport mechanism, e.g. like RFM69-radio, LoRa-radio, GSM or WiFi 
* ``protocol``  aimes for the protocol the following communication instance would be connected with

The reporitory is designed for use it with ino-tool, see: inotool.org . Ino-tool is a alternative to the Arduino IDE, which let you use your favourite editor and compile from the commandline. To start over you simply install ino-tool for your linux and recursivly clone this repository.

======================
Cloning the repository
======================

If you use ino-tool, you can start right over after cloning the repository recursive::

    $ git clone --recursive https://github.com/hiveeyes/arduino

Otherwise, if you want to use the Arduino IDE you have to install all necessary libraries manualy,
see: https://www.arduino.cc/en/Guide/Libraries. The sources of the libraries can be found in the header of each sketch (.ino).
In this case you clone the repository, in the normal manner::

    $ git clone https://github.com/hiveeyes/arduino

Or simply copy the needed sketch into your Arduino IDE.

========================================
Setup inotool - using phytons virtualenv
========================================
::

    $ virtualenv-2.7 --no-site-packages .venv27
    $ source .venv27/bin/activate
    $ pip install ino

or visit http://inotool.org/#installation

=========
Libraries
=========

The libraries for this arduino repository are organized on a central place; `libraries/` and are symlinked into the flavours lib folders 
To clone the repository with the all the libraries do::

    $ git clone --recursive https://github.com/hiveeyes/arduino

In order to update newly added libraries in submodules do::

    $ git submodule update --recursive --init

=======
Hacking
=======

********************************
initialize a new arduino flavour
********************************

The following steps are needed to initialize a new project in ino style::

    $ mkdir purpose-transport-protocol                    # make new project directory, following our naming convention
    $ cd purpose-transport-protocol                       # change dir
    $ ino init                                            # creates a src/ and lib/ folder. Alternativly do it manual
    $ rmdir lib/                                          # removes lib/ ;)
    $ cd ..                                               # change dir to repository root
    $ ln -sr libraries/ purpose-transport-protocol/lib    # create a relative symlink to the repositories library folder

********************
adding new libraries
********************

The libraries a central organized as git submodules in `libraries/` . To add new submodules do the followling::

    $ cd arduino/
    $ git submodule add https://mightbegithub.com/author/repo-name libraries/repo-name


