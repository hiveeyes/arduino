```{eval-rst}
.. include:: ../resources.rst
```

(firmware-overview-ng)=
# Firmware overview NG

There are different firmwares for different flavours of sensor appliances,
both in terms of sensor coverage, and telemetry capabilities.

```{contents}
:local:
:depth: 2
:class: this-will-duplicate-information-and-it-is-still-useful-here
```


## Features

### Telemetry
- [Cellular]: [GSM], [GPRS] uplink.
- [ISM radio]: RF packet radio links, communicating to other relay and network
  gateway devices.
- [ISM radio]: [LoRa] and [LoRaWAN] telemetry.
- Classic [Wi-Fi].


## Groups

This section lists firmwares by groups. See the legacy [](#firmware-overview) for
an alternative enumeration.

### Open Hive

All systems conceived by Open Hive.

#### Systems
- [GPRS sensor node](#open-hive-firmware-gprs)
- [](#scale-adjust-firmware)
- [](#open-hive-firmware-rfm69-node)
- [](#open-hive-firmware-rfm69-gateway-sdcard)
- [](#open-hive-firmware-rfm69-gateway-uart)

:::{note} **References:**
A few pointers to Clemens Gruber's and Markus Euskirchen's publications about
the development history.
- http://open-hive.org/
- https://www.euse.de/wp/blog/series/bienenwaage/
- https://www.euse.de/wp/blog/series/bienenwaage2/
:::

### Backdoor
- [Sensor and gateway nodes using BERadio](#firmwares-beradio)
- [](#multihop-firmware)
- [WiFi sensor node](#node-wifi-mqtt)

### Homie-based
- [WiFi sensor node based on Homie](#node-wifi-mqtt-homie)

### Trudering
- [](#bienenwaage-2.0)
- [](#bienenwaage-5.0)


:::{note}
## Wi-Fi

:::{todo}
As an example, link to [ESP8266 framework for Homie]. However, resolving
it apparently does not work, maybe because it is defined within a
[reStructuredText] file?
:::
:::


```{toctree}
---
caption: Firmwares
maxdepth: 1
hidden:
---
homie/node-homie-basic/README.rst
homie/node-homie-lowpower/README.rst
trudering/bienenwaage-2.0/README.rst
trudering/bienenwaage-5.0/README.rst
backdoor/node-pipa/README
```
