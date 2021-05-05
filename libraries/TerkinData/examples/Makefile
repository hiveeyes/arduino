$(eval venvpath  := $(PWD)/.venv)
$(eval python    := $(venvpath)/bin/python)
$(eval pip       := $(venvpath)/bin/pip)
$(eval pio       := $(venvpath)/bin/pio)

export PLATFORMIO_WORKSPACE_DIR := $(PWD)/.pio

build: setup-virtualenv
	cd json; $(pio) run --project-conf=../platformio.ini # --verbose
	cd csv; $(pio) run --project-conf=../platformio.ini # --verbose
	cd urlencoded; $(pio) run --project-conf=../platformio.ini # --verbose

upload: setup-virtualenv
	$(pio) run --target upload --upload-port=${MCU_PORT}

run:
	mkdir -p build
	./compile csv/csv_basic.cpp
	./compile json/json_basic.cpp
	./compile urlencoded/urlencoded_basic.cpp
	@echo

	./build/csv_basic
	@echo
	./build/json_basic
	@echo
	./build/urlencoded_basic
	@echo

setup-virtualenv:
	@test -e $(python) || `command -v virtualenv` --python=python3 $(venvpath)
	$(pip) install platformio
