(firmware-overview)=
# Firmware overview

```{include} ../resources-md.md
```

There are different firmwares for different flavours of appliances,
both in terms of sensor coverage, and telemetry capabilities.
We are enumerating both firmwares from our own pen, and from other
open source projects.

## Groups

This section lists firmwares by group / origin, alphabetically and
chronologically sorted.

### Backdoor

Those firmwares have been conceived by members of the [Backdoor collective].
:::{note}
_No worries, it is not about breaking into systems, it is only the name
of a collective_.
:::

:::::::{grid} 1
:margin: 0
:padding: 0

::::::{grid-item-card}
:::::{grid} 2
:margin: 0
:padding: 0

::::{grid-item}
:columns: 8
**ISM RF sensor and gateway nodes using BERadio**

The sensor device will acquire measurement readings, and emit messages
encoded with [](inv:beradio:std:label#beradio) protocol for [ISM radio]
telemetry. The gateway device receives it, and forwards it to the UART
interface of the gateway machine.

:::{note}
[beradio-python](inv:beradio#beradio-python) will read this data, decodes the
[](inv:beradio:std:label#beradio) protocol, and publishes the decoded data to an
[MQTT] bus topic.
[](inv:hiveeyes#hiveeyes-one) is running this firmware since February 2016.
:::

**Date:** 2014
**Source:** [node-rfm69-beradio.ino], [gateway-rfm69-beradio.ino]
::::
::::{grid-item}
:columns: 4
{bdg-primary-line}`rf69` {bdg-primary-line}`beradio` {bdg-primary-line}`mqtt`

{bdg-success-line}`hx711` {bdg-success-line}`ds18b20` {bdg-success-line}`dht22`

{bdg-secondary-line}`ATmega328`

{bdg-info-line}`low-power`
::::
:::::
::::::

::::::{grid-item-card}
:::::{grid} 2
:margin: 0
:padding: 0

::::{grid-item}
:columns: 8
[](#multihop-firmware)

A flexible software breadboard for ISM RF packet radio nodes, relays, and gateways.

**Date:** 2015
**Source:** [multihop.ino]
::::
::::{grid-item}
:columns: 4
{bdg-primary-line}`rf69` {bdg-primary-line}`rf96` {bdg-primary-line}`lora` {bdg-primary-line}`beradio`

{bdg-success-line}`hx711` {bdg-success-line}`ds18b20` {bdg-success-line}`dht22`

{bdg-secondary-line}`ATmega328`

{bdg-info-line}`low-power`
::::
:::::
::::::

::::::{grid-item-card}
:::::{grid} 2
:margin: 0
:padding: 0

::::{grid-item}
:columns: 8
[Wi-Fi sensor node](#node-wifi-mqtt)

An educational/testing firmware powering the Open Hive [Adafruit Feather HUZZAH],
transmitting sensor data using [MQTT] over [Wi-Fi].

**Date:** 2016
**Source:** [node-wifi-mqtt.ino]
::::
::::{grid-item}
:columns: 4
{bdg-primary-line}`wifi` {bdg-primary-line}`mqtt` {bdg-primary-line}`json`

{bdg-success-line}`hx711` {bdg-success-line}`ads1231` {bdg-success-line}`ds18b20` {bdg-success-line}`dht22`

{bdg-secondary-line}`esp8266`

{bdg-info-line}`education`
::::
:::::
::::::

::::::{grid-item-card}
:::::{grid} 2
:margin: 0
:padding: 0

::::{grid-item}
:columns: 8
[](#pipa-datalogger)

A playground for building a generic C++ datalogger framework.

**Date:** 2017
**Source:** [pipa-espressif.ino]
::::
::::{grid-item}
:columns: 4
{bdg-primary-line}`wifi` {bdg-primary-line}`gsm` {bdg-primary-line}`mqtt` 
{bdg-primary-line}`http` {bdg-primary-line}`json` {bdg-primary-line}`csv` {bdg-primary-line}`urlencoded`

{bdg-secondary-line}`esp8266` {bdg-secondary-line}`esp32` {bdg-secondary-line}`esp32s2`

{bdg-info-line}`pre-alpha`
::::
:::::
::::::

:::::::


### Homie-based

:::{note} **References:**
The Homie-based firmwares have been conceived by Alexander Wilms.
- https://www.imker-nettetal.de/esp8266-beescale-erste-eindruecke/
- https://community.hiveeyes.org/t/welcome-alex/189
- https://community.hiveeyes.org/t/inbetriebnahme-von-node-wifi-mqtt-homie-mit-hiveeyes-anbindung/185
- https://imker-nettetal.de/nsa-for-bees/
:::

::::::{grid} 1
:margin: 0
:padding: 0

:::::{grid-item-card}
::::{grid} 2
:margin: 0
:padding: 0

:::{grid-item}
:columns: 8
[Wi-Fi sensor node based on Homie](#node-wifi-mqtt-homie)

Powers the »[ESP8266 beehive scale by Alexander Wilms][esp8266-beescale-erste-eindruecke]«.
See also the low-power variant [](#node-homie-lowpower).

**Date:** 2017
**Source:** [node-homie-basic.ino]
:::
:::{grid-item}
:columns: 4
{bdg-primary-line}`wifi` {bdg-primary-line}`mqtt` {bdg-primary-line}`json`

{bdg-success-line}`hx711` {bdg-success-line}`ds18b20`

{bdg-secondary-line}`esp8266`
:::
::::
:::::

:::::{grid-item-card}
::::{grid} 2
:margin: 0
:padding: 0

:::{grid-item}
:columns: 8
[](#node-homie-lowpower)

Low-power variant of [](#node-homie-basic).

**Date:** 2017
**Source:** [node-homie-lowpower.ino]
:::
:::{grid-item}
:columns: 4
{bdg-primary-line}`wifi` {bdg-primary-line}`mqtt` {bdg-primary-line}`json`

{bdg-success-line}`hx711` {bdg-success-line}`ds18b20`

{bdg-secondary-line}`esp8266`

{bdg-info-line}`low-power`
:::
::::
:::::

::::::


### Open Hive

All systems conceived by Open Hive and friends.

:::{note} **References:**
Markus Euskirchen and Clemens Gruber reported about the development progress.
- http://open-hive.org/
- https://www.euse.de/wp/blog/series/bienenwaage/
- https://www.euse.de/wp/blog/series/bienenwaage2/
:::

#### GSM, Wi-Fi

::::::{grid} 1
:margin: 0
:padding: 0

:::::{grid-item-card}
::::{grid} 2
:margin: 0
:padding: 0

:::{grid-item}
:columns: 8
[Open Hive Yún](#openhive-yun)

Running the [Mois Box], transmitting sensor data over HTTP, using the
[Bridge Library for Yún devices].

**Date:** 2014
**Source:** [openhive-yun.ino]
:::
:::{grid-item}
:columns: 4
{bdg-primary-line}`wifi` {bdg-primary-line}`mqtt` {bdg-primary-line}`http` {bdg-primary-line}`json`

{bdg-success-line}`ads1231` {bdg-success-line}`ds18b20` {bdg-success-line}`dht22` {bdg-success-line}`tsl2591`

{bdg-secondary-line}`ATmega328` {bdg-secondary-line}`arm`
:::
::::
:::::

:::::{grid-item-card}
::::{grid} 2
:margin: 0
:padding: 0

:::{grid-item}
:columns: 8
[Open Hive GSM / Wi-Fi sensor node](#openhive-gsm-wifi)

The firmware source code powering the [Open Hive Box] and [Open Hive WiFi Solar],
using the [GPRSbee] GSM modem.

**Date:** 2015
**Source:** [openhive-gsm-wifi.ino]
:::
:::{grid-item}
:columns: 4
{bdg-primary-line}`gsm` {bdg-primary-line}`wifi` {bdg-primary-line}`http` {bdg-primary-line}`csv`

{bdg-success-line}`hx711` {bdg-success-line}`ads1231` {bdg-success-line}`ds18b20` {bdg-success-line}`dht22`

{bdg-secondary-line}`ATmega328` {bdg-secondary-line}`esp8266`

{bdg-info-line}`low-power`
:::
::::
:::::

:::::{grid-item-card}
::::{grid} 2
:margin: 0
:padding: 0

:::{grid-item}
:columns: 8
[](#scale-adjust-firmware)

Use these firmwares for adjusting the scale / load cell unit.

**Date:** 2016
**Source:** [scale-adjust-hx711.ino], [scale-adjust-ads1231.ino]
:::
:::{grid-item}
:columns: 4
{bdg-success-line}`hx711` {bdg-success-line}`ads1231`

{bdg-secondary-line}`ATmega328` {bdg-secondary-line}`esp8266` {bdg-secondary-line}`esp32` {bdg-secondary-line}`cortex-m`
:::
::::
:::::

::::::


#### ISM RF packet radio
::::::{grid} 1
:margin: 0
:padding: 0

:::::{grid-item-card}
::::{grid} 2
:margin: 0
:padding: 0

:::{grid-item}
:columns: 8
[](#openhive-rfm69-gateway-uart)

Receive sensor data over radio and forward it to the UART interface suitable for
further downstreaming to [MQTT], using the [beradio-python](inv:beradio#beradio-python)
gateway.

**Date:** 2014
**Source:** [rfm69-gateway-uart.ino]
:::
:::{grid-item}
:columns: 4
{bdg-primary-line}`rf69` {bdg-primary-line}`uart` {bdg-primary-line}`csv`

{bdg-secondary-line}`ATmega328`
:::
::::
:::::

:::::{grid-item-card}
::::{grid} 2
:margin: 0
:padding: 0

:::{grid-item}
:columns: 8
[](#openhive-rfm69-gateway-sdcard)

Receive sensor data over radio and store it onto an SD card on an Arduino Yún.

**Date:** 2015
**Source:** [rfm69-gateway-sdcard.ino]
:::
:::{grid-item}
:columns: 4
{bdg-primary-line}`rf69` {bdg-primary-line}`sdcard` {bdg-primary-line}`csv`

{bdg-secondary-line}`ATmega328`
:::
::::
:::::

:::::{grid-item-card}
::::{grid} 2
:margin: 0
:padding: 0

:::{grid-item}
:columns: 8
[](#openhive-rfm69-node)

Collect sensor data, encode it to [CSV], and send it using an [RFM69] radio module.

**Date:** 2015
**Source:** [rfm69-node.ino]
:::
:::{grid-item}
:columns: 4
{bdg-primary-line}`rf69` {bdg-primary-line}`csv`

{bdg-success-line}`ads1231` {bdg-success-line}`ds18b20` {bdg-success-line}`dht22`

{bdg-secondary-line}`ATmega328`
:::

::::
:::::

::::::

#### LoRa, LoRaWAN

:::{todo}
- Are there any Open Hive [LoRa]/[LoRaWAN] firmwares available already?
:::


### Ringlabs


:::::::{grid} 1
:margin: 0
:padding: 0

::::::{grid-item-card}
:::::{grid} 2
:margin: 0
:padding: 0

::::{grid-item}
:columns: 8
[](#bienenwaage-2.0)

A beehive monitoring sensor node based on the [NodeMCU] hardware, using an
[ESP8266] MCU. Telemetry data is transmitted using MQTT over Wi-Fi or GSM.

**Date:** 2019
**Source:** [bienenwaage-esp8266.ino]
::::
::::{grid-item}
:columns: 4
{bdg-primary-line}`wifi` {bdg-primary-line}`gsm` {bdg-primary-line}`mqtt`

{bdg-success-line}`hx711` {bdg-success-line}`ds18b20`

{bdg-secondary-line}`esp8266`

{bdg-info-line}`low-power`
:::::
::::::

::::::{grid-item-card}
:::::{grid} 2
:margin: 0
:padding: 0

::::{grid-item}
:columns: 8
[](#bienenwaage-5.0)

A beehive monitoring sensor node based on the [TTGO T-Call] board, featuring
an [ESP32] MCU. Telemetry data is transmitted using MQTT over Wi-Fi or GSM.

**Date:** 2019
**Source:** [bienenwaage-esp32.cpp]
::::
::::{grid-item}
:columns: 4
{bdg-primary-line}`wifi` {bdg-primary-line}`gsm` {bdg-primary-line}`mqtt`

{bdg-success-line}`hx711` {bdg-success-line}`ds18b20`

{bdg-secondary-line}`esp32`

{bdg-info-line}`low-power`
:::::
::::::

:::::::


```{toctree}
---
caption: Firmwares
maxdepth: 1
hidden:
---
backdoor/multihop/README
backdoor/node-pipa/README
backdoor/node-wifi-mqtt/README
homie/node-homie-basic/README
homie/node-homie-lowpower/README
openhive/openhive-gsm-wifi/README
openhive/openhive-yun/README
openhive/rfm69-gateway-sdcard/README
openhive/rfm69-gateway-uart/README
openhive/rfm69-node/README
openhive/scale-adjust/README
ringlabs/bienenwaage-2.0/README
ringlabs/bienenwaage-5.0/README
```
