# makeEspArduino: A makefile for ESP8266/Arduino projects.
# https://github.com/plerup/makeEspArduino

# Announce path to your "ESP8266 Arduino Core" installation
# Can also be announced via environment variable: export ESP_ROOT=...
# ESP_ROOT=~/sdk/esp8266-arduino

# Manually enumerate libraries, this is currently the only deterministic way.
LIBS = \
    $(ESP_LIBS)/esp8266 \
    $(ESP_LIBS)/GDBStub \
    $(ESP_LIBS)/SD \
    $(ESP_LIBS)/SPI \
    $(ESP_LIBS)/Wire \
    $(ESP_LIBS)/ESP8266WiFi \
    $(ESP_LIBS)/ESP8266HTTPClient \
    ../libraries/Adafruit_MQTT_Library/*.{h,cpp} \
    ../libraries/ArduinoJson/include \
    ../libraries/OneWire \
    ../libraries/DallasTemperature \
    ../libraries/RobTillaart-Arduino/libraries/DHTstable \
    ../libraries/RobTillaart-Arduino/libraries/RunningMedian \
    ../libraries/HX711

# Run makeEspArduino
include ../tools/makeEspArduino/makeEspArduino.mk
