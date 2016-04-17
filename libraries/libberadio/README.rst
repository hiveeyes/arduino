.. include:: ../resources.rst

.. _libberadio:

######################
libberadio c++ library
######################


*****
Intro
*****
Evaluate whether a Standard C++ based firmware will still
fit into the 32 KB flash memory of the little ATmega328_.


*****
Goals
*****

Packet size and fragmentation
=============================
Feature: Automatic packet fragmentation based on maximum payload size.

The maximum payload length is defined in `RFM69.h#L35`_::

    // to take advantage of the built in AES/CRC we want to limit the frame size
    // to the internal FIFO size (66 bytes - 3 bytes overhead - 2 bytes crc)
    #define RF69_MAX_DATA_LEN       61


.. _RFM69.h#L35: https://github.com/LowPowerLab/RFM69/blob/master/RFM69.h#L35


************
Dependencies
************

StandardCplusplus
=================
| Standard C++ for Arduino (port of uClibc++)
| https://github.com/maniacbug/StandardCplusplus

embencode
=========
| Bencode protocol support for embedded processors
| https://github.com/jcw/embencode

embencode patch
===============
Please apply patch ``libraries/libberadio/embencode.patch``.
This makes all methods non-static and adds the possibility to
override the method ``PushChar`` by inheriting classes.
This is required for future automatic message fragmentation
done by libberadio, see ``BERadioEncoder::PushChar``.


***************
Troubleshooting
***************
Q: You are getting exceptions like::

    /var/folders/88/ql5nj4ds6314wzkx8w6b2f1m0000gn/T//ccfxeWCn.o: In function `BERadioMessage::debug(bool)':
    /Users/amo/dev/hiveeyes/sources/arduino/libraries/libberadio/examples/message/../../beradio.cpp:36: undefined reference to `EmBencode::PushChar(char)'

A: Apply the required patch to embencode, see above.
