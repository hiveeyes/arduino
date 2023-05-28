.DEFAULT_GOAL := build

$(eval venvpath  := $(PWD)/.venv)
$(eval python    := $(venvpath)/bin/python)
$(eval pip       := $(venvpath)/bin/pip)
$(eval pio       := $(venvpath)/bin/pio)

export PLATFORMIO_WORKSPACE_DIR := $(PWD)/.pio
export BUILD_DIR := $(PWD)/.build


# ==================================================
# Build all examples for all PlatformIO environments
# ==================================================

setup-virtualenv:
	@test -e $(python) || python3 -m venv $(venvpath)
	$(pip) --quiet install platformio

build: setup-virtualenv
	PLATFORMIO_SRC_DIR=csv $(pio) run  # --verbose
	PLATFORMIO_SRC_DIR=json $(pio) run  # --verbose
	PLATFORMIO_SRC_DIR=urlencoded $(pio) run  # --verbose

upload: setup-virtualenv
	$(pio) run --target upload --upload-port=${MCU_PORT}


# ==================================
# Build and run all examples (POSIX)
# ==================================

posix-build:
	./compile csv/csv_basic.cpp
	./compile json/json_basic.cpp
	./compile urlencoded/urlencoded_basic.cpp
	@echo

posix-run: posix-build
	${BUILD_DIR}/csv_basic
	@echo
	${BUILD_DIR}/json_basic
	@echo
	${BUILD_DIR}/urlencoded_basic
	@echo

run: posix-run


# =========================================
# Build and run individual examples (POSIX)
# =========================================

$(BUILD_DIR)/csv_basic:
	./compile csv/csv_basic.cpp
$(BUILD_DIR)/json_basic:
	./compile json/json_basic.cpp
$(BUILD_DIR)/urlencoded_basic:
	./compile urlencoded/urlencoded_basic.cpp

csv: $(BUILD_DIR)/csv_basic
	$(BUILD_DIR)/csv_basic
json: $(BUILD_DIR)/json_basic
	$(BUILD_DIR)/json_basic
urlencoded: $(BUILD_DIR)/urlencoded_basic
	$(BUILD_DIR)/urlencoded_basic
