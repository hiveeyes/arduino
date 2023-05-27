###########
Development
###########

.. contents::
   :local:
   :depth: 1

----

****
Todo
****
Have a look at the agenda at :ref:`todo`.


.. _hacking:

*******
Hacking
*******
For hacking on the repository, there are some guidelines.

Initialize a new firmware flavour
=================================
The following steps are needed to initialize a new project inside the multivariant repository::

    # Create new project directory following the naming convention.
    mkdir purpose-transport-protocol

    # Initialize project directory by bootstrapping from another project.
    cp generic/{Makefile,platformio.ini} purpose-transport-protocol/


Libraries as git submodules
===========================
Dependency libraries are organized as git submodules inside the ``libraries/`` folder.
This avoids redundancy and delivers a coherent sandbox environment without too much
overhead on artifact maintenance.


Sync submodules
---------------
To sync newly added libraries, do::

    git submodule update --recursive --init


Add library
-----------
To add a new library as a submodule, do the following::

    git submodule add https://mightbegithub.com/author/repo-name libraries/repo-name


Update library
--------------
To update submodules to the latest commit available from their remote::

    # Change to the submodule directory
    cd submodule_dir

    # Checkout desired branch
    git checkout master

    # Update
    git pull

    # Get back to your project root
    cd ..

    # Now the submodule is in the state you want, so
    git commit -am "Pulled update to submodule_dir"

.. seealso:: https://stackoverflow.com/questions/5828324/update-git-submodule-to-latest-commit-on-origin/5828396#5828396


.. _simulation:

**********
Simulation
**********
There are some ways to simulate the MCU code on your workstation.

Run on Unix
===========
::

    cd generic

    # Build arch=x86_64, not Arduino
    ./sim/build

    # Receive data on localhost:4000 (TCP)
    socat tcp-listen:4000,reuseaddr -

    # Run "generic" program on local workstation, it
    # will send data to localhost:4000 through RH_TCP.
    ./sim/generic

::

    file ./sim/generic
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

    BERadio/research
    BERadio/firmware-size

.. seealso:: Specification and more at :ref:`beradio`.

