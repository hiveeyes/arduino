.. include:: ../resources.rst

.. _beradio-c++:

.. _libberadio:

###################
BERadio C++ library
###################


*****
Intro
*****
BERadio C++ is a convenient library for doing
telemetry over narrow-bandwidth radio links
on embedded low-power devices.

It defines an API and a data format, both
specified by :ref:`beradio`.


********
Synopsis
********
.. highlight:: cpp

::

    // Message object with node id "999" and communication profile "h1"
    BERadioMessage *message = new BERadioMessage(999, "h1");

    // Collect some measurement values
    FloatList *tempL = new FloatList();
    tempL->push_back(42.42);
    message->add("t", *tempL);

    // Encode, fragment and transmit message
    message.set_mtu_size(61);
    message->transmit();


************
Dependencies
************


StandardCplusplus
=================
    | Standard C++ for Arduino (port of uClibc++)
    | https://github.com/hiveeyes/StandardCplusplus


EmBencode
=========
    | Bencode protocol support for embedded processors
    | https://github.com/hiveeyes/embencode


Terrine
=======
    | Application boilerplate for convenient MCU development

