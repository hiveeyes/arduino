```{eval-rst}
.. include:: ../resources.rst
```

(firmware-overview-ng)=
# Firmware overview NG

There are different firmwares for different flavours of sensor appliances,
both in terms of sensor coverage, and telemetry capabilities.

## Features

### Telemetry
- [Cellular]: [GSM], [GPRS] uplink.
- [ISM radio]: RF packet radio links, communicating to other relay and network
  gateway devices.
- [ISM radio]: [LoRa] and [LoRaWAN] telemetry.
- Classic [Wi-Fi].

```{contents}
:local:
:depth: 2
:class: this-will-duplicate-information-and-it-is-still-useful-here
```

## Wi-Fi

:::{todo}
As an example, link to [ESP8266 framework for Homie]. However, resolving
it apparently does not work, maybe because it is defined within a
[reStructuredText] file?
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
