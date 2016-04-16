*********
Libraries
*********

The libraries for this arduino repositories are organized as central place `libraries/` and are symlinked into the flavours. 
To clone the repository with the all the libraries do::

    $ git clone --recursive https://github.com/hiveeyes/arduino




*******
Hacking
*******

===============================
intialize a new arduino flavour
===============================

The following steps are needed to intitialize a new project in ino style::

    $ mkdir purpose-transport-protocol                    # make new project directory, following our naming convention
    $ cd purpose-transport-protocol                       # change dir
    $ ino init                                            # creates a src/ and lib/ folder. Alternativly do it manual
    $ rmdir lib/                                          # removes lib/
    $ cd ..                                               # change dir to repository root
    $ ln -sr libraries/ purpose-transport-protocol/lib    # create a relative symlink to the repositories library folder

====================
adding new libraries
====================

The libraries a central organized as git submodules in `libraries/` . To add new submodules do the followling::

    $ cd arduino/
    $ git submodule add https://mightbegithub.com/author/repo-name libraries/repo-name



