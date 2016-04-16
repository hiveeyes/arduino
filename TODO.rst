#####################
Hiveeyes Arduino Todo
#####################


Code
====
- [o] Add Arduino gateway code, e.g. ``gateway-rfm69-beradio``
- [o] Add ``node-wifi-mqtt`` from Clemens
- [o] Add :ref:`libberadio` spike and associated documentation from :ref:`beradio`
- [o] Add HX711 calibration sketch


Documentation
=============
- [x] Add Sphinx for documentation
- [o] Purge docs/build from git repository
- [o] Publish at https://hiveeyes.org/docs/arduino/
- [o] Interlink Sphinx (conf.py) with swarm, beradio and maybe kotori docs
- [o] Add "Howto use inotool" to README
- [o] Check out different Sphinx theme


Infrastructure
==============
For more tooling like distribution package building, see :ref:`beradio` and :ref:`kotori` repositories

- [x] Add Makefile for convenience
- [x] Add bumpversion for release cutting
- [o] Project packaging (tarball, zip?) and publishing
- [o] Add ``git push github master`` to ``make release``. Don't miss the tags!
