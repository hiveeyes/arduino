.. include:: ../resources.rst

.. _beradio-c++:

.. _libberadio:

###########
BERadio C++
###########


*****
Intro
*****
`BERadio C++ <BERadio-Lib_>`_ is a software library
for transmitting telemetry data over narrow-bandwidth
radio links, all on embedded low-power devices.
It implements the :ref:`beradio` specification
and provides convenient API methods and data
structures.

`BERadio C++ <BERadio-Lib_>`_ was tested successfully on
Arduino (ATmega328p Pro/Uno), SimulAVR and x86_64 (both Mac OS X) .


********
Synopsis
********
.. highlight:: cpp

::

    // Message object with node id "999" and communication profile "h1"
    BERadioMessage *message = new BERadioMessage(999, "h1");

    // Collect some measurement values
    FloatList *temperatures = new FloatList();
    temperatures->push_back(21.21);
    temperatures->push_back(42.42);
    // ...
    message->add("t", *temperatures);

    // Encode, fragment and transmit message
    message.set_mtu_size(61);
    message->transmit();

    // Free memory
    delete temperatures;


************
Dependencies
************


StandardCplusplus
=================
    | Standard C++ for Arduino (port of uClibc++)
    | https://github.com/maniacbug/StandardCplusplus
    | https://github.com/hiveeyes/StandardCplusplus


EmBencode
=========
    | Bencode protocol support for embedded processors
    | https://github.com/jcw/embencode
    | https://github.com/hiveeyes/embencode


Terrine
=======
    | Application boilerplate for convenient MCU development
    | https://github.com/hiveeyes/arduino/tree/develop/libraries/Terrine

