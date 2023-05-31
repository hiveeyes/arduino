.. include:: ../../../resources.rst

.. _mois-node:
.. _mois-firmware:
.. _node-yun-http:
.. _beescale-yun:

######################
Mois Labs Beescale Yún
######################

`Documentation`_
| `Source code`_
| `Licenses`_
| `Issues`_
| `Hiveeyes Project`_
| `Hiveeyes Community Forum`_

.. _Documentation: https://hiveeyes.org/docs/arduino/firmware/moislabs/beescale-yun/README.html
.. _Hiveeyes Community Forum: https://community.hiveeyes.org/
.. _Hiveeyes Project: https://hiveeyes.org/
.. _Issues: https://github.com/hiveeyes/arduino/issues
.. _Licenses: https://hiveeyes.org/docs/arduino/project/licenses.html
.. _Source code: https://github.com/hiveeyes/arduino/tree/main/moislabs/beescale-yun

.. tip::

    Please `continue reading this document on our documentation space
    <https://hiveeyes.org/docs/arduino/firmware/moislabs/beescale-yun/README.html>`_,
    all inline links will be working there.


.. highlight:: bash

*****
About
*****

The second generation `Mois Box`_ is a beehive monitoring system based on the
`Arduino Yún Shield`_-compatible `Dragino Yun Shield v2.4`_, running `OpenWrt`_.

Project repository
==================

`@bee-mois`_ is maintaining the full sources, including all auxiliary helper programs,
within the `beescale repository`_.

Pictures
========

.. figure:: https://ptrace.hiveeyes.org/2017-03-21_mois-node-yun-http.jpg
    :alt: Mois Box with electronics
    :width: 350px
    :align: left

    `Mois Box`_

.. figure:: http://www.dragino.com/media/k2/galleries/105/YunShieldv2.4_10.png
    :target: https://wiki1.dragino.com/index.php/Yun_Shield
    :alt: Dragino Yun Shield v2.4
    :width: 350px
    :align: right

    `Dragino Yun Shield v2.4`_

|clearfix|



********
Hardware
********

Board
=====
- `Dragino Yun Shield v2.4`_

Sensors
=======
- ADS1231_ ADC weigh scale breakout board
- DS18B20_ digital thermometer
- `SHT30`_ digital humidity/temperature sensor
- `TSL25911`_ Ambient Light Sensor aka.
  `Adafruit TSL2591 STEMMA QT`_ High Dynamic Range Digital Light Sensor



********
Firmware
********

The most recent firmware version is available at `beescale-yun.ino`_.

Acquire source code
===================
::

    # Acquire source code repository
    git clone https://github.com/hiveeyes/arduino

    # Select this firmware
    cd arduino/moislabs/beescale-yun


Configure
=========
.. highlight:: c++

Have a look at the source code `beescale-yun.ino`_ and adapt feature flags and setting
variables according to your environment.

Configure load cell calibration settings::

    // Use sketches "scale-adjust-hx711.ino" or "scale-adjust-ads1231.ino" for calibration

    // The raw sensor value for "0 kg"
    const long loadCellZeroOffset = 38623;

    // The raw sensor value for a 1 kg weight load
    const long loadCellKgDivider  = 11026;

.. tip::

    Please use the corresponding :ref:`scale-adjust-firmware` to determine those values.

Build
=====

.. highlight:: bash

The build system is based on `PlatformIO`_, which will install toolchains and build
your customized firmware without efforts. All you need is a Python installation.

::

    make build

Upload to MCU
=============
::

    export MCU_PORT=/dev/ttyUSB0
    make upload


.. tip::

    If you need to build for different targets, or if you want to modernize your dependencies,
    you may want to adjust the ``platformio.ini`` file, to match your needs.

    In order to make changes to the firmware, edit the ``beescale-yun.ino`` file, and
    invoke ``make build`` to build it again.


*********************
Telemetry and backend
*********************


Overview
========

This flowchart will give you an idea how measurement data is acquired and processed
within the sensor domain, and how it will be converged to the network and submitted
to the backend systems.

.. Mermaid Flowcharts documentation: https://mermaid.js.org/syntax/flowchart.html

.. mermaid::

    %%{init: {"flowchart": {"defaultRenderer": "dagre", "htmlLabels": false}} }%%

    flowchart LR

      subgraph sensors [sensor domain]
        direction LR
        ATmega328P --> AR9331
      end

      subgraph network
        direction TB
        HTTP{HTTP}
        MQTT{MQTT}
      end

      subgraph backend
        direction LR
        PHP    --> CSV --> dygraphs
        Kotori --> InfluxDB --> Grafana
        PHP    --> Kotori
      end

      sensors --> network
      network --> backend


Transport
=========

To get the measurement data from the *sensor domain* to the *network*, the `Bridge Library
for Yún devices`_ enables communication between the `ATmega328P`_ MCU on the `Arduino Uno`_,
and the `AR9331`_, running `Linux`_. The library will enable transparent HTTP communication
through the venerable Arduino `HttpClient`_.

.. code-block:: c++

    #include <Bridge.h>
    #include <HttpClient.h>

    HttpClient client;
    client.post(url);

Using this HTTP client (example program at `Yún HTTP Client`_), telemetry data is transmitted
to a `custom PHP receiver program`_, and is also stored on the SD card attached to the device.

Reception and relay
===================

On the backend, the PHP program receives the telemetry data record, and stores it into two
different databases. First, it adds the record to a CSV file stored on the server's file
system, and second, it emits another HTTP request to the data acquisition server of our
beekeepers collective at https://swarm.hiveeyes.org/.

To learn more about how this works, please visit the documentation about `Daten zu Hiveeyes
übertragen`_, and the `Kotori message router and data historian`_.

Other than this, for publishing the webcam image, a cron job on the web server acquires the
latest image from the Yun's SD card every 15 minutes, and stores it on its own filesystem,
where HTTP clients are able to consume it without further ado.

Visualization
=============

The data in the CSV file is visualized using the `graph.php`_ program. It uses `dygraphs`_,
a JavaScript charts framework, and that's it.

On the other hand, when submitting data to `Kotori`_ on the collaborative data collection
server at https://swarm.hiveeyes.org/, it will store the data into `InfluxDB`_, and will
populate a `Grafana`_ dashboard correspondingly.


*********
Live data
*********

- https://www.euse.de/honig/beescale/graph.php
- https://www.euse.de/honig/beescale/graph_pure.php
- https://swarm.hiveeyes.org/grafana/d/5NpVD1qiz/mois-2-1-hives-overview-and-bee-weather
- https://www.euse.de/honig/beescale/latest.jpg



*********************
Contributions welcome
*********************

You can run parts of this, or the whole system, on your own hardware, for yourselves, or
as a service for your local beekeepers collective. If you want to report or fix a bug or
documentation flaw, or if you would like to suggest an improvement, feel free to `create
an issue`_, or `submit a patch`_. Thank you.

----

.. note::

    ::

        echo "Viel Spaß am Gerät"
        cat <<ZUSE

        Es hat viele Erfinder außer mir gebraucht, um den Computer, so wie wir ihn heute kennen, zu entwickeln.
        Ich wünsche der nachfolgenden Generation Alles Gute im Umgang mit dem Computer. Möge dieses Instrument
        Ihnen helfen, die Probleme dieser Welt zu beseitigen, die wir Alten Euch hinterlassen haben.

        -- Konrad Zuse

        ZUSE


.. _AR9331: http://en.techinfodepot.shoutwiki.com/wiki/Atheros_AR9331
.. _@bee-mois: https://github.com/bee-mois
.. _beescale repository: https://github.com/bee-mois/beescale
.. _create an issue: https://github.com/hiveeyes/arduino/issues
.. _custom PHP receiver program: https://github.com/bee-mois/beescale/blob/master/add_line2.php
.. _Daten zu Hiveeyes übertragen: https://community.hiveeyes.org/t/daten-per-http-und-php-ans-backend-auf-swarm-hiveeyes-org-ubertragen/162
.. _dygraphs: https://dygraphs.com/
.. _Grafana: https://en.wikipedia.org/wiki/Grafana
.. _graph.php: https://github.com/bee-mois/beescale/blob/master/graph.php
.. _HttpClient: https://www.arduino.cc/reference/en/libraries/httpclient/
.. _InfluxDB: https://en.wikipedia.org/wiki/InfluxDB
.. _Kotori message router and data historian: https://getkotori.org/
.. _Linux: https://en.wikipedia.org/wiki/Linux
.. _OpenWrt: https://en.wikipedia.org/wiki/OpenWrt
.. _submit a patch: https://github.com/hiveeyes/arduino/pulls
