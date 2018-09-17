.. include:: ../resources.rst

.. _beradio-c++:

.. _libberadio:

###########
BERadio C++
###########

.. contents::
   :local:
   :depth: 1

----

.. tip::

    You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/BERadio/README.html>`_,
    all inline links will be working there.


************
Introduction
************
`BERadio C++`_ is a software library
for transmitting telemetry data over radio links with
narrow bandwidth, all on embedded low-power devices.

Its serialization format is Bencode_ according to the
:ref:`beradio:beradio-spec`, the C++ interface provides
convenient API methods and container data structures
for working with variable amounts of measurement
values.

By featuring automatic message fragmentation, data transmission
is safe, even when using radio transceivers with constrained
payloads. Yet, it is reasonably compact and still readable by
humans (8-bit clean).

BERadio C++ was tested successfully on
Arduino (ATmega328p Pro/Uno), SimulAVR and x86_64 (both Mac OS X).
It compiles with *avr-g++ 4.8, 4.9 and 5.2* as well as *clang++ 3.4*
on Mac OS X and Archlinux.



********
Synopsis
********
.. highlight:: cpp

A convenient C++ API for creating and transmitting message objects::

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

.. highlight:: none

This will serialize measurement values to Bencode_
using EmBencode_::

    d1:#i999e1:_2:h11:tli2121ei4242eee


.. hint::

    A scaling factor of ``* 100`` was applied to all floating point values,
    giving an effective precision of 2 digits.



********
Download
********

BERadio C++
===========
    | Please download from GitHub:
    | https://github.com/hiveeyes/arduino/tree/master/libraries/BERadio


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
    | https://github.com/hiveeyes/arduino/tree/master/libraries/Terrine

****
TODO
****

- prevent sending empty lists
- allow sending plain int's
