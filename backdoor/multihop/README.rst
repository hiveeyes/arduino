.. include:: ../../../resources.rst

.. _generic-firmware:
.. _multihop-firmware:

#################
multihop firmware
#################

» `Documentation <About the generic firmware_>`_
| `Discussions <Embedded Radio-to-IP Gateway (AVR)_>`_
| `Issues`_
| `Source code`_

.. tip::

    Please `continue reading this document on our documentation space <https://hiveeyes.org/docs/arduino/>`_,
    all inline links will be working there.


*****
About
*****

A flexible software breadboard for sensor-, relay-, and gateway-nodes, transmitting
telemetry data in multi-hop radio link environments. It is a hybrid/unified single-file
firmware `generic.ino`_, with an accompanying configuration settings header file, `config.h`_.

The system has originally been conceived for beehive monitoring, but can be used for
other environmental monitoring purposes.

Hardware
========

The firmware has been primarily conceived for MCUs of the `AVR microcontroller family`_,
specifically the `Atmel`_ `ATmega328`_ (now `Microchip`_), in form of the vanilla `Arduino Uno`_,
the `JeeLink v3c`_, and with a spin-off to the `LinkIt Smart 7688 Duo`_.

For sensor hardware, it uses the usual suspects `HX711`_, `DS18B20`_, and `DHT22`_, for weight-,
temperature-, and humidity-measurements.

Software
========

The telemetry subsystem is based on the excellent `RFM69`_, `RadioHead`_, and :ref:`beradio-c++`
libraries.


********
Synopsis
********

`Multi-hop`_ RF / ISM / packet radio and telemetry `yak shaving`_.

.. graphviz:: multihop.dot

*The firmware serves different purposes, you can configure it as a »sensor node«,
»telemetry relay node«, or »network gateway node«.*

- A **sensor node** collects sensor data, encodes it using :ref:`BERadio`, and sends it
  through a RFM69 radio module directly to a network gateway node, or to an intermediary
  telemetry relay node.

- A **telemetry relay node** receives radio signals on RFM69 and emits them to RFM95
  (LoRa). The messages are processed opaque, no decoding takes place here.

- The **network gateway node** receives RFM95 (LoRa) radio signals and emits the message
  payloads to its UART interface connected to the gateway computer.

- The **gateway computer**, for example a Linux machine like the Raspberry Pi, is running
  the :ref:`BERadio forwarder <beradio-python>` application. It will decode the data
  according to the :ref:`beradio-spec`, and forward it to the TCP/IP network using the
  `MQTT`_ protocol, serialized as JSON dictionary.


*********
Workbench
*********

.. figure:: https://ptrace.hiveeyes.org/2016-07-08_Hiveeyes%20RadioHead%20Trio%20-%20RFM69%2CRFM95%2CUART.png
    :alt: Three nodes, each running the "generic" firmware in a different configuration.
    :width: 800px

    Three nodes, each running the "generic" firmware in a different configuration.
    A »sensor node« emitting telemetry data via RFM69 (left),
    a »telemetry relay node« receiving RFM69 and emitting RFM95 LoRa (middle), and
    a »network gateway node« receiving RFM95 LoRa and sending it to the UART interface (right),
    effectively plugged into a Linux computer's USB port.

.. figure:: https://ptrace.hiveeyes.org/2016-07-08_Hiveeyes%20generic%20node%20RFM69-to-RFM95%20picocom.png
    :alt: Three picocom instances connected to each of the three networked nodes.
    :width: 800px

    Three `picocom`_ instances connected to each of the three networked nodes.


*****
Usage
*****

.. highlight:: bash


Build on your workstation
=========================
::

    # Acquire the source code repository including all dependencies.
    git clone https://github.com/hiveeyes/arduino

    # Select this firmware.
    cd generic

    # Build firmware.
    make build

    # Upload to MCU.
    make upload

.. tip:: Please inspect the ``platformio.ini`` file, and adjust it to match your setup.


Configuration
=============

In order to configure and build the firmware for different operation modes (node vs.
relay vs. gateway), the `PlatformIO`_ configuration file ``platformio.ini``
provides corresponding environment sections, which you can adjust to your needs.

Try those commands to invoke builds for the individual target environments::

    # Activate Python virtualenv, where your PlatformIO installation lives.
    source .venv/bin/activate

    # Run build for individual environments.
    pio run --environment=node-rfm69
    pio run --environment=node-rh69
    pio run --environment=relay
    pio run --environment=gateway-rh69
    pio run --environment=gateway-rh95
    pio run --environment=gateway-rh95-linkit7688

In order to adjust individual build flags for your setup, we recommend to add a custom
environment section to the ``platformio.ini`` file. Alternatively, you can build with
a custom configuration settings header file, using the ``-D CUSTOM_CONFIG=config_foobar.h``
directive.

You can also invoke the build by defining ``build_flags`` on the command line, like::

    PLATFORMIO_BUILD_FLAGS="-D CUSTOM_CONFIG=config_foobar.h" pio run --environment=relay


.. _About the generic firmware: https://community.hiveeyes.org/t/about-the-generic-firmware/317
.. _Embedded Radio-to-IP Gateway (AVR): https://community.hiveeyes.org/t/embedded-radio-to-ip-gateway-avr/314
.. _Issues: https://github.com/hiveeyes/arduino/issues
.. _Source code: https://github.com/hiveeyes/arduino/tree/main/backdoor/multihop
