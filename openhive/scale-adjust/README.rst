.. include:: ../../../resources.rst

.. _scale-adjust-firmware:

##################################
Firmwares for load cell adjustment
##################################


************
Introduction
************
There are adjustment sketches for two different precision analog-to-digital
converters (ADC), acting as a frontend for `bridge sensors <Wheatstone bridge_>`_
commonly used in industrial `load cell`_ components:

- `scale-adjust-ads1231.ino`_ for ADS1231_ from Texas Instruments
- `scale-adjust-hx711.ino`_ for HX711_ from Avia Semiconductor


************
Instructions
************
When working with the load cell, individual parameters have to be defined.
To get hold of the appropriate values, you have to measure them to adjust
the raw sensor readings properly.

These values are not type specific! Even load cells of the same type/model
have individual characteristics, so you have to determine both per load cell.

The sketches listed above can help to discover these two values:

    - ``ZeroOffset`` is the raw sensor value for zero balance, *0 kg*.
    - ``KgDivider`` is the raw sensor value for a load of *1 kg*.

Please follow the instructions in the respective source code sketch...

.. tip::

    On some load cells, the range around 0 kg gives negative raw value readings,
    so we try to start for the *1 kg* (or whatever weight you may choose) on
    a higher range.


********
Appendix
********
If you're still reading, you might want to have a look at these fine papers:

- `Load cell Primer`_ by Phidgets, Inc.
- `Load cell glossary`_
- `How to use load cells`_ from A&D
- `Load cell and weight module handbook`_ by Rice Lake Weighing Systems
- `Load cell troubleshooting from VPG Sensors`_
- `Load cell troubleshooting from Interface`_


.. _How to use load cells: http://www.aandd.jp/products/weighing/loadcell/introduction/pdf/6-1.pdf
.. _Load cell Primer: http://www.phidgets.com/docs/Load_Cell_Primer
.. _Load cell glossary: http://www.interfaceforce.com/index.php?mod=library&show=22
.. _Load cell and weight module handbook: https://www.ricelake.com/media/b5mlti1q/m_us_22054_lc-wm_handbook_revb.pdf
.. _Load cell troubleshooting from VPG Sensors: http://www.vishaypg.com/docs/11867/vpg-08.pdf
.. _Load cell troubleshooting from Interface: http://www.loadcelltheory.com/troubleshoot.html
