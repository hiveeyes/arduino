###################
Development sandbox
###################


****
Todo
****

.. todo::

    Have a look at the agenda at :ref:`todo`.


.. _hacking:

*******
Hacking
*******

For hacking on the repository, there are some guidelines.

Initialize a new firmware flavour
=================================
The following steps are needed to initialize a new project inside the
multi-variant firmware repository::

    # Create new project directory following the naming convention.
    mkdir -p origin/purpose-transport-protocol

    # Initialize project directory by bootstrapping from another project.
    cp backdoor/node-wifi-mqtt/{Makefile,platformio.ini} origin/purpose-transport-protocol/


.. _simulation:

**********
Simulation
**********
There are some ways to simulate the MCU code on your workstation.

Run on Unix
===========
::

    cd backdoor/multihop

    # Build arch=x86_64, not Arduino
    ./sim/build

    # Receive data on localhost:4000 (TCP)
    socat tcp-listen:4000,reuseaddr -

    # Run "generic" program on local workstation, it
    # will send data to localhost:4000 through RH_TCP.
    ./sim/multihop

::

    file ./sim/multihop
    sim/generic: Mach-O 64-bit x86_64 executable


Run in SimulAVR
===============
::

    cd libraries/BERadio/examples/message

    # Edit Makefile and enable line "include Makefile-SimulAVR.mk"

    make sim-all


********
Research
********

.. toctree::
    :maxdepth: 1
    :glob:

    ../BERadio/research
    ../BERadio/firmware-size

.. seealso:: Specification and more at :ref:`beradio`.
