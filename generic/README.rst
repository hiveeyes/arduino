.. include:: ../../resources.rst

.. _generic-firmware:

################
Generic firmware
################

.. ifconfig:: False

    .. tip::

        You might want to `read this document on our documentation space <https://hiveeyes.org/docs/arduino/firmware/generic/README.html>`_,
        all inline links will be working there.


************
Introduction
************
A flexible software breadboard for sensor-, transceiver- and gateway-nodes
transmitting telemetry data in radio link environments.
The firmware (`config.h`_, `generic.ino`_) is based on the fine RadioHead_
and :ref:`beradio-c++` libraries as well as a bunch of other ones.

.. note:: This is currently a work in progress, as of June 2016.


********
Synopsis
********

RF multi-hop yak shaving.

.. graphviz:: synopsis.dot

This firmware can satisfy different purposes:

    - A sensor node collects sensor data, encodes it with :ref:`BERadio`
      and sends it through a RFM69 radio module to a gateway node
      or a protocol transceiver node.

    - A protocol transceiver node receives radio signals on RFM69
      and emits them to RFM95 (LoRa). The messages are processed
      opaque, no decoding takes place here.

    - A gateway node receives RFM95 (LoRa) radio signals and emits
      the payloads to its UART interface connected to the gateway
      SoC (e.g. a RaspberryPi) running the :ref:`BERadio forwarder <beradio-python>`.

The :ref:`BERadio forwarder <beradio-python>` will decode the data according
to the :ref:`beradio-spec` and forward it to the MQTT bus serialized as JSON
dictionary, all of which is beyond the scope of this MCU code.


*********
Workbench
*********
.. figure:: https://ptrace.hiveeyes.org/2016-07-08_Hiveeyes%20RadioHead%20Trio%20-%20RFM69%2CRFM95%2CUART.png
    :alt: Three nodes, each running the "generic" firmware in different roles.
    :width: 800px

    Three nodes, each running the "generic" firmware in different roles:

        - A sensor node emitting telemetry data via RFM69 (left)
        - A transceiver node receiving RFM69 and emitting RFM95 LoRa (middle)
        - A gateway node receiving RFM95 LoRa and sending it to the UART interface (right)

.. figure:: https://ptrace.hiveeyes.org/2016-07-08_Hiveeyes%20generic%20node%20RFM69-to-RFM95%20picocom.png
    :alt: Three picocom instances connected to each of the three networked nodes.
    :width: 800px

    Three picocom instances connected to each of the three networked nodes.


*****
Build
*****
.. highlight:: bash


Build on your workstation
=========================
::

    # Get hold of the source code repository including all dependencies
    git clone --recursive https://github.com/hiveeyes/arduino

    # Select this firmware
    cd generic

    # Select appropriate Makefile
    ln -s Makefile-Linux.mk Makefile

    # Build firmware
    make

    # Upload to MCU
    make upload

.. note:: You might want to adjust the appropriate Makefile to match your environment.

